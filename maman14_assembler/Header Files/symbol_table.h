#ifndef SYMBOL_H
#define SYMBOL_H
#include "globals.h"
#define SYMBOL_TYPE_CODE 1
#define SYMBOL_TYPE_DATA 2
#define SYMBOL_TYPE_EXTERNAL 3
/* Definition of symbol types according to the course requirements */

/* Structure representing a single symbol (label) node in the linked list */
typedef struct symbol_node {
    char name[MAX_LINE_LENGTH];
    int address;
    int type;
    int is_entry;
    struct symbol_node *next;
} SymbolNode;
/* --- Public Functions for Symbol Table Management --- */
/*
 * Adds a new symbol to the symbol table.
 * Returns TRUE on success, or FALSE if allocation failed or symbol already exists.
 */
boolean add_symbol(SymbolNode **head, char *name, int address, int type,int line_number);
/*
 * Searches for a symbol by name in the symbol table.
 * Returns a pointer to the symbol node, or NULL if not found.
 */
SymbolNode* find_symbol(SymbolNode *head, char *name);
/*
 * Frees all dynamically allocated memory for the symbol table.
 */
void free_symbol_table(SymbolNode *head);
void update_data_symbols(SymbolNode *head, int ICF);
void print_symbol_table(SymbolNode *head);
int mark_symbol_as_entry(SymbolNode *head, char *name);
int find_symbol_address(SymbolNode *head, const char *name, int *address);
#endif /* SYMBOL_H */
