#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "symbol_table.h"
#include "output_files.h"

/* 
 * ---------------------------------------------------------
 * Creates the .ob (Object) file containing the machine code.
 * ---------------------------------------------------------
 */
void create_ob_file(const char *filename, InstructionNode *inst_head, DataNode *data_head, int icf, int dcf) {
    FILE *file;
    char ob_filename[256];
    InstructionNode *inst_curr = inst_head;
    DataNode *data_curr = data_head;
    int data_byte_count = 0;
    
    sprintf(ob_filename, "%s.ob", filename);
    
    file = fopen(ob_filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot create output file '%s'.\n", ob_filename);
        return;
    }
    
    /* Print Header: Instruction image size and Data image size */
    fprintf(file, "\t%d %d\n", icf - 100, dcf);
    
    /* 1. Print Instruction Image */
    while (inst_curr != NULL) {
        unsigned int val = inst_curr->word.machine_code; 
        
        fprintf(file, "%04d %02X %02X %02X %02X\n", 
                inst_curr->address,
                (val >> 24) & 0xFF,
                (val >> 16) & 0xFF,
                (val >> 8) & 0xFF,
                val & 0xFF);
                
        inst_curr = inst_curr->next;
    }
    
    /* 2. Print Data Image (4 bytes per line format) */
    while (data_curr != NULL) {
        if (data_byte_count % 4 == 0) {
            if (data_byte_count > 0) {
                fprintf(file, "\n"); 
            }
            fprintf(file, "%04d", data_curr->address);
        }
        
        fprintf(file, " %02X", data_curr->byte & 0xFF);
        
        data_byte_count++;
        data_curr = data_curr->next;
    }
    
    if (data_byte_count > 0) {
        fprintf(file, "\n");
    }
    
    fclose(file);
}

/* 
 * ---------------------------------------------------------
 * Creates the .ent (Entries) file.
 * Only creates the file if there is at least one .entry symbol.
 * ---------------------------------------------------------
 */
void create_ent_file(const char *filename, SymbolNode *sym_head) {
    SymbolNode *curr = sym_head;
    int has_entry = 0;
    FILE *file;
    char ent_filename[256];

    /* First, check if there are any entry symbols to avoid creating an empty file */
    while (curr != NULL) {
        if (curr->is_entry) {
            has_entry = 1;
            break;
        }
        curr = curr->next;
    }

    if (!has_entry) {
        return; /* No entry symbols found, do not create .ent file */
    }

    sprintf(ent_filename, "%s.ent", filename);
    file = fopen(ent_filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot create output file '%s'.\n", ent_filename);
        return;
    }

    /* Print all entry symbols and their addresses */
    curr = sym_head;
    while (curr != NULL) {
        if (curr->is_entry) {
            fprintf(file, "%s %04d\n", curr->name, curr->address);
        }
        curr = curr->next;
    }

    fclose(file);
}

/* 
 * ---------------------------------------------------------
 * Creates the .ext (Externals) file.
 * Only creates the file if there is at least one external usage.
 * ---------------------------------------------------------
 */
void create_ext_file(const char *filename, ExternNode *ext_head) {
    FILE *file;
    char ext_filename[256];
    ExternNode *curr = ext_head;

    if (ext_head == NULL) {
        return; /* No external usages found, do not create .ext file */
    }

    sprintf(ext_filename, "%s.ext", filename);
    file = fopen(ext_filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot create output file '%s'.\n", ext_filename);
        return;
    }

    /* Print all external usages and the addresses where they were used */
    while (curr != NULL) {
        fprintf(file, "%s %04d\n", curr->name, curr->address);
        curr = curr->next;
    }

    fclose(file);
}

/* 
 * ---------------------------------------------------------
 * Adds a new node to the external usages linked list.
 * ---------------------------------------------------------
 */
void add_extern_usage(ExternNode **head, const char *name, int address) {
    ExternNode *new_node = (ExternNode *)malloc(sizeof(ExternNode));
    ExternNode *temp = *head;

    if (new_node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for ExternNode.\n");
        exit(1);
    }

    strcpy(new_node->name, name);
    new_node->address = address;
    new_node->next = NULL;

    if (*head == NULL) {
        *head = new_node;
        return;
    }

    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = new_node;
}

/* 
 * ---------------------------------------------------------
 * Frees all memory allocated for the external usages list.
 * ---------------------------------------------------------
 */
void free_extern_list(ExternNode **head) {
    ExternNode *temp;

    while (*head != NULL) {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}
