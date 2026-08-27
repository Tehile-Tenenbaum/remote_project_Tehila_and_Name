#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pass2.h"
#include "image.h"
#include "symbol_table.h"
#include "globals.h"
#include "parser.h"
#include "output_files.h"
#include "error.h"

/* Bit masks for instruction encoding */
#define J_ADDRESS_MASK 0x1FFFFFF
#define I_IMMED_MASK   0xFFFF

int execute_pass2(const char *filename, SymbolNode *sym_head, InstructionNode *inst_head, ExternNode **ext_head) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_LINE_LENGTH];
    char second_word[MAX_LINE_LENGTH];
    char operand[MAX_LINE_LENGTH];
    int line_number = 0;
    
    InstructionNode *current_inst = inst_head; 
    SymbolNode *target_sym = NULL;
    char *file_with_extension;
    size_t len;
    
    /* איפוס דגל השגיאות הגלובלי לקובץ הנוכחי */
    error_found = FALSE;
    
    /* הקצאה דינמית לשם הקובץ */
    len = strlen(filename);
    file_with_extension = (char *)malloc(len + 4);
    if (file_with_extension == NULL) {
        printf("Memory allocation failed for filename.\n");
        return PASS2_FAILURE;
    }

    sprintf(file_with_extension, "%s.am", filename);
    file = fopen(file_with_extension, "r");
    if (file == NULL) {
        report_error(file_with_extension, 0, "Could not open file for Pass 2.");
        free(file_with_extension);
        return PASS2_FAILURE; 
    }

    while (fgets(line, sizeof(line), file)) {
        char *operation_word = first_word;
        char *line_remainder = line;
        line_number++;
        
        memset(first_word, 0, sizeof(first_word));
        memset(second_word, 0, sizeof(second_word));
        memset(operand, 0, sizeof(operand));

        sscanf(line, "%s %s", first_word, second_word);

        /* התעלמות משורות ריקות או הערות */
        if (first_word[0] == '\0' || first_word[0] == ';') {
            continue;
        }

        /* דילוג על הגדרת תווית בתחילת שורה */
        if (first_word[strlen(first_word) - 1] == ':') {
            operation_word = second_word;
            line_remainder = strstr(line, second_word); 
        }

        if (operation_word == NULL || operation_word[0] == '\0') {
            continue;
        }

        /* טיפול בהנחיות .entry */
        if (is_entry_directive(operation_word)) {
            if (sscanf(line_remainder, "%*s %s", operand) == 1) {
                SymbolNode *sym = find_symbol(sym_head, operand);
                
                if (sym != NULL && sym->type == SYMBOL_TYPE_EXTERNAL) {
                    report_error(file_with_extension, line_number, "Symbol '%s' is external and cannot be declared as .entry.", operand);
                } else if (!mark_symbol_as_entry(sym_head, operand)) {
                    report_error(file_with_extension, line_number, "Symbol '%s' declared as .entry but never defined.", operand);
                }
            } else {
                report_error(file_with_extension, line_number, "Missing operand for .entry directive.");
            }
            continue; 
        }

        /* התעלמות מנתונים או מ-extern שטופלו כבר במעבר הראשון */
        if (is_data_directive(operation_word) || is_extern_directive(operation_word)) {
            continue;
        }

        if (current_inst == NULL) {
            report_error(file_with_extension, line_number, "Critical Error: Instruction image size mismatch.");
            break;
        }

        /* בדיקה אם זו פקודה שדורשת השלמת כתובת */
        if (strcmp(operation_word, "jmp") == 0 || strcmp(operation_word, "la") == 0 || strcmp(operation_word, "call") == 0 ||
            strcmp(operation_word, "beq") == 0 || strcmp(operation_word, "bne") == 0 || strcmp(operation_word, "blt") == 0 || strcmp(operation_word, "bgt") == 0) {
            
            int found_operand = 0;
            
            if (strcmp(operation_word, "jmp") == 0 || strcmp(operation_word, "la") == 0 || strcmp(operation_word, "call") == 0) {
                if (sscanf(line_remainder, "%*s %s", operand) == 1) {
                    found_operand = 1;
                }
            } else {
                /* מציאת האופרנד האחרון (אחרי הפסיק) לפקודות התניה */
                char *last_comma = strrchr(line_remainder, ',');
                if (last_comma != NULL) {
                    if (sscanf(last_comma + 1, "%s", operand) == 1) {
                        found_operand = 1;
                    }
                }
            }

            if (found_operand) {
                /* קפיצה ישירה לרגיסטר לא דורשת השלמת כתובת מהטבלה */
                if (strcmp(operation_word, "jmp") == 0 && operand[0] == '$') {
                    current_inst = current_inst->next;
                    continue;
                }
                
                target_sym = find_symbol(sym_head, operand);
                if (target_sym != NULL) {
                    
                    /* חסימת קפיצה מותנית לתווית חיצונית */
                    if (target_sym->type == SYMBOL_TYPE_EXTERNAL) {
                        if (strcmp(operation_word, "beq") == 0 || strcmp(operation_word, "bne") == 0 ||
                            strcmp(operation_word, "blt") == 0 || strcmp(operation_word, "bgt") == 0) {
                            report_error(file_with_extension, line_number, "Conditional branch instruction '%s' cannot use external symbol '%s'.", operation_word, operand);
                            current_inst = current_inst->next;
                            continue;
                        }
                    }

                    /* קידוד הכתובת החסרה תוך שימוש במסכות */
                    if (strcmp(operation_word, "jmp") == 0 || strcmp(operation_word, "la") == 0 || strcmp(operation_word, "call") == 0) {
                        current_inst->word.machine_code |= (target_sym->address & J_ADDRESS_MASK);
                    } else {
                        long distance = target_sym->address - current_inst->address;
                        current_inst->word.machine_code |= (distance & I_IMMED_MASK);
                    }

                    /* רישום קריאה לתווית חיצונית עבור קובץ ה-.ext */
                    if (target_sym->type == SYMBOL_TYPE_EXTERNAL) {
                        add_extern_usage(ext_head, operand, current_inst->address);
                    }
                    
                } else {    
                    report_error(file_with_extension, line_number, "Undefined symbol '%s' used as operand.", operand);
                }
            }
        }

        current_inst = current_inst->next;
    }

    fclose(file);
    free(file_with_extension);

    /* הפונקציה מצליחה אך ורק אם לא נדלק דגל השגיאות */
    return (error_found == FALSE) ? PASS2_SUCCESS : PASS2_FAILURE;
}


