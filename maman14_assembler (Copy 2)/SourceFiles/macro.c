#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "macro.h"
#include "utils.h"
#include "error.h"

/* definitions for macro parsing */
#define EXPECTED_MACRO_DEF_WORDS 2
#define MACRO_DEF_WORDS_WITH_GARBAGE 3

/* static helper functions */
static boolean save_new_macro(MacroNode **head, char *line, MacroNode **current_macro, int line_number, const char *filename);
static boolean add_line_to_macro(MacroNode *macro, char *line, int line_number, const char *filename);
static MacroNode* find_macro(MacroNode *head, const char *name);

/* main macro processing function */
boolean process_macros(const char *base_filename, MacroNode **out_macro_head) {
    FILE *file_as;
    FILE *file_am;
    char *filename_as;
    char *filename_am;
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_LINE_LENGTH];
    char dummy[MAX_LINE_LENGTH];
    
    boolean error_found = FALSE;
    boolean inside_macro = FALSE;
    int line_number = 0;
    size_t base_len;
    
    MacroNode *macro_head = NULL;    
    MacroNode *current_macro = NULL; 
    MacroNode *found_macro = NULL;   
    MacroLine *current_line = NULL;  

    /* alloc memory for the filenames with extensions */
    base_len = strlen(base_filename);
    filename_as = (char *)malloc(base_len + 4);
    filename_am = (char *)malloc(base_len + 4);
    
    if (filename_as == NULL || filename_am == NULL) {
        printf("Memory allocation failed for filenames.\n");
        if (filename_as) free(filename_as);
        if (filename_am) free(filename_am);
        return FALSE;
    }

    sprintf(filename_as, "%s.as", base_filename);
    sprintf(filename_am, "%s.am", base_filename);

    file_as = fopen(filename_as, "r");
    if (file_as == NULL) {
        report_error(filename_as, 0, "Could not open input file.");
        free(filename_as);
        free(filename_am);
        return FALSE;
    }

    file_am = fopen(filename_am, "w");
    if (file_am == NULL) {
        report_error(filename_am, 0, "Could not create output file.");
        fclose(file_as);
        free(filename_as);
        free(filename_am);
        return FALSE;
    }

    /* read line by line */
    while (fgets(line, MAX_LINE_LENGTH, file_as) != NULL) {
        line_number++;
        
        /* line too long check */
        if (strchr(line, '\n') == NULL && !feof(file_as)) {
            int c;
            report_error(filename_as, line_number, "Line exceeds maximum length of 80 characters.");
            error_found = TRUE; 
            
            /* clear buffer to end of line */
            while ((c = fgetc(file_as)) != '\n' && c != EOF);
        }
        
        /* grab first word */
        if (sscanf(line, "%s", first_word) != 1) {
            /* empty line or whitespaces */
            if (!inside_macro) {
                fputs(line, file_am);
            } else {
                add_line_to_macro(current_macro, line, line_number, filename_as);
            }
            continue;
        }

        /* found macro start */
        if (strcmp(first_word, "mcro") == 0) {
            if (inside_macro) {
                report_error(filename_as, line_number, "Nested macros are not allowed.");
                error_found = TRUE;
                continue;
            }
            
            if (save_new_macro(&macro_head, line, &current_macro, line_number, filename_as) == FALSE) {
                error_found = TRUE;
            }
            inside_macro = TRUE;
            continue;
        }

        /* found macro end */
        if (strcmp(first_word, "mcroend") == 0) {
            if (!inside_macro) {
                fputs(line, file_am);
                continue;
            }
            
            /* check garbage after mcroend */
            if (sscanf(line, "%*s %s", dummy) == 1) {
                report_error(filename_as, line_number, "Extraneous text after 'mcroend'.");
                error_found = TRUE;
            }
            
            inside_macro = FALSE;
            current_macro = NULL;
            continue;
        }

        /* inside macro, save the line */
        if (inside_macro) {
            if (add_line_to_macro(current_macro, line, line_number, filename_as) == FALSE) {
                error_found = TRUE;
            }
            continue;
        }

        /* check if the word is a macro call */
        found_macro = find_macro(macro_head, first_word);
        
        if (found_macro != NULL) {
            /* expand macro */
            current_line = found_macro->content;
            while (current_line != NULL) {
                fputs(current_line->line, file_am);
                current_line = current_line->next;
            }
        } else {
            /* regular code, write directly */
            fputs(line, file_am);
        }
    }

    fclose(file_as);
    fclose(file_am);

    /* if we found errors, delete the corrupted .am file */
    if (error_found == TRUE) {
        printf("Errors found during macro expansion in file '%s'. Output cancelled.\n", filename_as);
        remove(filename_am); 
        free_macro_table(macro_head); 
        free(filename_as);
        free(filename_am);
        return FALSE;
    }

    /* success */
    *out_macro_head = macro_head;
    free(filename_as);
    free(filename_am);
    return TRUE;
}

/* creates and saves a new macro node */
static boolean save_new_macro(MacroNode **head, char *line, MacroNode **current_macro, int line_number, const char *filename) {
    char keyword[MAX_LINE_LENGTH];
    char macro_name[MAX_LINE_LENGTH];
    char garbage[MAX_LINE_LENGTH];
    int parsed_items;
    int i;
    MacroNode *new_node;
    MacroNode *current;
   
    /* extract words */
    parsed_items = sscanf(line, "%s %s %s", keyword, macro_name, garbage);

    if (parsed_items < EXPECTED_MACRO_DEF_WORDS) {
        report_error(filename, line_number, "Missing macro name in definition.");
        return FALSE;
    }
    if (parsed_items == MACRO_DEF_WORDS_WITH_GARBAGE) {
        report_error(filename, line_number, "Extraneous text after macro name '%s'.", macro_name);
        return FALSE;
    }
    
    /* check max length */
    if (strlen(macro_name) >= MAX_LABEL_LENGTH) {
        report_error(filename, line_number, "Macro name '%s' exceeds the maximum allowed length of 31 characters.", macro_name);
        return FALSE;
    }

    /* must start with a letter */
    if (!isalpha((unsigned char)macro_name[0])) {
        report_error(filename, line_number, "Macro name '%s' must start with an alphabetic letter.", macro_name);
        return FALSE;
    }

    /* must be alphanumeric */
    for (i = 1; macro_name[i] != '\0'; i++) {
        if (!isalnum((unsigned char)macro_name[i]) && macro_name[i] != '_') {
            report_error(filename, line_number, "Invalid macro name '%s' (contains invalid characters).", macro_name);
            return FALSE;
        }
    }
    
    if (is_reserved_word(macro_name) == TRUE) {
        report_error(filename, line_number, "Macro name '%s' is a reserved word.", macro_name);
        return FALSE;
    }

    /* avoid duplicates */
    current = *head;
    while (current != NULL) {
        if (strcmp(current->name, macro_name) == 0) {
            report_error(filename, line_number, "Macro '%s' is already defined.", macro_name);
            return FALSE;
        }
        current = current->next;
    }

    new_node = (MacroNode *)malloc(sizeof(MacroNode));
    if (new_node == NULL) {
        report_error(filename, line_number, "Memory allocation failed for macro '%s'!", macro_name);
        return FALSE;
    }

    strcpy(new_node->name, macro_name);
    new_node->content = NULL;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
    } else {
        current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }

    *current_macro = new_node;
    return TRUE;
}

/* appends a new line to a macro's content */
static boolean add_line_to_macro(MacroNode *macro, char *line, int line_number, const char *filename) {
    MacroLine *new_line;
    MacroLine *current_line;

    if (macro == NULL) return FALSE;

    new_line = (MacroLine *)malloc(sizeof(MacroLine));
    if (new_line == NULL) {
        report_error(filename, line_number, "Memory allocation failed for a macro line.");
        return FALSE;
    }

    strcpy(new_line->line, line);
    new_line->next = NULL;

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

/* search for a macro by name */
static MacroNode* find_macro(MacroNode *head, const char *name) {
    MacroNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; 
        }
        current = current->next;
    }
    return NULL; 
}

/* frees the macro list and its lines */
void free_macro_table(MacroNode *head) {
    MacroNode *current_macro = head;
    MacroNode *next_macro;
    MacroLine *current_line;
    MacroLine *next_line;

    while (current_macro != NULL) {
        next_macro = current_macro->next;

        /* free all line nodes */
        current_line = current_macro->content;
        while (current_line != NULL) {
            next_line = current_line->next;
            free(current_line);
            current_line = next_line;
        }

        /* free macro node */
        free(current_macro);
        current_macro = next_macro;
    }
}
