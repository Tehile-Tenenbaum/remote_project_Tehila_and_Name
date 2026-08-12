#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "symbol_table.h"

/*
 * Searches for a symbol by name in the symbol table.
 * Returns a pointer to the symbol node, or NULL if not found.
 */
SymbolNode* find_symbol(SymbolNode *head, char *name) {
    SymbolNode *current = head;
    
    /* Iterate through the linked list */
    while (current != NULL) {
        /* Compare the requested name with the current node's name */
        if (strcmp(current->name, name) == 0) {
            return current; /* Match found */
        }
        current = current->next; /* Move to the next node */
    }
    
    return NULL; /* Symbol not found */
}

/*
 * Adds a new symbol to the symbol table at the head of the list.
 * Validates symbol length and checks for duplicates.
 * Returns TRUE on success, or FALSE if validation or allocation failed.
 */
boolean add_symbol(SymbolNode **head, char *name, int address, SymbolType type) {
    SymbolNode *new_node;

    /* 1. Validation: Check if the symbol name exceeds the maximum allowed length (31 characters) */
    if (strlen(name) > MAX_LABEL_LENGTH) {
        printf("Error: Symbol name '%s' exceeds the maximum allowed length of 31 characters.\n", name);
        return FALSE;
    }

    /* 2. Duplicate check: Verify if the symbol already exists in the table */
    if (find_symbol(*head, name) != NULL) {
        /* Print an error message regarding the duplicate symbol */
        printf("Error: Symbol '%s' is already defined.\n", name);
        return FALSE; 
    }

    /* 3. Memory allocation for the new node */
    new_node = (SymbolNode *)malloc(sizeof(SymbolNode));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed for symbol table.\n");
        return FALSE;
    }

    /* 4. Populate the new node with data */
    /* Since length was validated in step 1, using standard strcpy is safe here */
    strcpy(new_node->name, name);
    new_node->address = address;
    new_node->type = type;
    new_node->is_entry = 0; /* Default: not an entry point until proven otherwise in Pass 2 */

    /* 5. Insert at the head of the list - Max efficiency O(1) */
    new_node->next = *head; /* Point the new node to the current head of the list */
    *head = new_node;       /* Set the new node as the new head of the list */

    return TRUE; /* Successfully added */
}

/*
 * Frees all dynamically allocated memory for the symbol table.
 */
void free_symbol_table(SymbolNode *head) {
    SymbolNode *current = head;
    SymbolNode *next_node;

    /* Traverse the list and free each node */
    while (current != NULL) {
        next_node = current->next; /* Store the next pointer before freeing */
        free(current);             /* Free the current node */
        current = next_node;       /* Advance to the next node */
    }
}
/*
 * Function: update_data_symbols
 * -----------------------------
 * Updates the addresses of all data symbols in the symbol table at the end of Pass 1.
 * Adds the final Instruction Counter (ICF) to the value of each symbol with the 'data' attribute.
 * 
 * @param head: Pointer to the head of the symbol table.
 * @param ICF: The final Instruction Counter value.
 */
void update_data_symbols(SymbolNode *head, int ICF) {
    SymbolNode *current = head;
    
    /* עוברים על כל הצמתים ברשימה המקושרת */
    while (current != NULL) {
        /* אם המאפיין של הסמל הוא נתונים */
        if (current->attribute == ATTRIBUTE_DATA) {
            /* מוסיפים לערך שלו את ICF */
            current->value += ICF;
        }
        current = current->next;
    }
}
