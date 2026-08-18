#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "macro.h"
#include "utils.h"
#include <ctype.h>
/* 
 * Forward Declarations of Static Helper Functions
*/
/* Saves a new macro to the macro table. Returns TRUE on success, FALSE on error. */
static boolean save_new_macro(MacroNode **head, char *line, MacroNode **current_macro,int line_number);
/* Adds a line of code to the currently active macro. Returns TRUE on success. */
static boolean add_line_to_macro(MacroNode *macro, char *line);
/* Searches for a macro by name in the table. Returns a pointer to it, or NULL if not found. */
static MacroNode* find_macro(MacroNode *head, char *name);
/* Frees all dynamically allocated memory in the macro table to prevent leaks. */
void free_macro_table(MacroNode *head);


/* 
 * Main Macro Processing Function
*/

boolean process_macros(char *base_filename, MacroNode **out_macro_head) {
    FILE *file_as;
    FILE *file_am;
    char filename_as[MAX_LINE_LENGTH];
    char filename_am[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_LINE_LENGTH];
    char dummy[MAX_LINE_LENGTH];
    boolean error_found = FALSE;
    boolean inside_macro = FALSE;
    int line_number = 0;
    MacroNode *macro_head = NULL;    /* Head of the macro table linked list */
    MacroNode *current_macro = NULL; /* Pointer to the macro currently being defined */
    MacroNode *found_macro = NULL;   /* Pointer used when a macro call is found */
    MacroLine *current_line = NULL;  /* Pointer used for expanding macro lines */

    /* Create full file names with proper extensions */
    sprintf(filename_as, "%s.as", base_filename);
    sprintf(filename_am, "%s.am", base_filename);

    /* Open the original .as file for reading */
    file_as = fopen(filename_as, "r");
    if (file_as == NULL) {
        printf("Error: Could not open input file %s\n", filename_as);
        return FALSE;
    }

    /* Open the new .am file for writing */
    file_am = fopen(filename_am, "w");
    if (file_am == NULL) {
        printf("Error: Could not create output file %s\n", filename_am);
        fclose(file_as);
        return FALSE;
    }

    /* Read the file line by line */
   while (fgets(line, MAX_LINE_LENGTH, file_as) != NULL) {
        line_number++;
        if (strchr(line, '\n') == NULL && !feof(file_as)) {
            int c;
            printf("Error at line %d: Line exceeds maximum length of 80 characters.\n", line_number);
            error_found = TRUE; /* סימון שגיאה כדי לבטל יצירת פלט */
            /* מנקים (קוראים וזורקים) את שאר התווים עד שמגיעים לסוף השורה האמיתי */
            while ((c = fgetc(file_as)) != '\n' && c != EOF);
        }
        /* Extract the first word of the line safely (ignores leading whitespaces) */
        if (sscanf(line, "%s", first_word) != 1) {
            /* Empty line or just whitespaces - write as is if not inside macro */
            if (!inside_macro) {
                fputs(line, file_am);
            } else {
                add_line_to_macro(current_macro, line);
            }
            continue;
        }

        /* Case 1: Start of a new macro definition */
        if (strcmp(first_word, "mcro") == 0) {
            if (inside_macro) {
               printf("Error at line %d: Nested macros are not allowed.\n", line_number);
                error_found = TRUE;
                continue;
            }
            
            if (save_new_macro(&macro_head, line, &current_macro,line_number) == FALSE) {
                error_found = TRUE;
            }
            inside_macro = TRUE;
            continue;
        }

        /* Case 2: End of a macro definition */
        if (strcmp(first_word, "mcroend") == 0) {
            if (!inside_macro) {
               /* printf("Error at line %d: 'mcroend' encountered without a preceding 'mcro'.\n", line_number);*/
                fputs(line, file_am);
                continue;
            }
            
            /* Check if there is extra garbage after 'mcroend' */
            /* Using a dummy variables to check if a second word exists */

            if (sscanf(line, "%*s %s", dummy) == 1) {
               printf("Error at line %d: Extraneous text after 'mcroend'.\n", line_number);
                error_found = TRUE;
            }
            
            inside_macro = FALSE;
            current_macro = NULL;
            continue;
        }

        /* Case 3: We are inside a macro definition, save the line */
        if (inside_macro) {
            if (add_line_to_macro(current_macro, line) == FALSE) {
                error_found = TRUE;
            }
            continue;
        }

        /* Case 4: Not inside a macro. Check if the first word is a macro call */
        found_macro = find_macro(macro_head, first_word);
        
        if (found_macro != NULL) {
            /* Macro found! Expand it by writing its stored lines to the .am file */
            current_line = found_macro->content;
            while (current_line != NULL) {
                fputs(current_line->line, file_am);
                current_line = current_line->next;
            }
        } else {
            /* Regular line of code, write it directly to the .am file */
            fputs(line, file_am);
        }
    }

   /* Close files */
    fclose(file_as);
    fclose(file_am);

    /* If any syntax or memory error was found, do not output a valid .am file */
    if (error_found == TRUE) {
        printf("Errors found during macro expansion in file '%s.as'. Output cancelled.\n", base_filename);
        remove(filename_am); /* Delete the potentially corrupted .am file */
        
        /* If there was an error, we MUST free the macro table since we failed */
        free_macro_table(macro_head); 
        
        return FALSE;
    }

    /* NOTE: If we reached here, macro expansion succeeded! 
     * We intentionally DO NOT call free_macro_table(macro_head) here, 
     * because the macro table must stay in memory for Pass 1 to check for label conflicts. 
     */
*out_macro_head = macro_head;
    return TRUE;
  }

/* 
 * Helper Functions Implementation
 */
static boolean save_new_macro(MacroNode **head, char *line, MacroNode **current_macro, int line_number) {
    char keyword[MAX_LINE_LENGTH];
    char macro_name[MAX_LINE_LENGTH];
    char garbage[MAX_LINE_LENGTH];
    int parsed_items;
    int i;
    MacroNode *new_node;
    MacroNode *current;
   
    /* Extract words from the line: expecting exactly "mcro" and "macro_name" */
    parsed_items = sscanf(line, "%s %s %s", keyword, macro_name, garbage);

    if (parsed_items < 2) {
        printf("Error at line %d: Missing macro name in definition.\n", line_number);
        return FALSE;
    }
    if (parsed_items == 3) {
        printf("Error at line %d: Extraneous text after macro name '%s'.\n", line_number, macro_name);
        return FALSE;
    }
    for (i = 0; macro_name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)macro_name[i])&& macro_name[i] != '_') {
            printf("Error at line %d: Invalid macro name '%s' (contains invalid characters).\n", line_number, macro_name);
            return FALSE;
        }
    }
    /* Validation: Check if the macro name is a reserved assembly word */
    if (is_reserved_word(macro_name) == TRUE) {
        printf("Error at line %d: Macro name '%s' is a reserved word.\n", line_number, macro_name);
        return FALSE;
    }

    /* Validation: Check if a macro with this name already exists */
    current = *head;
    while (current != NULL) {
        if (strcmp(current->name, macro_name) == 0) {
            printf("Error at line %d: Macro '%s' is already defined.\n", line_number, macro_name);
            return FALSE;
        }
        current = current->next;
    }

    /* Allocate memory for the new macro node */
    new_node = (MacroNode *)malloc(sizeof(MacroNode));
    if (new_node == NULL) {
        printf("Error at line %d: Memory allocation failed for macro '%s'!\n", line_number, macro_name);
        return FALSE;
    }

    /* Initialize node */
    strcpy(new_node->name, macro_name);
    new_node->content = NULL;
    new_node->next = NULL;

    /* Append to the linked list */
    if (*head == NULL) {
        *head = new_node;
    } else {
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    /* Update the current_macro pointer so add_line_to_macro knows where to add lines */
    *current_macro = new_node;
    
    return TRUE;
}

static boolean add_line_to_macro(MacroNode *macro, char *line) {
    MacroLine *new_line;
    MacroLine *current_line;

    if (macro == NULL) return FALSE;

    /* Allocate memory for the new line */
    new_line = (MacroLine *)malloc(sizeof(MacroLine));
    if (new_line == NULL) {
        printf("Error: Memory allocation failed for a macro line.\n");
        return FALSE;
    }

    /* Initialize the new line */
    strcpy(new_line->line, line);
    new_line->next = NULL;

    /* Append to the macro's internal linked list of lines */
    if (macro->content == NULL) {
        macro->content = new_line;
    } else {
        current_line = macro->content;
        while (current_line->next != NULL) {
            current_line = current_line->next;
        }
        current_line->next = new_line;
    }

    return TRUE;
}

static MacroNode* find_macro(MacroNode *head, char *name) {
    MacroNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; /* Macro found */
        }
        current = current->next;
    }
    return NULL; /* Macro not found */
}

void free_macro_table(MacroNode *head) {
    MacroNode *current_macro = head;
    MacroNode *next_macro;
    MacroLine *current_line;
    MacroLine *next_line;

    while (current_macro != NULL) {
        next_macro = current_macro->next;

        /* Free all line nodes inside the current macro */
        current_line = current_macro->content;
        while (current_line != NULL) {
            next_line = current_line->next;
            free(current_line);
            current_line = next_line;
        }

        /* Free the macro node itself */
        free(current_macro);
        current_macro = next_macro;
    }
}

