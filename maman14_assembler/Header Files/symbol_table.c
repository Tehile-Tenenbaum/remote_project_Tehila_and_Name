#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symbol_table.h"
#include "utils.h"
#include "error.h"

SymbolNode* find_symbol(SymbolNode *head, const char *name) {
    SymbolNode *current = head;
    
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

boolean add_symbol(SymbolNode **head, const char *name, int address, int type, int line_number, const char *filename) {
    SymbolNode *new_node;

    /* 1. Validation: Check maximum allowed length */
    if (strlen(name) > MAX_LABEL_LENGTH) {
        report_error(filename, line_number, "Symbol name '%s' exceeds the maximum allowed length.", name);
        return FALSE;
    }

    /* 2. Duplicate check */
    if (find_symbol(*head, name) != NULL) {
        report_error(filename, line_number, "Symbol '%s' is already defined.", name);
        return FALSE; 
    }

    /* 3. Memory allocation */
    new_node = (SymbolNode *)malloc(sizeof(SymbolNode));
    if (new_node == NULL) {
        report_error(filename, line_number, "Memory allocation failed for symbol table.");
        return FALSE;
    }

    /* 4. Populate node */
    strcpy(new_node->name, name);
    new_node->address = address;
    new_node->type = type;
    new_node->is_entry = SYMBOL_NOT_ENTRY;

    /* 5. Insert at head */
    new_node->next = *head;
    *head = new_node;

    return TRUE;
}

void free_symbol_table(SymbolNode *head) {
    SymbolNode *current = head;
    SymbolNode *next_node;

    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
}

void update_data_symbols(SymbolNode *head, int ICF) {
    SymbolNode *current = head;
    
    while (current != NULL) {
        if (current->type == SYMBOL_TYPE_DATA) {
            current->address += ICF;
        }
        current = current->next;
    }
}

void print_symbol_table(SymbolNode *head) {
    SymbolNode *current = head;
    printf("\n--- SYMBOL TABLE DUMP ---\n");
    while (current != NULL) {
        printf("Symbol Name: %s | Address: %d | Type: %d\n", current->name, current->address, current->type);
        current = current->next;
    }
    printf("-------------------------\n");
}

int mark_symbol_as_entry(SymbolNode *head, const char *name) {
    SymbolNode *current = head;
    
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            current->is_entry = SYMBOL_IS_ENTRY;
            return SYMBOL_SUCCESS;
        }
        current = current->next;
    }
    return SYMBOL_FAILURE;
}

int find_symbol_address(SymbolNode *head, const char *name, int *address) {
    SymbolNode *current = head;
    
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            *address = current->address;
            return SYMBOL_SUCCESS;
        }
        current = current->next;
    }
    return SYMBOL_FAILURE;
}

 
