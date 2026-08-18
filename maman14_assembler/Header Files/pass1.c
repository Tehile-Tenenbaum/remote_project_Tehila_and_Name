#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "symbol_table.h"
#include "pass1.h"
#include "parser.h"
#include "utils.h"
#include "macro.h"
#define IC_INIT_VALUE 100
#define DC_INIT_VALUE 0

/* Helper functions assumed to be implemented in parsing modules */
/*
 * Function: execute_pass1
 * -----------------------
 * Executes the first pass algorithm on the .am file as defined in the syllabus.
 * @param filename: The base name of the source file.
 * @param symbol_table: Pointer to the symbol table.
 * @param ICF: Pointer to store the final Instruction Counter.
 * @param DCF: Pointer to store the final Data Counter.
 * @return: TRUE if no errors were found, FALSE otherwise.
 */
boolean execute_pass1(char *filename, SymbolNode **symbol_table, InstructionNode **inst_head, DataNode **data_head, int *ICF, int *DCF, MacroNode *macro_table) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char file_with_extension[MAX_LINE_LENGTH];
    
    /* Step 1: Initialize IC to 100, DC to 0 */
    int IC = IC_INIT_VALUE; 
    int DC = DC_INIT_VALUE;   
    int line_number = 0;
    boolean error_found = FALSE;
   char ext_label[MAX_LABEL_LENGTH];
 char current_word[MAX_LINE_LENGTH];
    strcpy(file_with_extension, filename);
    strcat(file_with_extension, ".am");

    file = fopen(file_with_extension, "r");
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", file_with_extension);
        return FALSE; 
    }

    /* Step 2: Read the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char first_word[MAX_LINE_LENGTH];
        char label_name[MAX_LABEL_LENGTH];
        int index = 0;
        boolean has_label = FALSE;
        
        line_number++;
          /* בדיקת אורך השורה: אם אין ירידת שורה ולא הגענו לסוף הקובץ, השורה ארוכה מדי */
        if (strchr(line, '\n') == NULL && !feof(file)) {
            int c;
            printf("Error at line %d: Line exceeds maximum length of 80 characters.\n", line_number);
            error_found = TRUE;
            
            /* ניקוי החוצץ (Buffer) עד סוף השורה כדי למנוע גלישה לאיטרציה הבאה */
        
            while ((c = fgetc(file)) != '\n' && c != EOF);
            
            continue; /* מדלגים על עיבוד השורה השגויה ועוברים לשורה הבאה */
        }
        /* Step 3: Skip comments and empty lines */
        skip_white_spaces(line, &index);
        if (line[index] == ';' || line[index] == '\n' || line[index] == '\r' || line[index] == '\0') {
            continue; 
        }
        
        /* Step 4: Check if the first field is a label */
        if (sscanf(&line[index], "%s", first_word) == 1) {
            if (first_word[strlen(first_word) - 1] == ':') {
                /* Step 5: Turn on "has label" flag */
                has_label = TRUE;
                first_word[strlen(first_word) - 1] = '\0'; /* Remove colon */
                strcpy(label_name, first_word);
                
                if (!is_valid_label_name(label_name)) {
                    printf("Error at line %d: Invalid label name '%s'\n", line_number, label_name);
                    error_found = TRUE;
                }
                if (!is_valid_label_name(label_name)) {
                    printf("Error at line %d: Invalid label name '%s'\n", line_number, label_name);
                    error_found = TRUE;
                } else if (is_macro_name(label_name, macro_table)) {
                    printf("Error at line %d: Label '%s' is already used as a macro name\n", line_number, label_name);
                    error_found = TRUE;
                }
                /* Advance index past the label */
                while (line[index] != ':' && line[index] != '\0') index++;
                index++; /* skip the colon */
                skip_white_spaces(line, &index);
            }
        }
        
        /* Read the next word (or first word if no label) to determine sentence type */
       
        if (sscanf(&line[index], "%s", current_word) != 1) {
            continue; /* Empty line after label */
        }

        /* Step 6: Is it a data directive? (.db, .dw, .dh, .asciz) */
        if (is_data_directive(current_word)) {
            /* Step 7: If there's a label, insert to symbol table with value DC and attribute 'data' */
            if (has_label) {
                if (find_symbol(*symbol_table, label_name) != NULL) {
                    printf("Error at line %d: Label '%s' already defined\n", line_number, label_name);
                    error_found = TRUE;
                } else {
                    add_symbol(symbol_table, label_name, DC, SYMBOL_TYPE_DATA,line_number);
                }
            }
            /* Step 8: Identify data type, encode to data image, update DC */
            index += strlen(current_word);
            if (!process_data_directive(line, &index, &DC, current_word, data_head)) {
                printf("Error at line %d: Invalid data directive syntax\n", line_number);
                error_found = TRUE;
            }
            continue; /* Go to next line (Step 2) */
        }
        
        /* Step 9: Is it an .extern or .entry directive? */
        if (is_extern_directive(current_word) || is_entry_directive(current_word)) {
            /* Step 10: If .entry, do nothing in Pass 1 */
            if (is_entry_directive(current_word)) {
                continue; 
            }
            
            /* Step 11: If .extern, insert operand to symbol table with attribute 'external' */
            if (is_extern_directive(current_word)) {

                index += strlen(current_word);
                skip_white_spaces(line, &index);
                
                if (sscanf(&line[index], "%s", ext_label) == 1) {
                    if (!is_valid_label_name(ext_label)) {
                         printf("Error at line %d: Invalid extern label\n", line_number);
                         error_found = TRUE;
                    } else {
                         add_symbol(symbol_table, ext_label, 0, SYMBOL_TYPE_EXTERNAL,line_number);
                    }
                }
            }
            continue; /* Go to next line (Step 2) */
        }
        
        /* Step 12: If not a directive, it must be a code instruction */
        /* Step 13: If there's a label, insert to symbol table with value IC and attribute 'code' */
        if (has_label) {
            if (find_symbol(*symbol_table, label_name) != NULL) {
                printf("Error at line %d: Label '%s' already defined\n", line_number, label_name);
                error_found = TRUE;
            } else {
                add_symbol(symbol_table, label_name, IC, SYMBOL_TYPE_CODE,line_number);
            }
        }
        
        /* Step 14 & 16: Encode instruction partially, update IC by 4 */
        index += strlen(current_word);
       if (!process_instruction(line, &index, &IC, current_word, inst_head)) {
            printf("Error at line %d: Invalid instruction syntax\n", line_number);
            error_found = TRUE;
        } 
    }

    fclose(file);

    /* Step 17: Stop if errors were found */
    if (error_found) {
        return FALSE;
    }

    /* Step 18: Save final IC and DC values */
    *ICF = IC;
    *DCF = DC;

    /* Step 19 & 20: Update all 'data' symbols value by adding ICF */
    update_data_symbols(*symbol_table, *ICF);
    /* Note: Updating the data image addresses is usually handled implicitly when writing the final output */

    return TRUE;
}
