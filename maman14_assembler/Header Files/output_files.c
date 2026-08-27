#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "symbol_table.h"
#include "output_files.h"
#include "globals.h"
#include "error.h"

/* Constants for bitwise operations and formatting */
#define BYTE_MASK       0xFF
#define SHIFT_8_BITS    8
#define SHIFT_16_BITS   16
#define SHIFT_24_BITS   24
#define BYTES_PER_LINE  4

void create_ob_file(const char *filename, InstructionNode *inst_head, DataNode *data_head, int icf, int dcf) {
    FILE *file;
    char *ob_filename;
    size_t len;
    InstructionNode *inst_curr = inst_head;
    DataNode *data_curr = data_head;
    int data_byte_count = 0;
    
    /* הקצאה דינמית לשם הקובץ */
    len = strlen(filename);
    ob_filename = (char *)malloc(len + 4); /* ".ob" + '\0' */
    if (ob_filename == NULL) {
        printf("Memory allocation failed for .ob filename.\n");
        return;
    }
    
    sprintf(ob_filename, "%s.ob", filename);
    
    file = fopen(ob_filename, "w");
    if (file == NULL) {
        report_error(ob_filename, 0, "Cannot create output file.");
        free(ob_filename);
        return;
    }
    
    /* Print Header: Instruction image size and Data image size */
    fprintf(file, "     %d %d\n", icf - IC_INIT_VALUE, dcf);
    
    /* 1. Print Instruction Image */
    while (inst_curr != NULL) {
        unsigned int val = inst_curr->word.machine_code; 
        
        fprintf(file, "%04d %02X %02X %02X %02X\n", 
                inst_curr->address,
                val & BYTE_MASK,
                (val >> SHIFT_8_BITS) & BYTE_MASK,
                (val >> SHIFT_16_BITS) & BYTE_MASK,
                (val >> SHIFT_24_BITS) & BYTE_MASK);
                
        inst_curr = inst_curr->next;
    }
    
    /* 2. Print Data Image (4 bytes per line format) */
    while (data_curr != NULL) {
        if (data_byte_count % BYTES_PER_LINE == 0) {
            if (data_byte_count > 0) {
                fprintf(file, "\n"); 
            }
            fprintf(file, "%04d", data_curr->address + icf);
        }
        
        fprintf(file, " %02X", data_curr->byte & BYTE_MASK);
        
        data_byte_count++;
        data_curr = data_curr->next;
    }
    
    if (data_byte_count > 0) {
        fprintf(file, "\n");
    }
    
    fclose(file);
    free(ob_filename); /* שחרור הזיכרון */
}

void create_ent_file(const char *filename, SymbolNode *sym_head) {
    SymbolNode *curr = sym_head;
    int has_entry = 0;
    FILE *file;
    char *ent_filename;
    size_t len;

    /* Check if there are any entry symbols */
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

    /* הקצאה דינמית לשם הקובץ */
    len = strlen(filename);
    ent_filename = (char *)malloc(len + 5); /* ".ent" + '\0' */
    if (ent_filename == NULL) {
        printf("Memory allocation failed for .ent filename.\n");
        return;
    }

    sprintf(ent_filename, "%s.ent", filename);
    file = fopen(ent_filename, "w");
    if (file == NULL) {
        report_error(ent_filename, 0, "Cannot create output file.");
        free(ent_filename);
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
    free(ent_filename); /* שחרור הזיכרון */
}

void create_ext_file(const char *filename, ExternNode *ext_head) {
    FILE *file;
    char *ext_filename;
    size_t len;
    ExternNode *curr = ext_head;

    if (ext_head == NULL) {
        return; /* No external usages found, do not create .ext file */
    }

    /* הקצאה דינמית לשם הקובץ */
    len = strlen(filename);
    ext_filename = (char *)malloc(len + 5); /* ".ext" + '\0' */
    if (ext_filename == NULL) {
        printf("Memory allocation failed for .ext filename.\n");
        return;
    }

    sprintf(ext_filename, "%s.ext", filename);
    file = fopen(ext_filename, "w");
    if (file == NULL) {
        report_error(ext_filename, 0, "Cannot create output file.");
        free(ext_filename);
        return;
    }

    /* Print all external usages and the addresses where they were used */
    while (curr != NULL) {
        fprintf(file, "%s %04d\n", curr->name, curr->address);
        curr = curr->next;
    }

    fclose(file);
    free(ext_filename); /* שחרור הזיכרון */
}

void add_extern_usage(ExternNode **head, const char *name, int address) {
    ExternNode *new_node = (ExternNode *)malloc(sizeof(ExternNode));
    ExternNode *temp = *head;

    if (new_node == NULL) {
        report_error("Internal", 0, "Memory allocation failed for ExternNode.");
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

void free_extern_list(ExternNode **head) {
    ExternNode *temp;

    while (*head != NULL) {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}
