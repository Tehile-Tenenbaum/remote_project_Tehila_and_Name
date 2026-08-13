#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "parser.h"
#include "utils.h"

/* Declarations of external functions for adding data/code and skipping spaces */
extern void add_to_data_image(long value, int size_in_bytes, int *DC);
extern void add_to_code_image(unsigned long machine_code, int *IC);
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
 * Parses numeric operands or strings for data directives (.db, .dh, .dw, .asciz)
 * using standard sscanf, and encodes them into the data image while updating DC.
 * 
 * @param line: The full string of the current line.
 * @param index: Pointer to the current index in the line (right after the directive).
 * @param DC: Pointer to the Data Counter.
 * @param directive: The directive name (e.g., ".db", ".asciz").
 * @return: TRUE if parsing succeeded without syntax errors, FALSE otherwise.
 */
boolean process_data_directive(char *line, int *index, int *DC, char *directive) {
    int size_in_bytes = 1; /* Default size for .db */
    
    /* 1. Skip spaces after the directive */
    skip_white_spaces(line, index);

    /* 2. Handle .asciz (String directive) */
    if (strcmp(directive, ".asciz") == 0) {
        if (line[*index] != '"') {
            return FALSE; /* Error: String must start with double quotes */
        }
        (*index)++; /* Skip the opening quote */
        
        /* Read characters until the closing quote or end of line */
        while (line[*index] != '"' && line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
            add_to_data_image((long)line[*index], 1, DC);
            (*index)++;
        }
        
        if (line[*index] != '"') {
            return FALSE; /* Error: Missing closing quote */
        }
        (*index)++; /* Skip the closing quote */
        
        /* Add the null terminator '\0' at the end of the string */
        add_to_data_image(0, 1, DC); 
        return TRUE;
    }

    /* 3. Determine size for numeric directives */
    if (strcmp(directive, ".dh") == 0) {
        size_in_bytes = 2; /* Half-word */
    } else if (strcmp(directive, ".dw") == 0) {
        size_in_bytes = 4; /* Word */
    }

    /* 4. Loop to read all comma-separated numbers using standard sscanf */
    while (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
        long value;
        int chars_read = 0;
        
        skip_white_spaces(line, index);
        
        /* Check if we hit the end of the line unexpectedly */
        if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
            break;
        }

        /* Read the number and track consumed characters using %n */
        if (sscanf(&line[*index], "%ld%n", &value, &chars_read) != 1) {
            return FALSE; /* Error: Expected a valid number */
        }
        
        /* Advance the index by the number of characters read */
        *index += chars_read; 
        
        /* Add the parsed number to the data image */
        add_to_data_image(value, size_in_bytes, DC);
        
        /* 5. Handle commas between numbers */
        skip_white_spaces(line, index);
        
        if (line[*index] == ',') {
            (*index)++; /* Skip the comma */
            skip_white_spaces(line, index);
            
            /* Error if the line ends immediately after a comma */
            if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
                return FALSE; 
            }
        } else if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
            return FALSE; /* Error: Missing comma between numbers */
        }
    }
    
    return TRUE;
}

/*
 * Function: process_instruction
 * -----------------------------
 * Parses a standard assembly instruction (e.g., add, mov, jmp, hlt), encodes its 
 * primary machine code word, and handles instruction processing during Pass 1.
 * 
 * @param line: The full string of the current line.
 * @param index: Pointer to the current index in the line (pointing to the operation name).
 * @param IC: Pointer to the Instruction Counter.
 * @param operation: The name of the operation (e.g., "add", "hlt").
 * @return: TRUE if the instruction was successfully processed, FALSE otherwise.
 */
boolean process_instruction(char *line, int *index, int *IC, char *operation) {
    unsigned long first_word = 0;
    
    /* 1. Skip any white spaces after the operation name */
    skip_white_spaces(line, index);

    /* 2. Handle instructions with no operands (e.g., hlt, rts) */
    if (strcmp(operation, "hlt") == 0) {
        first_word = (63UL << 26);
    } 
    else if (strcmp(operation, "rts") == 0) {
        first_word = (62UL << 26);
    }
    else {
        /* 3. Handle R-type, J-type, and other instruction formats */
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

    /* 4. Add the encoded instruction word to the code image */
    add_to_code_image(first_word, IC);

    return TRUE;
}

/* 
 * Function: add_to_data_image
 * ---------------------------
 * Inserts a parsed numeric value or character into the data image array.
 */
void add_to_data_image(long value, int size_in_bytes, int *DC) {
    int i;
    for (i = 0; i < size_in_bytes; i++) {
        data_image[*DC] = (value >> (i * 8)) & 0xFF;
        (*DC)++; 
    }
}

/* 
 * Function: add_to_code_image
 * ---------------------------
 * Inserts an encoded machine code instruction word into the code image array.
 */
void add_to_code_image(unsigned long machine_code, int *IC) {
    code_image[*IC - IC_INIT_VALUE] = machine_code;
}
