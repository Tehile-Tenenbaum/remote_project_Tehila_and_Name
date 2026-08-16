#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "parser.h"
#include "utils.h"
#include "image.h"
/* Declarations of external functions for adding data/code and skipping spaces */
extern void skip_white_spaces(char *line, int *index);
/*
 * Function: is_data_directive
 * ---------------------------
 * Checks if a given word is a data directive (.db, .dh, .dw, .asciz).
 */
boolean is_data_directive(char *word) {
    if (strcmp(word, ".db") == 0 || strcmp(word, ".dh") == 0 ||
        strcmp(word, ".dw") == 0 || strcmp(word, ".asciz") == 0) {
        return TRUE;
    }
    return FALSE;
}

/*
 * Function: is_extern_directive
 * -----------------------------
 * Checks if a given word is the .extern directive.
 */
boolean is_extern_directive(char *word) {
    if (strcmp(word, ".extern") == 0) {
        return TRUE;
    }
    return FALSE;
}

/*
 * Function: is_entry_directive
 * ----------------------------
 * Checks if a given word is the .entry directive.
 */
boolean is_entry_directive(char *word) {
    if (strcmp(word, ".entry") == 0) {
        return TRUE;
    }
    return FALSE;
}
/*
 * Function: process_data_directive
 * --------------------------------
 */
boolean process_data_directive(char *line, int *index, int *DC, char *directive, DataNode **data_head) {
    int size_in_bytes = 1; /* Default size for .db */
    
    skip_white_spaces(line, index);

    if (strcmp(directive, ".asciz") == 0) {
        if (line[*index] != '"') {
            return FALSE;
        }
        (*index)++;
        
        while (line[*index] != '"' && line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
            add_to_data_image((long)line[*index], 1, DC, data_head);
            (*index)++;
        }
        
        if (line[*index] != '"') {
            return FALSE;
        }
        (*index)++;
        
        add_to_data_image(0, 1, DC, data_head); 
        return TRUE;
    }

    if (strcmp(directive, ".dh") == 0) {
        size_in_bytes = 2;
    } else if (strcmp(directive, ".dw") == 0) {
        size_in_bytes = 4;
    }

    while (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
        long value;
        int chars_read = 0;
        
        skip_white_spaces(line, index);
        
        if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
            break;
        }

        if (sscanf(&line[*index], "%ld%n", &value, &chars_read) != 1) {
            return FALSE;
        }
        
        *index += chars_read; 
        add_to_data_image(value, size_in_bytes, DC, data_head);
        
        skip_white_spaces(line, index);
        
        if (line[*index] == ',') {
            (*index)++;
            skip_white_spaces(line, index);
            if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
                return FALSE; 
            }
        } else if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
            return FALSE;
        }
    }
    
    return TRUE;
}

/*
 * Function: process_instruction
 * -----------------------------
 */
boolean process_instruction(char *line, int *index, int *IC, char *operation, InstructionNode **inst_head) {
    unsigned long first_word = 0;
    
    skip_white_spaces(line, index);

    if (strcmp(operation, "hlt") == 0) {
        first_word = (63UL << 26);
    } 
    else if (strcmp(operation, "rts") == 0) {
        first_word = (62UL << 26);
    }
    else {
        if (strcmp(operation, "add") == 0 || strcmp(operation, "sub") == 0 ||
            strcmp(operation, "and") == 0 || strcmp(operation, "or") == 0 ||
            strcmp(operation, "nor") == 0) {
            unsigned long opcode = 0;
            unsigned long funct = 1;
            if (strcmp(operation, "sub") == 0) funct = 2;
            else if (strcmp(operation, "and") == 0) funct = 3;
            else if (strcmp(operation, "or") == 0) funct = 4;
            else if (strcmp(operation, "nor") == 0) funct = 5;

            first_word = (opcode << 26) | (funct);
        }
        else if (strcmp(operation, "jmp") == 0) {
            first_word = (30UL << 26);
        }
        else if (strcmp(operation, "la") == 0 || strcmp(operation, "call") == 0) {
            unsigned long opcode = (strcmp(operation, "la") == 0) ? 31 : 32;
            first_word = (opcode << 26);
        }
    }

    add_to_code_image(first_word, IC, inst_head);
    return TRUE;
}

/* 
 * Function: add_to_data_image
 * ---------------------------
 */
void add_to_data_image(long value, int size_in_bytes, int *DC, DataNode **data_head) {
    int i;
    for (i = 0; i < size_in_bytes; i++) {
        unsigned char byte_val = (value >> (i * 8)) & 0xFF;
        add_data_byte(data_head, byte_val, *DC); 
        (*DC)++; 
    }
}

/* 
 * Function: add_to_code_image
 * ---------------------------
 */
void add_to_code_image(unsigned long machine_code, int *IC, InstructionNode **inst_head) {
    InstructionWord word;
    word.machine_code = machine_code;
    add_instruction(inst_head, word, *IC);
    *IC += 4;
}
