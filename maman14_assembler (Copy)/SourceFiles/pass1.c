#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "symbol_table.h"
#include "pass1.h"
#include "parser.h"
#include "utils.h"
#include "macro.h"
#include "error.h"

#ifndef DC_INIT_VALUE
#define DC_INIT_VALUE 0
#endif

/* performs the first pass of the assembler */
boolean execute_pass1(const char *filename, SymbolNode **symbol_table, InstructionNode **inst_head, DataNode **data_head, int *ICF, int *DCF, MacroNode *macro_table) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *file_with_extension;
    size_t len;
    
    int IC = IC_INIT_VALUE; 
    int DC = DC_INIT_VALUE;   
    int line_number = 0;
    
    char ext_label[MAX_LINE_LENGTH];
    char current_word[MAX_LINE_LENGTH];

    /* reset global error flag for the current file */
    error_found = FALSE;

    len = strlen(filename);
    file_with_extension = (char *)malloc(len + FILE_EXT_LEN);
    if (file_with_extension == NULL) {
        printf("Memory allocation failed for filename.\n");
        return FALSE;
    }

    sprintf(file_with_extension, "%s.am", filename);

    file = fopen(file_with_extension, "r");
    if (file == NULL) {
        report_error(file_with_extension, 0, "Cannot open file.");
        free(file_with_extension);
        return FALSE; 
    }

    /* process each line */
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        char first_word[MAX_LINE_LENGTH];
        char label_name[MAX_LINE_LENGTH];
        int index = 0;
        boolean has_label = FALSE;
        
        line_number++;

        /* check max line length */
        if (strchr(line, '\n') == NULL && !feof(file)) {
            int c;
            report_error(file_with_extension, line_number, "Line exceeds maximum length of 80 characters.");
            while ((c = fgetc(file)) != '\n' && c != EOF); /* clear buffer */
            continue; 
        }
 
        skip_white_spaces(line, &index);
        if (line[index] == ';' || line[index] == '\n' || line[index] == '\r' || line[index] == '\0') {
            continue; 
        }
        
        if (line[index] == ',') {
            report_error(file_with_extension, line_number, "Illegal comma at the beginning of the line.");
            continue;
        }
        
        /* check if the line starts with a label */
        if (sscanf(&line[index], "%s", first_word) == 1) {
            char *colon_ptr = strchr(first_word, ':');
            
            if (colon_ptr != NULL) {
                if (colon_ptr == first_word + strlen(first_word) - 1) {
                    has_label = TRUE;
                    first_word[strlen(first_word) - 1] = '\0'; 
                    strcpy(label_name, first_word);
                    
                   if (strlen(label_name) >= MAX_LABEL_LENGTH) {
                        report_error(file_with_extension, line_number, "Symbol name '%s' exceeds the maximum allowed length of 31 characters.", label_name);
                        continue; 
                    } else if (!is_valid_label_name(label_name)) {
                        report_error(file_with_extension, line_number, "Invalid label name '%s'.", label_name);
                        continue; 
                    } else if (is_macro_name(label_name, macro_table)) {
                        report_error(file_with_extension, line_number, "Label '%s' is already used as a macro name.", label_name);
                        continue; 
                    }
                    
                    while (line[index] != ':' && line[index] != '\0') index++;
                    index++; 
                    skip_white_spaces(line, &index);
                } else {
                    report_error(file_with_extension, line_number, "Missing white space after the colon in label definition.");
                    continue; 
                }
            } else {
                /* check if there's a space before the colon */
                int temp_idx = index + strlen(first_word);
                skip_white_spaces(line, &temp_idx);
                if (line[temp_idx] == ':') {
                    report_error(file_with_extension, line_number, "Space is not allowed before colon in label definition.");
                    continue; 
                }
            }
        }
        
        if (sscanf(&line[index], "%s", current_word) != 1) {
            if (has_label) {
                report_error(file_with_extension, line_number, "Label definition must be followed by an instruction or a directive.");
            }
            continue; 
        }
        
        /* process data directives */
        if (is_data_directive(current_word)) {
            if (has_label) {
                if (find_symbol(*symbol_table, label_name) != NULL) {
                    report_error(file_with_extension, line_number, "Label '%s' is already defined.", label_name);
                } else {
                    add_symbol(symbol_table, label_name, DC, SYMBOL_TYPE_DATA, line_number, file_with_extension);
                }
            }
            
            index += strlen(current_word);
            process_data_directive(line, &index, &DC, current_word, data_head, line_number, file_with_extension);
            continue; 
        }

 /* process .extern or .entry directives */
        if (is_extern_directive(current_word) || is_entry_directive(current_word)) {

            if (is_entry_directive(current_word)) {
                char entry_label[MAX_LINE_LENGTH];
                int temp_index = index + strlen(current_word);
                int chars_read = 0;

                skip_white_spaces(line, &temp_index);

                if (sscanf(&line[temp_index], "%s%n", entry_label, &chars_read) == 1) {
                    if (!is_valid_label_name(entry_label)) {
                        report_error(file_with_extension, line_number,
                                     "Invalid label after .entry.");
                    } else if (!check_no_garbage(line, temp_index + chars_read)) {
                        report_error(file_with_extension, line_number,
                                     "Extraneous text after end of command.");
                    } else {
                        SymbolNode *existing = find_symbol(*symbol_table, entry_label);
                        if (existing != NULL && existing->type == SYMBOL_TYPE_EXTERNAL) {
                            report_error(file_with_extension, line_number,
                                         "Symbol '%s' cannot be defined as both .extern and .entry.",
                                         entry_label);
                        }
                    }
                } else {
                    report_error(file_with_extension, line_number,
                                 "Missing operand for .entry directive.");
                }

                continue;
            }

            if (is_extern_directive(current_word)) {
                int chars_read = 0;

                index += strlen(current_word);
                skip_white_spaces(line, &index);

                if (sscanf(&line[index], "%s%n", ext_label, &chars_read) == 1) {
                    if (!is_valid_label_name(ext_label)) {
                        report_error(file_with_extension, line_number,
                                     "Invalid extern label.");
                    } else if (!check_no_garbage(line, index + chars_read)) {
                        report_error(file_with_extension, line_number,
                                     "Extraneous text after end of command.");
                    } else {
                        SymbolNode *existing = find_symbol(*symbol_table, ext_label);
                        if (existing != NULL) {
                            if (existing->type != SYMBOL_TYPE_EXTERNAL) {
                                report_error(file_with_extension, line_number,
                                             "Symbol '%s' already defined locally.", ext_label);
                            }
                        } else {
                            add_symbol(symbol_table, ext_label, 0, SYMBOL_TYPE_EXTERNAL,
                                       line_number, file_with_extension);
                        }
                    }
                } else {
                    report_error(file_with_extension, line_number,
                                 "Missing operand for .extern directive.");
                }
            }

            continue;
        }

        /* unknown directive catch */
        if (current_word[0] == '.') {
            report_error(file_with_extension, line_number, "Unknown directive '%s'.", current_word);
            continue;
        }

        /* if not a directive, process it as a regular instruction */
        if (has_label) {
            if (find_symbol(*symbol_table, label_name) != NULL) {
                report_error(file_with_extension, line_number, "Label '%s' is already defined.", label_name);
            } else {
                add_symbol(symbol_table, label_name, IC, SYMBOL_TYPE_CODE, line_number, file_with_extension);
            }
        }
        
        index += strlen(current_word);
        process_instruction(line, &index, &IC, current_word, inst_head, line_number, file_with_extension);
    }

    fclose(file);

    /* if we found errors, stop before proceeding */
    if (error_found) {
        free(file_with_extension);
        return FALSE;
    }

    *ICF = IC;
    *DCF = DC;

    /* update data symbols addresses */
    update_data_symbols(*symbol_table, *ICF);

    free(file_with_extension);
    return TRUE;
}
