#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
#include "parser.h"
#include "utils.h"
#include "image.h"
#include "error.h"

extern void skip_white_spaces(char *line, int *index);

/* checks if the word is a data directive */
boolean is_data_directive(const char *word) {
    if (strcmp(word, ".db") == 0 || strcmp(word, ".dh") == 0 ||
        strcmp(word, ".dw") == 0 || strcmp(word, ".asciz") == 0) {
        return TRUE;
    }
    return FALSE;
}

/* checks if the word is .extern */
boolean is_extern_directive(const char *word) {
    return (strcmp(word, ".extern") == 0) ? TRUE : FALSE;
}

/* checks if the word is .entry */
boolean is_entry_directive(const char *word) {
    return (strcmp(word, ".entry") == 0) ? TRUE : FALSE;
}

/* validates and extracts register number */
boolean parse_register(const char *token, int *reg_num, int line_number, const char *filename) {
    int num, chars_read;
    
    if (token[0] != '$') {
        report_error(filename, line_number, "Invalid operand, expected register.");
        return FALSE;
    }
    /* blocks leading zeros */
    if (token[1] == '0' && token[2] != '\0') {
        report_error(filename, line_number, "Register names cannot have leading zeros.");
        return FALSE;
    }
    
    if (sscanf(token + 1, "%d%n", &num, &chars_read) != 1 || token[1 + chars_read] != '\0') {
        report_error(filename, line_number, "Unknown register name.");
        return FALSE;
    }
    
    if (num < 0 || num > MAX_REGISTER) {
        report_error(filename, line_number, "Unknown register name (out of bounds).");
        return FALSE;
    }
    
    *reg_num = num;
    return TRUE;
}

/* splits operands by commas */
int split_operands(char *line, int *index, char tokens[MAX_OPERANDS][MAX_LABEL_LENGTH], int line_number, const char *filename) {
    int count = 0;
    char dummy[MAX_LABEL_LENGTH];
    char *dest;
    boolean expect_comma = FALSE;

    skip_white_spaces(line, index);

    if (line[*index] == ',') {
        report_error(filename, line_number, "Illegal comma before first operand.");
        return -1;
    }

    while (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
        if (expect_comma) {
            if (line[*index] != ',') {
                report_error(filename, line_number, "Missing comma between operands or extraneous text.");
                return -1;
            }
            (*index)++; 
            skip_white_spaces(line, index);

            if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
                report_error(filename, line_number, "Illegal comma at the end of command.");
                return -1;
            }
            if (line[*index] == ',') {
                report_error(filename, line_number, "Multiple consecutive commas.");
                return -1;
            }
            expect_comma = FALSE;
        } else {
            int j = 0;
            dest = (count < MAX_OPERANDS) ? tokens[count] : dummy;

            while (line[*index] != ',' && line[*index] != '\0' && line[*index] != '\n' &&
                   line[*index] != '\r' && line[*index] != ' ' && line[*index] != '\t' && j < MAX_LABEL_LENGTH - 1) {
                dest[j++] = line[(*index)++];
            }
            dest[j] = '\0';
            count++;

            skip_white_spaces(line, index);
            expect_comma = TRUE; 
        }
    }
    return count;
}

/* encodes instruction based on operation name */
boolean process_instruction(char *line, int *index, int *IC, const char *operation, InstructionNode **inst_head, int line_number, const char *filename) {
    unsigned long first_word = 0;
    char tokens[MAX_OPERANDS][MAX_LABEL_LENGTH];
    int num_tokens;
    int chars_read = 0;
    
    skip_white_spaces(line, index);
    
    num_tokens = split_operands(line, index, tokens, line_number, filename);

    if (num_tokens < 0) {
        return FALSE; 
    }

    if (strcmp(operation, "hlt") == 0) {
        if (num_tokens != 0) {
            report_error(filename, line_number, "Extraneous text after end of command.");
            return FALSE;
        }
        first_word = (OPCODE_HLT << OPCODE_SHIFT);
    }
    else if (strcmp(operation, "add") == 0 || strcmp(operation, "sub") == 0 ||
             strcmp(operation, "and") == 0 || strcmp(operation, "or") == 0 ||
             strcmp(operation, "nor") == 0) {
        unsigned long funct = FUNCT_ADD;
        int rs, rt, rd;
        
        if (strcmp(operation, "sub") == 0) funct = FUNCT_SUB;
        else if (strcmp(operation, "and") == 0) funct = FUNCT_AND;
        else if (strcmp(operation, "or") == 0) funct = FUNCT_OR;
        else if (strcmp(operation, "nor") == 0) funct = FUNCT_NOR;

        if (num_tokens != MAX_OPERANDS) {
            report_error(filename, line_number, "Invalid number of operands for R-arithmetic instruction.");
            return FALSE;
        }
        if (!parse_register(tokens[0], &rs, line_number, filename)) return FALSE;
        if (!parse_register(tokens[1], &rt, line_number, filename)) return FALSE;
        if (!parse_register(tokens[2], &rd, line_number, filename)) return FALSE;

        first_word = (OPCODE_R_ARITH << OPCODE_SHIFT) | ((unsigned long)rs << RS_SHIFT) |
                     ((unsigned long)rt << RT_SHIFT) | ((unsigned long)rd << RD_SHIFT) | (funct << FUNCT_SHIFT);
    }
    else if (strcmp(operation, "move") == 0 || strcmp(operation, "mvhi") == 0 ||
             strcmp(operation, "mvlo") == 0) {
        unsigned long funct = FUNCT_MOVE;
        int dest, src;
        if (strcmp(operation, "mvhi") == 0) funct = FUNCT_MVHI;
        else if (strcmp(operation, "mvlo") == 0) funct = FUNCT_MVLO;

        if (num_tokens != 2) {
             report_error(filename, line_number, "Invalid number of operands for R-move instruction.");
             return FALSE;
        }
        if (!parse_register(tokens[0], &dest, line_number, filename)) return FALSE;
        if (!parse_register(tokens[1], &src, line_number, filename)) return FALSE;

        first_word = (OPCODE_R_MOVE << OPCODE_SHIFT) | ((unsigned long)dest << RS_SHIFT) |
                     ((unsigned long)src << RD_SHIFT) | (funct << FUNCT_SHIFT);
    }
    else if (strcmp(operation, "addi") == 0 || strcmp(operation, "subi") == 0 ||
             strcmp(operation, "andi") == 0 || strcmp(operation, "ori") == 0 ||
             strcmp(operation, "nori") == 0) {
        unsigned long opcode = OPCODE_ADDI;
        int rs, rt, immed;
        if (strcmp(operation, "subi") == 0) opcode = OPCODE_SUBI;
        else if (strcmp(operation, "andi") == 0) opcode = OPCODE_ANDI;
        else if (strcmp(operation, "ori") == 0) opcode = OPCODE_ORI;
        else if (strcmp(operation, "nori") == 0) opcode = OPCODE_NORI;

        if (num_tokens != MAX_OPERANDS) {
            report_error(filename, line_number, "Invalid number of operands for I-arithmetic instruction.");
            return FALSE;
        }
        if (!parse_register(tokens[0], &rs, line_number, filename)) return FALSE;
        if (sscanf(tokens[1], "%d%n", &immed, &chars_read) != 1 || tokens[1][chars_read] != '\0') {
            report_error(filename, line_number, "Invalid immediate operand.");
            return FALSE;
        }
        if (immed < MIN_IMMED || immed > MAX_IMMED) {
            report_error(filename, line_number, "Immediate value out of range.");
            return FALSE;
        }
        if (!parse_register(tokens[2], &rt, line_number, filename)) return FALSE;

        first_word = (opcode << OPCODE_SHIFT) | ((unsigned long)rs << RS_SHIFT) |
                     ((unsigned long)rt << RT_SHIFT) | ((unsigned long)immed & 0xFFFFUL);
    }
    else if (strcmp(operation, "beq") == 0 || strcmp(operation, "bne") == 0 ||
             strcmp(operation, "blt") == 0 || strcmp(operation, "bgt") == 0) {
        unsigned long opcode = OPCODE_BEQ;
        int rs, rt;
        if (strcmp(operation, "bne") == 0) opcode = OPCODE_BNE;
        else if (strcmp(operation, "blt") == 0) opcode = OPCODE_BLT;
        else if (strcmp(operation, "bgt") == 0) opcode = OPCODE_BGT;

        if (num_tokens != MAX_OPERANDS) {
            report_error(filename, line_number, "Invalid number of operands for branching instruction.");
            return FALSE;
        }
        if (!parse_register(tokens[0], &rs, line_number, filename)) return FALSE;
        if (!parse_register(tokens[1], &rt, line_number, filename)) return FALSE;
        if (!is_valid_label_name(tokens[2])) {
            report_error(filename, line_number, "Invalid label name '%s' as operand.", tokens[2]);
            return FALSE;
        }
        first_word = (opcode << OPCODE_SHIFT) | ((unsigned long)rs << RS_SHIFT) | ((unsigned long)rt << RT_SHIFT);
    }
    else if (strcmp(operation, "lb") == 0 || strcmp(operation, "sb") == 0 ||
             strcmp(operation, "lw") == 0 || strcmp(operation, "sw") == 0 ||
             strcmp(operation, "lh") == 0 || strcmp(operation, "sh") == 0) {
        unsigned long opcode;
        int rs, rt, immed;
        
        if (strcmp(operation, "lb") == 0) opcode = OPCODE_LB;
        else if (strcmp(operation, "sb") == 0) opcode = OPCODE_SB;
        else if (strcmp(operation, "lw") == 0) opcode = OPCODE_LW;
        else if (strcmp(operation, "sw") == 0) opcode = OPCODE_SW;
        else if (strcmp(operation, "lh") == 0) opcode = OPCODE_LH;
        else opcode = OPCODE_SH;

        if (num_tokens != MAX_OPERANDS) {
            report_error(filename, line_number, "Invalid number of operands for load/store instruction.");
            return FALSE;
        }
        if (!parse_register(tokens[0], &rs, line_number, filename)) return FALSE;
        
        chars_read=0;
        if (sscanf(tokens[1], "%d%n", &immed, &chars_read) != 1 || tokens[1][chars_read] != '\0') {
            report_error(filename, line_number, "Invalid immediate operand.");
            return FALSE;
        }
        if (immed < MIN_IMMED || immed > MAX_IMMED) {
             report_error(filename, line_number, "Immediate value out of range.");
             return FALSE;
        }
        if (!parse_register(tokens[2], &rt, line_number, filename)) return FALSE;

        first_word = (opcode << OPCODE_SHIFT) | ((unsigned long)rs << RS_SHIFT) |
                     ((unsigned long)rt << RT_SHIFT) | ((unsigned long)immed & 0xFFFFUL);
    }
    else if (strcmp(operation, "jmp") == 0) {
        if (num_tokens != 1) {
            report_error(filename, line_number, "Invalid number of operands for jmp instruction.");
            return FALSE;
        }
        if (tokens[0][0] == '$') {
            int reg;
            if (!parse_register(tokens[0], &reg, line_number, filename)) return FALSE;
            first_word = (OPCODE_JMP << OPCODE_SHIFT) | (1UL << J_REG_SHIFT) | (unsigned long)reg;
        } else {
            if (!is_valid_label_name(tokens[0])) {
                report_error(filename, line_number, "Invalid label name '%s' as operand.", tokens[0]);
                return FALSE;
            }
            first_word = (OPCODE_JMP << OPCODE_SHIFT);
        }
    }
    else if (strcmp(operation, "la") == 0 || strcmp(operation, "call") == 0) {
        if (num_tokens != 1 || tokens[0][0] == '$') {
             report_error(filename, line_number, "Invalid operand for %s instruction.", operation);
             return FALSE;
        }
        if (!is_valid_label_name(tokens[0])) {
            report_error(filename, line_number, "Invalid label name '%s' as operand.", tokens[0]);
            return FALSE;
        }
        first_word = (strcmp(operation, "la") == 0) ? (OPCODE_LA << OPCODE_SHIFT) : (OPCODE_CALL << OPCODE_SHIFT);
    }
    else {
        report_error(filename, line_number, "Unknown opcode '%s'.", operation);
        return FALSE;
    }

    add_to_code_image(first_word, IC, inst_head);
    return TRUE;
}
/* parses a data directive (.db, .dh, .dw, .asciz) and appends the
   encoded values to the data image, advancing DC accordingly */
boolean process_data_directive(char *line, int *index, int *DC, const char *directive,
                               DataNode **data_head, int line_number, const char *filename) {
    int size_in_bytes = 1;
    int values_found = 0;
    long min_value = MIN_DB;
    long max_value = MAX_DB;

    skip_white_spaces(line, index);

    /* ---------- .asciz : a single quoted string ---------- */
    if (strcmp(directive, ".asciz") == 0) {
        if (line[*index] != '"') {
            report_error(filename, line_number, "Missing opening quote for .asciz string.");
            return FALSE;
        }
        (*index)++;

        while (line[*index] != '"' && line[*index] != '\0' &&
               line[*index] != '\n' && line[*index] != '\r') {

            /* the language only allows printable ascii inside a string */
            if (!isprint((unsigned char)line[*index])) {
                report_error(filename, line_number,
                             "Non-printable character inside .asciz string.");
                return FALSE;
            }

            add_to_data_image((long)line[*index], 1, DC, data_head);
            (*index)++;
        }

        if (line[*index] != '"') {
            report_error(filename, line_number, "Missing closing quote in .asciz string.");
            return FALSE;
        }
        (*index)++;

        /* the terminating '\0' is stored as part of the string */
        add_to_data_image(0, 1, DC, data_head);

        if (!check_no_garbage(line, *index)) {
            report_error(filename, line_number, "Extraneous text after string.");
            return FALSE;
        }
        return TRUE;
    }

    /* ---------- .db / .dh / .dw : one or more integers ---------- */
    if (strcmp(directive, ".dh") == 0) {
        size_in_bytes = 2;
        min_value = MIN_DH;
        max_value = MAX_DH;
    } else if (strcmp(directive, ".dw") == 0) {
        size_in_bytes = 4;
        min_value = MIN_DW;
        max_value = MAX_DW;
    }

    if (line[*index] == ',') {
        report_error(filename, line_number, "Illegal comma before first value.");
        return FALSE;
    }

    while (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
        long value;
        int chars_read = 0;
        int digit_count = 0;
        int scan_index = *index;

        /* Count the significant digits before parsing. A literal with more
           than MAX_VALUE_DIGITS digits cannot fit any data directive, and
           handing it to sscanf would overflow the conversion itself. */
        if (line[scan_index] == '+' || line[scan_index] == '-') {
            scan_index++;
        }
        while (line[scan_index] == '0') {
            scan_index++;
        }
        while (isdigit((unsigned char)line[scan_index])) {
            digit_count++;
            scan_index++;
        }

        if (digit_count > MAX_VALUE_DIGITS) {
            report_error(filename, line_number,
                         "Numeric literal is too large for the '%s' directive.", directive);
            return FALSE;
        }

        if (sscanf(&line[*index], "%ld%n", &value, &chars_read) != 1) {
            report_error(filename, line_number,
                         "Invalid syntax in '%s' directive, expected a number.", directive);
            return FALSE;
        }

        if (value < min_value || value > max_value) {
            report_error(filename, line_number,
                         "Value %ld is out of range for the '%s' directive.", value, directive);
            return FALSE;
        }

        *index += chars_read;
        add_to_data_image(value, size_in_bytes, DC, data_head);
        values_found++;

        skip_white_spaces(line, index);

        if (line[*index] == ',') {
            (*index)++;
            skip_white_spaces(line, index);

            if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
                report_error(filename, line_number,
                             "Illegal comma at the end of the directive.");
                return FALSE;
            }
            if (line[*index] == ',') {
                report_error(filename, line_number,
                             "Multiple consecutive commas are not allowed.");
                return FALSE;
            }
        } else if (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
            report_error(filename, line_number, "Missing comma between values.");
            return FALSE;
        }
    }

    /* the directive must define at least one value */
    if (values_found == 0) {
        report_error(filename, line_number,
                     "Directive '%s' requires at least one value.", directive);
        return FALSE;
    }

    return TRUE;
}
/* ------------------------------------------------------------------
 * Companion fix, also in parser.c.
 * Right-shifting a negative long is implementation-defined in C90,
 * so the value is converted before the shift.
 * ------------------------------------------------------------------ */

/* writes a value to the data image, least significant byte first */
void add_to_data_image(long value, int size_in_bytes, int *DC, DataNode **data_head) {
    int i;
    unsigned long raw = (unsigned long)value;

    for (i = 0; i < size_in_bytes; i++) {
        unsigned char byte_val = (unsigned char)((raw >> (i * 8)) & 0xFFUL);
        add_data_byte(data_head, byte_val, *DC);
        (*DC)++;
    }
}
/* writes the full instruction word to the code image */
void add_to_code_image(unsigned long machine_code, int *IC, InstructionNode **inst_head) {
    InstructionWord word;
    word.machine_code = machine_code;
    add_instruction(inst_head, word, *IC);
    *IC += INSTRUCTION_SIZE_BYTES;
}
