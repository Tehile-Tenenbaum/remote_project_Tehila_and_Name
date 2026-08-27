#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"

/* symbol types */
#define SYMBOL_TYPE_CODE 1
#define SYMBOL_TYPE_DATA 2
#define SYMBOL_TYPE_EXTERNAL 3

/* symbol entry flags */
#define SYMBOL_NOT_ENTRY 0
#define SYMBOL_IS_ENTRY 1

/* return statuses */
#define SYMBOL_SUCCESS 1
#define SYMBOL_FAILURE 0

/* node for the symbol table linked list */
typedef struct symbol_node {
    char name[MAX_LINE_LENGTH];
    int address;
    int type;
    int is_entry;
    struct symbol_node *next;
} SymbolNode;

/* functions for symbol table */
boolean add_symbol(SymbolNode **head, const char *name, int address, int type, int line_number, const char *filename);
SymbolNode* find_symbol(SymbolNode *head, const char *name);
void free_symbol_table(SymbolNode *head);
void update_data_symbols(SymbolNode *head, int ICF);
void print_symbol_table(SymbolNode *head);
int mark_symbol_as_entry(SymbolNode *head, const char *name);
int find_symbol_address(SymbolNode *head, const char *name, int *address);

#endif /* SYMBOL_TABLE_H */
