#ifndef IMAGE_H
#define IMAGE_H

/* --- Standard Libraries Required --- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* 
 * ---------------------------------------------------------
 * INSTRUCTION IMAGE DATA STRUCTURES
 * ---------------------------------------------------------
 * These structures represent the 32-bit machine code 
 * of the imaginary computer. They use bit-fields to 
 * ensure exact bit sizes for each segment.
 * ---------------------------------------------------------
 */

/* 
 * Structure for R-Type Instructions.
 * Total: 32 bits.
 */
typedef struct {
    unsigned int unused : 6; /* Bits 0-5:   Unused (always 0) */
    unsigned int funct  : 5; /* Bits 6-10:  Function code */
    unsigned int rd     : 5; /* Bits 11-15: Destination register */
    unsigned int rt     : 5; /* Bits 16-20: Source register 2 */
    unsigned int rs     : 5; /* Bits 21-25: Source register 1 */
    unsigned int opcode : 6; /* Bits 26-31: Operation code */
} R_Instruction;

/* 
 * Structure for I-Type Instructions.
 * Total: 32 bits.
 */
typedef struct {
    int immed           : 16; /* Bits 0-15:  Immediate value (signed) */
    unsigned int rt     : 5;  /* Bits 16-20: Target/Source register */
    unsigned int rs     : 5;  /* Bits 21-25: Source register 1 */
    unsigned int opcode : 6;  /* Bits 26-31: Operation code */
} I_Instruction;

/* 
 * Structure for J-Type Instructions.
 * Total: 32 bits.
 */
typedef struct {
    unsigned int address : 25; /* Bits 0-24:  Target memory address */
    unsigned int reg     : 1;  /* Bit 25:     Register bit flag (0 or 1) */
    unsigned int opcode  : 6;  /* Bits 26-31: Operation code */
} J_Instruction;

/* 
 * Union representing a general 32-bit instruction word.
 * This allows treating the same 32 bits of memory differently 
 * depending on the instruction type (R, I, or J).
 */
typedef union {
    R_Instruction r_inst;
    I_Instruction i_inst;
    J_Instruction j_inst;
    unsigned long machine_code; /* Useful for printing the final 32-bit code to the .ob file */
} InstructionWord;

/* 
 * Node for the Instruction Image Linked List.
 * Represents a single instruction stored in the memory.
 */
typedef struct InstructionNode {
    InstructionWord word;         /* The encoded 32-bit instruction */
    int address;                  /* The memory address (starts at 100) */
    struct InstructionNode *next; /* Pointer to the next instruction node */
} InstructionNode;


/* 
 * ---------------------------------------------------------
 * DATA IMAGE DATA STRUCTURES
 * ---------------------------------------------------------
 * These structures represent the data stored by directives 
 * (.db, .dh, .dw, .asciz). Data is stored byte-by-byte.
 * ---------------------------------------------------------
 */

/* 
 * Node for the Data Image Linked List.
 * Represents a single byte (8 bits) of data stored in memory.
 */
typedef struct DataNode {
    unsigned char byte;      /* Stores exactly one byte (8 bits) */
    int address;             /* The relative data address */
    struct DataNode *next;   /* Pointer to the next data byte node */
} DataNode;
/* Function Prototypes */
void add_instruction(InstructionNode **head, InstructionWord word, int current_ic);
void add_data_byte(DataNode **head, unsigned char byte_val, int current_dc);
void free_images(InstructionNode **inst_head, DataNode **data_head);

#endif /* IMAGE_H */
