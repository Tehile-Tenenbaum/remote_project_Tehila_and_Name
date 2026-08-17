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
 * Function: split_operands
 * -------------------------
 * מפרקת את מה שנשאר בשורה (החל מ-index) לרשימת אופרנדים מופרדים בפסיק.
 * שומרת רק עד 3 אופרנדים בפועל בתוך tokens (זה המקסימום האפשרי בכל הוראה
 * בשפה הזו), אבל ממשיכה לספור גם אם יש יותר - כדי שאפשר יהיה לזהות
 * "יותר מדי אופרנדים" כשגיאה. לאחר הקריאה, *index מצביע לתו הראשון
 * שאחרי האופרנד האחרון שנקרא (כדי לבדוק גרביג' בעזרת check_no_garbage).
 */
 int split_operands(char *line, int *index, char tokens[MAX_OPERANDS][MAX_LABEL_LENGTH]) {
    int count = 0;
    char dummy[32];
    char *dest;

    skip_white_spaces(line, index);
    while (line[*index] != '\0' && line[*index] != '\n' && line[*index] != '\r') {
        int j = 0;
        dest = (count < 3) ? tokens[count] : dummy;

        while (line[*index] != ',' && line[*index] != '\0' && line[*index] != '\n' &&
               line[*index] != '\r' && line[*index] != ' ' && line[*index] != '\t' && j < MAX_LABEL_LENGTH-1) {
            dest[j++] = line[(*index)++];
        }
        dest[j] = '\0';
        count++;

        skip_white_spaces(line, index);
        if (line[*index] == ',') {
            (*index)++;
            skip_white_spaces(line, index);
        } else {
            break; /* אין פסיק - סיימנו לקרוא אופרנדים */
        }
    }
    return count;
}

/*
 * Function: parse_register
 * -------------------------
 * בודקת שהטוקן הוא רגיסטר תקין בצורה $0 עד $31 (ללא תווים מיותרים
 * אחרי המספר), ומחזירה את מספר הרגיסטר דרך reg_num.
 */
 boolean parse_register(char *token, int *reg_num) {
    int num, chars_read;
    if (token[0] != '$') return FALSE;
    if (sscanf(token + 1, "%d%n", &num, &chars_read) != 1) return FALSE;
    if (token[1 + chars_read] != '\0') return FALSE;
    if (num < 0 || num > MAX_REGISTER) return FALSE;
    *reg_num = num;
    return TRUE;
}

/*
 * Function: process_instruction
 * -----------------------------
 * מנתחת ומקודדת שורת הוראה בודדת (32 סיביות), כולל כל האופרנדים
 * שלה, לפי סוג ההוראה (R / I / J). אופרנדים שהם תוויות (jmp/la/call
 * ללא רגיסטר, וכן היעד של beq/bne/blt/bgt) לא מקודדים כאן במלואם -
 * הם יושלמו ב-Pass 2, לאחר שכל הכתובות ידועות.
 */

boolean process_instruction(char *line, int *index, int *IC, char *operation, InstructionNode **inst_head) {
    unsigned long first_word = 0;
    char tokens[MAX_OPERANDS][MAX_LABEL_LENGTH];
    int num_tokens;

    skip_white_spaces(line, index);
    num_tokens = split_operands(line, index, tokens);

    if (!check_no_garbage(line, *index)) {
        return FALSE;
    }

    if (strcmp(operation, "hlt") == 0) {
        if (num_tokens != 0) return FALSE;
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

        if (num_tokens != MAX_OPERANDS) return FALSE;
        if (!parse_register(tokens[0], &rs)) return FALSE;
        if (!parse_register(tokens[1], &rt)) return FALSE;
        if (!parse_register(tokens[2], &rd)) return FALSE;

        first_word = (OPCODE_R_ARITH << OPCODE_SHIFT) | ((unsigned long)rs << RS_SHIFT) |
                     ((unsigned long)rt << RT_SHIFT) | ((unsigned long)rd << RD_SHIFT) | (funct << FUNCT_SHIFT);
    }
    else if (strcmp(operation, "move") == 0 || strcmp(operation, "mvhi") == 0 ||
             strcmp(operation, "mvlo") == 0) {
        unsigned long funct = FUNCT_MOVE;
        int dest, src;
        if (strcmp(operation, "mvhi") == 0) funct = FUNCT_MVHI;
        else if (strcmp(operation, "mvlo") == 0) funct = FUNCT_MVLO;

        if (num_tokens != 2) return FALSE;
        if (!parse_register(tokens[0], &dest)) return FALSE;
        if (!parse_register(tokens[1], &src)) return FALSE;

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

        if (num_tokens != MAX_OPERANDS) return FALSE;
        if (!parse_register(tokens[0], &rs)) return FALSE;
        if (sscanf(tokens[1], "%d", &immed) != 1) return FALSE;
        if (immed < MIN_IMMED || immed > MAX_IMMED) return FALSE;
        if (!parse_register(tokens[2], &rt)) return FALSE;

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

        if (num_tokens != MAX_OPERANDS) return FALSE;
        if (!parse_register(tokens[0], &rs)) return FALSE;
        if (!parse_register(tokens[1], &rt)) return FALSE;

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

        if (num_tokens != MAX_OPERANDS) return FALSE;
        if (!parse_register(tokens[0], &rs)) return FALSE;
        if (sscanf(tokens[1], "%d", &immed) != 1) return FALSE;
        if (immed < MIN_IMMED || immed > MAX_IMMED) return FALSE;
        if (!parse_register(tokens[2], &rt)) return FALSE;

        first_word = (opcode << OPCODE_SHIFT) | ((unsigned long)rs << RS_SHIFT) |
                     ((unsigned long)rt << RT_SHIFT) | ((unsigned long)immed & 0xFFFFUL);
    }
    else if (strcmp(operation, "jmp") == 0) {
        if (num_tokens != 1) return FALSE;
        if (tokens[0][0] == '$') {
            int reg;
            if (!parse_register(tokens[0], &reg)) return FALSE;
            first_word = (OPCODE_JMP << OPCODE_SHIFT) | (1UL << J_REG_SHIFT) | (unsigned long)reg;
        } else {
            first_word = (OPCODE_JMP << OPCODE_SHIFT); 
        }
    }
    else if (strcmp(operation, "la") == 0) {
        if (num_tokens != 1 || tokens[0][0] == '$') return FALSE;
        first_word = (OPCODE_LA << OPCODE_SHIFT); 
    }
    else if (strcmp(operation, "call") == 0) {
        if (num_tokens != 1 || tokens[0][0] == '$') return FALSE;
        first_word = (OPCODE_CALL << OPCODE_SHIFT); 
    }
    else {
        return FALSE; 
    }

    add_to_code_image(first_word, IC, inst_head);
    return TRUE;
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
        return check_no_garbage(line, *index);
    }

    if (strcmp(directive, ".dh") == 0) {
        size_in_bytes = 2;
    } else if (strcmp(directive, ".dw") == 0) {
        size_in_bytes = 4;
    }
 skip_white_spaces(line, index);
    if (line[*index] == ',') {
        return FALSE; /* פסיק לפני המספר הראשון אסור */
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

        /* בדיקת טווח לפי גודל התא (סעיף שסטודנטיות תיקנו קודם) */
    if (size_in_bytes == 1 && (value < MIN_DB || value > MAX_DB)) return FALSE;
        if (size_in_bytes == 2 && (value < MIN_DH || value > MAX_DH)) return FALSE;
        if (size_in_bytes == 4 && (value < MIN_DW || value > MAX_DW)) return FALSE;

        *index += chars_read;
        add_to_data_image(value, size_in_bytes, DC, data_head);

        skip_white_spaces(line, index);

        if (line[*index] == ',') {
            (*index)++;
            skip_white_spaces(line, index);
             if (line[*index] == '\0' || line[*index] == '\n' || line[*index] == '\r') {
                return FALSE; /* פסיק אחרי המספר האחרון (בסוף השורה) אסור */
            }
            if (line[*index] == ',') {
                return FALSE; /* פסיק כפול (רצוף) אסור */
            }
           
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
 * מוסיפה הוראה לתמונת הזיכרון ומקדמת את IC ב-4. זהו המקום היחיד
 * שבו IC מתקדם - ב-pass1.c אין להוסיף עוד IC += 4 אחרי הקריאה
 * לפונקציית process_instruction, אחרת ה-IC יתקדם פי 2.
 */
void add_to_code_image(unsigned long machine_code, int *IC, InstructionNode **inst_head) {
    InstructionWord word;
    word.machine_code = machine_code;
    add_instruction(inst_head, word, *IC);
    *IC += 4;
}
