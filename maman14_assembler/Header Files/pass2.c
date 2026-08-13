#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pass2.h"
#include "image.h"
#include "symbol_table.h"
#include "globals.h"
#include "parser.h"

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
    char line[256];
    char first_word[64];
    char second_word[64];
    char operand[64];
    int line_number = 0;
    int error_flag = 0; /* 0 means success, 1 means errors found */
    int found_address;
    
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
        if (strcmp(operation_word, "jmp") == 0 || strcmp(operation_word, "la") == 0 || strcmp(operation_word, "call") == 0) {
            
            /* Extract the label operand (assuming it's the next string after the operation) */
            if (sscanf(line_remainder, "%*s %s", operand) == 1) {
                
                /* Search for the label in the symbol table */
                if (find_symbol_address(sym_head, operand, &found_address)) {
                    /* Fill the "hole" (the 0 we left in Pass 1) with the correct address */
                    current_inst->word.j_inst.address = found_address; 
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

