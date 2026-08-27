#include "image.h"

/* 
 * adds a new instruction to the end of the code image list 
 */
void add_instruction(InstructionNode **head, InstructionWord word, int current_ic) {
    InstructionNode *new_node = (InstructionNode *)malloc(sizeof(InstructionNode));
    InstructionNode *temp = *head;

    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for InstructionNode.\n");
        exit(1); 
    }

    /* setup the new node */
    new_node->word = word;
    new_node->address = current_ic;
    new_node->next = NULL;

    /* if list is empty, make it the head */
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /* find the last node and append */
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

/* 
 * adds a new data byte to the end of the data image list
 */
void add_data_byte(DataNode **head, unsigned char byte_val, int current_dc) {
    DataNode *new_node = (DataNode *)malloc(sizeof(DataNode));
    DataNode *temp = *head;

    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for DataNode.\n");
        exit(1);
    }

    /* setup the new node */
    new_node->byte = byte_val;
    new_node->address = current_dc;
    new_node->next = NULL;

    /* if list is empty, make it the head */
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    /* find the last node and append */
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

/* 
 * frees all the memory we allocated for both images 
 */
void free_images(InstructionNode **inst_head, DataNode **data_head) {
    InstructionNode *inst_temp;
    DataNode *data_temp;

    /* free code image list */
    while (*inst_head != NULL) {
        inst_temp = *inst_head;
        *inst_head = (*inst_head)->next;
        free(inst_temp);
    }

    /* free data image list */
    while (*data_head != NULL) {
        data_temp = *data_head;
        *data_head = (*data_head)->next;
        free(data_temp);
    }
}
