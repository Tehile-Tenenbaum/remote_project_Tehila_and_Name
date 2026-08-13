#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "macro.h"

/*
 * Checks if a given word is a reserved assembly word.
 */
boolean is_reserved_word(char *word) {
    int i;
    /* Array of all reserved words according to the course syllabus */
    char *reserved_words[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", 
        "inc", "dec", "jmp", "bne", "red", "prn", "jsr", 
        "rts", "stop", ".data", ".string", ".entry", ".extern",
        "mcro", "mcroend"
    };
    int num_reserved = sizeof(reserved_words) / sizeof(reserved_words[0]);

    for (i = 0; i < num_reserved; i++) {
        if (strcmp(word, reserved_words[i]) == 0) {
            return TRUE; /* Match found - it is a reserved word */
        }
    }
    return FALSE; /* Safe to use */
}
/*
 * Advances the index to skip any spaces or tabs.
 */
void skip_white_spaces(char *line, int *index) {
    while (line[*index] == ' ' || line[*index] == '\t') {
        (*index)++;
    }
}
/*
 * Checks if there are any extra characters (garbage) left in the line.
 */
boolean check_no_garbage(char *line, int current_index) {
    skip_white_spaces(line, &current_index);
    /* If we reached the end of the string or a newline, it's clean */
    if (line[current_index] == '\0' || line[current_index] == '\n' || line[current_index] == '\r') {
        return TRUE; 
    }
    return FALSE; /* Garbage characters found */
}
boolean is_macro_name(char *name, MacroNode *macro_head) {
    MacroNode *current = macro_head;
    
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return TRUE; /* השם כבר תפוס על ידי מאקרו */
        }
        current = current->next;
    }
    return FALSE;
}
/*
 * בודקת נטו את התקינות התחבירית של שם התווית.
 * מחזירה TRUE אם השם תקין, ו-FALSE אם לא.
 */
boolean is_valid_label_name(char *name) {
    int i;

    /* 1. אורך מקסימלי (31 תווים) ומניעת מחרוזת ריקה */
    if (name == NULL || strlen(name) == 0 || strlen(name) > MAX_LABEL_LENGTH) {
        return FALSE;
    }

    /* 2. התו הראשון חייב להיות אות (אנגלית, גדולה או קטנה) */
    if (!isalpha(name[0])) {
        return FALSE;
    }

    /* 3. שאר התווים חייבים להיות רק אותיות או ספרות */
    for (i = 1; i < strlen(name); i++) {
        if (!isalnum(name[i])) {
            return FALSE;
        }
    }

    /* אם הגענו לפה, השם תקין תחבירית */
    return TRUE; 
}


