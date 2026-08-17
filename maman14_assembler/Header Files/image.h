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
 * Union representing a general 32-bit instruction word.
 * This allows treating the same 32 bits of memory differently 
 * depending on the instruction type (R, I, or J).
 */
typedef union {
    unsigned long machine_code;
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
