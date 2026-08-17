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
int find_symbol_address(SymbolNode *head, const char *name, int *address);

/*
 * ---------------------------------------------------------
 * Executes Pass 2:
 * 1. Reads the source file line by line.
 * 2. Marks .entry symbols in the symbol table.
 * 3. Completes missing addresses in I-type (branches) and J-type instructions.
 * ---------------------------------------------------------
 */
int execute_pass2(const char *filename, SymbolNode *sym_head, InstructionNode *inst_head, ExternNode **ext_head) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char first_word[MAX_LABEL_LENGTH];
    char second_word[MAX_LABEL_LENGTH];
    char operand[MAX_LABEL_LENGTH];
    int line_number = 0;
    int error_flag = 0; /* 0 means success, 1 means errors found */
    
    InstructionNode *current_inst = inst_head; 

    char file_with_extension[256];
    sprintf(file_with_extension, "%s.am", filename);
    file = fopen(file_with_extension, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file '%s' for Pass 2.\n", file_with_extension);
        return 0; /* Failure */
    }

    /* Read the file line by line */
    while (fgets(line, sizeof(line), file)) {
        char *operation_word = first_word;
        char *line_remainder = line;
        line_number++;
        
        /* Clear buffers for the new line */
        memset(first_word, 0, sizeof(first_word));
        memset(second_word, 0, sizeof(second_word));
        memset(operand, 0, sizeof(operand));

        /* Extract the first and second words to analyze the line */
        sscanf(line, "%s %s", first_word, second_word);

        /* STEP 1: Skip empty lines and comment lines */
        if (first_word[0] == '\0' || first_word[0] == ';') {
            continue;
        }

        /* 
         * STEP 2: Handle labels at the beginning of the line.
         * If the first word ends with ':', it's a label definition.
         * We skip it and analyze the second word instead.
         */
        if (first_word[strlen(first_word) - 1] == ':') {
            operation_word = second_word;
            /* Advance the line pointer past the label to extract operands later if needed */
            line_remainder = strstr(line, second_word); 
        }

        /* If there's nothing after the label, skip the line */
        if (operation_word == NULL || operation_word[0] == '\0') {
            continue;
        }

        /* STEP 3: Handle .entry directives */
        if (is_entry_directive(operation_word)) {
            /* Extract the operand (the symbol name) that comes after .entry */
            if (sscanf(line_remainder, "%*s %s", operand) == 1) {
                /* Attempt to mark it in the symbol table */
                if (!mark_symbol_as_entry(sym_head, operand)) {
                    fprintf(stderr, "Error in file %s (Line %d): Symbol '%s' declared as .entry but never defined.\n", 
                            filename, line_number, operand);
                    error_flag = 1;
                }
            } else {
                fprintf(stderr, "Error in file %s (Line %d): Missing operand for .entry directive.\n", filename, line_number);
                error_flag = 1;
            }
            continue; /* Move to the next line */
        }

        /* 
         * STEP 4: Skip data and extern directives.
         * We already handled .db, .dh, .dw, .asciz, and .extern in Pass 1. 
         */
        if (is_data_directive(operation_word) || is_extern_directive(operation_word)) {
            continue;
        }

        /* 
         * STEP 5: Handle code instructions.
         * At this point, the line is a valid code instruction.
         */
        if (current_inst == NULL) {
            fprintf(stderr, "Critical Error: Instruction image size mismatch at line %d.\n", line_number);
            error_flag = 1;
            break;
        }

        /* 
         * Check if this specific instruction requires us to fill in a missing label address.
         * Instructions that use labels are J-type (jmp, la, call) and I-type branches (beq, bne, blt, etc.)
         */
      if (strcmp(operation_word, "jmp") == 0 || strcmp(operation_word, "la") == 0 || strcmp(operation_word, "call") == 0 ||
            strcmp(operation_word, "beq") == 0 || strcmp(operation_word, "bne") == 0 || strcmp(operation_word, "blt") == 0 || strcmp(operation_word, "bgt") == 0) {
            
            /* Extract the label operand */
            if (sscanf(line_remainder, "%*s %s", operand) == 1) {
                
                /* Search for the FULL symbol node to get its address AND type */
                SymbolNode *target_sym = find_symbol(sym_head, operand);
                
                if (target_sym != NULL) {
                    
                    /* Fill the "hole" left in Pass 1 with safe bitwise operations */
                    
                    /* For J-type instructions (jmp, la, call), insert the absolute address */
                    if (strcmp(operation_word, "jmp") == 0 || strcmp(operation_word, "la") == 0 || strcmp(operation_word, "call") == 0) {
                        current_inst->word.machine_code |= (target_sym->address & 0x1FFFFFF);
                    } 
                    /* For I-type branch instructions, insert the relative distance */
                    else {
                        long distance = target_sym->address - current_inst->address;
                        current_inst->word.machine_code |= (distance & 0xFFFF);
                    }

                    /* --- EXTERNAL USAGE LOGIC --- */
                    /* If the symbol used is external, we must add it to the .ext file list */
                    if (target_sym->type == SYMBOL_TYPE_EXTERNAL) {
                        add_extern_usage(ext_head, operand, current_inst->address);
                    }
                    
                } else {
                    fprintf(stderr, "Error in file %s (Line %d): Undefined symbol '%s' used as operand.\n", 
                            filename, line_number, operand);
                    error_flag = 1;
                }
            }
        }

        /* 
         * CRITICAL: Advance the instruction pointer for EVERY valid code line!
         */
        current_inst = current_inst->next;
    }

    fclose(file);

    /* Return 1 (success) ONLY if no errors were found throughout Pass 2 */
    return (error_flag == 0) ? 1 : 0;
}

