#ifndef PARSER_H
#define PARSER_H

#include "globals.h"
#include "image.h"

#define MAX_OPERANDS 3
#define MAX_REGISTER 31
#define INSTRUCTION_SIZE_BYTES 4 /* number of bytes an instruction takes in memory */

/* bit shifts for instruction encoding */
#define OPCODE_SHIFT 26
#define RS_SHIFT     21
#define RT_SHIFT     16
#define RD_SHIFT     11
#define FUNCT_SHIFT  6
#define J_REG_SHIFT  25

/* opcodes */
#define OPCODE_R_ARITH  0UL
#define OPCODE_R_MOVE   1UL

#define OPCODE_ADDI 10UL
#define OPCODE_SUBI 11UL
#define OPCODE_ANDI 12UL
#define OPCODE_ORI  13UL
#define OPCODE_NORI 14UL

#define OPCODE_BNE  15UL
#define OPCODE_BEQ  16UL
#define OPCODE_BLT  17UL
#define OPCODE_BGT  18UL

#define OPCODE_LB   19UL
#define OPCODE_SB   20UL
#define OPCODE_LW   21UL
#define OPCODE_SW   22UL
#define OPCODE_LH   23UL
#define OPCODE_SH   24UL

#define OPCODE_JMP  30UL
#define OPCODE_LA   31UL
#define OPCODE_CALL 32UL
#define OPCODE_HLT  63UL

/* funct codes for R-Type instructions */
#define FUNCT_ADD  1UL
#define FUNCT_SUB  2UL
#define FUNCT_AND  3UL
#define FUNCT_OR   4UL
#define FUNCT_NOR  5UL

#define FUNCT_MOVE 1UL
#define FUNCT_MVHI 2UL
#define FUNCT_MVLO 3UL

/* limits for immediate and data values */
#define MIN_IMMED -32768
#define MAX_IMMED 32767

#define MIN_DB -128
#define MAX_DB 127
#define MIN_DH -32768
#define MAX_DH 32767
#define MIN_DW -2147483648L
#define MAX_DW 2147483647L

/* parses data directives (.db, .dh, .dw, .asciz) */
boolean process_data_directive(char *line, int *index, int *DC, const char *directive, DataNode **data_head, int line_number, const char *filename);

/* splits instruction operands by commas */
int split_operands(char *line, int *index, char tokens[MAX_OPERANDS][MAX_LABEL_LENGTH], int line_number, const char *filename);

/* verifies a token is a valid register and gets its number */
boolean parse_register(const char *token, int *reg_num, int line_number, const char *filename);

/* parses an instruction and encodes it */
boolean process_instruction(char *line, int *index, int *IC, const char *operation, InstructionNode **inst_head, int line_number, const char *filename);

/* adds a multi-byte value to the data image */
void add_to_data_image(long value, int size_in_bytes, int *DC, DataNode **data_head);

/* adds an encoded 32-bit word to the instruction image */
void add_to_code_image(unsigned long machine_code, int *IC, InstructionNode **inst_head);

boolean is_data_directive(const char *word);
boolean is_entry_directive(const char *word);
boolean is_extern_directive(const char *word);

#endif /* PARSER_H */
