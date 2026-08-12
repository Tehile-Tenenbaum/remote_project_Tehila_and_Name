#ifndef SYMBOL_H
#define SYMBOL_H
#include "globals.h"
#define ATTRIBUTE_CODE 1
#define ATTRIBUTE_DATA 2
#define ATTRIBUTE_EXTERNAL 3
/* Definition of symbol types according to the course requirements */
typedef enum {
    SYMBOL_TYPE_CODE,
    SYMBOL_TYPE_DATA,
    SYMBOL_TYPE_EXTERN
} SymbolType;
/* Structure representing a single symbol (label) node in the linked list */
typedef struct symbol_node {
    char name[MAX_LINE_LENGTH];
    int address;
    SymbolType type;
    int is_entry;
    struct symbol_node *next;
} SymbolNode;
/* --- Public Functions for Symbol Table Management --- */
/*
 * Adds a new symbol to the symbol table.
 * Returns TRUE on success, or FALSE if allocation failed or symbol already exists.
 */
boolean add_symbol(SymbolNode **head, char *name, int address, SymbolType type);
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
#endif /* SYMBOL_H */
