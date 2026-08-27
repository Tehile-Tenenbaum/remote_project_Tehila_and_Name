#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Union representing a general 32-bit instruction word.
 *        Allows treating the same 32 bits differently depending on context.
 */
typedef union {
    unsigned long machine_code;
} InstructionWord;

/**
 * @brief Node for the Instruction Image Linked List.
 *        Represents a single instruction stored in the memory.
 */
typedef struct InstructionNode {
    InstructionWord word;         
    int address;                  
    struct InstructionNode *next; 
} InstructionNode;

/**
 * @brief Node for the Data Image Linked List.
 *        Represents a single byte (8 bits) of data stored in memory.
 */
typedef struct DataNode {
    unsigned char byte;      
    int address;             
    struct DataNode *next;   
} DataNode;

/* --- Function Prototypes --- */

void add_instruction(InstructionNode **head, InstructionWord word, int current_ic);
void add_data_byte(DataNode **head, unsigned char byte_val, int current_dc);
void free_images(InstructionNode **inst_head, DataNode **data_head);

#endif /* IMAGE_H */
