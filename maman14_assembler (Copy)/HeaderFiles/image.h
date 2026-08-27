#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* holds a 32-bit machine code instruction */
typedef union {
    unsigned long machine_code;
} InstructionWord;

/* linked list node for the code image */
typedef struct InstructionNode {
    InstructionWord word;         
    int address;                  
    struct InstructionNode *next; 
} InstructionNode;

/* linked list node for the data image */
typedef struct DataNode {
    unsigned char byte;      
    int address;             
    struct DataNode *next;   
} DataNode;

/* function prototypes */
void add_instruction(InstructionNode **head, InstructionWord word, int current_ic);
void add_data_byte(DataNode **head, unsigned char byte_val, int current_dc);
void free_images(InstructionNode **inst_head, DataNode **data_head);

#endif /* IMAGE_H */
