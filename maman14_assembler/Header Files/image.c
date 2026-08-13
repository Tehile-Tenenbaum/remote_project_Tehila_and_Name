#include "image.h"

/* 
 * ---------------------------------------------------------
 * Adds a new instruction node to the Instruction Image list.
 * Allocates memory dynamically and appends it to the end of the list.
 * 
 * @param head - Pointer to the head of the instruction list.
 * @param word - The encoded 32-bit instruction word.
 * @param current_ic - The memory address (IC) for this instruction.
 * ---------------------------------------------------------
 */
void add_instruction(InstructionNode **head, InstructionWord word, int current_ic) {
    InstructionNode *new_node = (InstructionNode *)malloc(sizeof(InstructionNode));
    InstructionNode *temp = *head;

    /* Check for memory allocation failure */
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for InstructionNode.\n");
        exit(1); 
    }

    /* Initialize the new node */
    new_node->word = word;
    new_node->address = current_ic;
    new_node->next = NULL;

    /* If the list is empty, make this node the head */
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /* Otherwise, traverse to the end of the list and attach it */
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

/* 
 * ---------------------------------------------------------
 * Adds a new byte node to the Data Image list.
 * Allocates memory dynamically and appends it to the end of the list.
 * 
 * @param head - Pointer to the head of the data list.
 * @param byte_val - The exactly 8-bit data byte to store.
 * @param current_dc - The memory address (DC) for this byte.
 * ---------------------------------------------------------
 */
void add_data_byte(DataNode **head, unsigned char byte_val, int current_dc) {
    DataNode *new_node = (DataNode *)malloc(sizeof(DataNode));
    DataNode *temp = *head;

    /* Check for memory allocation failure */
    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for DataNode.\n");
        exit(1);
    }

    /* Initialize the new node */
    new_node->byte = byte_val;
    new_node->address = current_dc;
    new_node->next = NULL;

    /* If the list is empty, make this node the head */
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /* Otherwise, traverse to the end of the list and attach it */
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

/* 
 * ---------------------------------------------------------
 * Frees all dynamically allocated memory for both images.
 * This is crucial to prevent memory leaks at the end of the program.
 * 
 * @param inst_head - Pointer to the head of the instruction list.
 * @param data_head - Pointer to the head of the data list.
 * ---------------------------------------------------------
 */
void free_images(InstructionNode **inst_head, DataNode **data_head) {
    InstructionNode *inst_temp;
    DataNode *data_temp;

    /* Free Instruction Image List */
    while (*inst_head != NULL) {
        inst_temp = *inst_head;
        *inst_head = (*inst_head)->next;
        free(inst_temp);
    }

    /* Free Data Image List */
    while (*data_head != NULL) {
        data_temp = *data_head;
        *data_head = (*data_head)->next;
        free(data_temp);
    }
}
