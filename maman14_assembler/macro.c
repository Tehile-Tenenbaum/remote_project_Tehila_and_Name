#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "macro.h"

/* הצהרות על פונקציות עזר פנימיות (פרטיות לקובץ זה) */
static void save_new_macro(MacroNode **head, char *line);
static void add_line_to_macro(MacroNode *head, char *line);
static int expand_macro(MacroNode *head, char *line, FILE *file_am);
static void free_macro_list(MacroNode *head);

boolean process_macros(char *base_filename) {
    FILE *file_as;
    FILE *file_am;
    char filename_as[MAX_LINE_LENGTH];
    char filename_am[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    
    int inside_macro = 0; 
    /* הנה ההצהרה על ראש הרשימה המקושרת שלנו! */
    MacroNode *macro_head = NULL;
    
    /* בהמשך נגדיר כאן מצביע לרשימה המקושרת שלנו שתחזיק את המקרואים */
    /* MacroNode *macro_head = NULL; */

    sprintf(filename_as, "%s.as", base_filename);
    sprintf(filename_am, "%s.am", base_filename);

    file_as = fopen(filename_as, "r");
    if (file_as == NULL) {
        printf("Error: Could not open file %s\n", filename_as);
        return FALSE;
    }

    file_am = fopen(filename_am, "w");
    if (file_am == NULL) {
        printf("Error: Could not create file %s\n", filename_am);
        fclose(file_as); 
        return FALSE;
    }

    while (fgets(line, MAX_LINE_LENGTH, file_as) != NULL) {

        /* 1. התחלת מקרו - מצאנו את המילה mcro */
        if (strncmp(line, "mcro", 4) == 0) {
            inside_macro = 1;
            /* כאן תבוא הפונקציה ששולפת את שם המקרו מהשורה ופותחת עבורו מקום בזיכרון */
            /* save_new_macro(&macro_head, line); */
            save_new_macro(&macro_head, line);
            continue; 
        }

        /* 2. סיום מקרו - מצאנו endmcro */
        if (strncmp(line, "endmcro", 7) == 0) {
            inside_macro = 0;
            continue; 
        }

        /* 3. שמירת תוכן המקרו - אנחנו בתוך בלוק של מקרו */
        if (inside_macro == 1) {
            /* כאן תבוא הפונקציה שמוסיפה את השורה הזו לתוך המקרו האחרון שפתחנו */
            /* add_line_to_macro(macro_head, line); */
            add_line_to_macro(macro_head, line);
            continue;
        }

       /* 4. פריסת מקרו או כתיבת שורה רגילה */
        if (expand_macro(macro_head, line, file_am) == 1) {
            /* הפונקציה זיהתה שזה מקרו, הדפיסה אותו, ולכן אפשר לעבור לשורה הבאה */
            continue;
        }
            
            /* שורה רגילה לחלוטין - מדפיסים לקובץ */
            fputs(line, file_am);
            
        /* } */
    }

    fclose(file_as);
    fclose(file_am);
    
    /* שחרור הזיכרון של כל הרשימה המקושרת */
    free_macro_list(macro_head);
    
    return TRUE;
}

void save_new_macro(MacroNode **head, char *line) {
    char macro_name[MAX_LABEL_LENGTH];
    MacroNode *new_node;
    MacroNode *current;

    /* 1. חילוץ שם המקרו מתוך השורה */
    sscanf(line, "mcro %s", macro_name);

    /* 2. הקצאת זיכרון דינמית לצומת החדש */
    new_node = (MacroNode *)malloc(sizeof(MacroNode));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed!\n");
        return;
    }

    /* 3. מילוי הנתונים בתוך הצומת החדש */
    strcpy(new_node->name, macro_name); /* העתקת השם שחילצנו */
    new_node->content = NULL;           /* עדיין אין שורות תוכן, לכן מצביע ל-NULL */
    new_node->next = NULL;              /* כרגע זה המקרו האחרון ברשימה */

    /* 4. חיבור הצומת החדש לרשימה המקושרת שלנו */
    if (*head == NULL) {
        /* מצב א': הרשימה ריקה לגמרי. הצומת החדש הופך לראש הרשימה */
        *head = new_node;
    } else {
        /* מצב ב': כבר יש מקרואים. נרוץ עד לסוף הרשימה ונוסיף אותו שם */
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}
void add_line_to_macro(MacroNode *head, char *line) {
    MacroNode *current_macro = head;
    MacroLine *new_line;
    MacroLine *current_line;

    /* הגנה קטנה: אם אין שום מקרו ברשימה, אי אפשר להוסיף שורה */
    if (current_macro == NULL) {
        return; 
    }

    /* 1. מציאת המקרו הפעיל (זה שתמיד נמצא בסוף רשימת המקרואים) */
    while (current_macro->next != NULL) {
        current_macro = current_macro->next;
    }

    /* 2. הקצאת זיכרון לשורת הקוד החדשה (הקרון) */
    new_line = (MacroLine *)malloc(sizeof(MacroLine));
    if (new_line == NULL) {
        printf("Error: Memory allocation failed for macro line!\n");
        return;
    }

    /* 3. העתקת טקסט השורה לתוך הזיכרון החדש שהקצינו */
    strcpy(new_line->line, line);
    new_line->next = NULL; /* זו השורה האחרונה כרגע, אז אין לה המשך */

    /* 4. חיבור השורה החדשה לתוך המקרו שמצאנו */
    if (current_macro->content == NULL) {
        /* מצב א': זו שורת הקוד הראשונה בתוך המקרו הזה */
        current_macro->content = new_line;
    } else {
        /* מצב ב': כבר יש שורות קוד במקרו הזה, נרוץ עד השורה האחרונה ונוסיף אחריה */
        current_line = current_macro->content;
        while (current_line->next != NULL) {
            current_line = current_line->next;
        }
        current_line->next = new_line;
    }
}

static int expand_macro(MacroNode *head, char *line, FILE *file_am) {
    char first_word[MAX_LINE_LENGTH];
    MacroNode *current_macro = head;
    MacroLine *current_line;

    /* 1. שליפת המילה הראשונה בשורה (כדי לבדוק אם היא קריאה למקרו) */
    if (sscanf(line, "%s", first_word) != 1) {
        return 0; /* אם השורה ריקה, זה בטוח לא מקרו */
    }

    /* 2. ריצה על רשימת המקרואים כדי לחפש התאמה */
    while (current_macro != NULL) {
        
        /* השוואה בין המילה הראשונה בשורה לבין שם המקרו הנוכחי */
        if (strcmp(current_macro->name, first_word) == 0) {
            
            /* מצאנו התאמה! עכשיו נדפיס את כל השורות שלו לקובץ */
            current_line = current_macro->content;
            current_line = current_macro->content;
            while (current_line != NULL) {
                /* נבדוק האם שורת התוכן הזו בעצמה היא קריאה למקרו אחר! */
                if (expand_macro(head, current_line->line, file_am) == 0) {
                    /* אם היא לא מקרו, נדפיס אותה רגיל לקובץ */
                    fputs(current_line->line, file_am);
                }
                current_line = current_line->next;
            }
            
            /* מחזירים 1 כדי לאותת לתוכנית הראשית שהשורה הזו טופלה (נפרסה) */
            return 1; 
        }
        
        /* מעבר למקרו הבא ברשימה */
        current_macro = current_macro->next;
    }

    /* אם סיימנו לרוץ על כל הרשימה ולא מצאנו התאמה - זה לא מקרו */
    return 0;
}
static void free_macro_list(MacroNode *head) {
    MacroNode *temp_macro;
    MacroLine *temp_line;

    while (head != NULL) {
        temp_macro = head;
        head = head->next;

        /* שחרור כל שורות הקוד שנמצאות בתוך המקרו הנוכחי */
        while (temp_macro->content != NULL) {
            temp_line = temp_macro->content;
            temp_macro->content = temp_macro->content->next;
            free(temp_line);
        }

        /* שחרור צומת המקרו עצמו */
        free(temp_macro);
    }
}