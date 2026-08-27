#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"

/* Symbol Types */
#define SYMBOL_TYPE_CODE 1
#define SYMBOL_TYPE_DATA 2
#define SYMBOL_TYPE_EXTERNAL 3

/* Symbol Entry Flags */
#define SYMBOL_NOT_ENTRY 0
#define SYMBOL_IS_ENTRY 1

/* Return Statuses */
#define SYMBOL_SUCCESS 1
#define SYMBOL_FAILURE 0

/**
 * @brief Structure representing a single symbol (label) node in the linked list.
 */
typedef struct symbol_node {
    char name[MAX_LINE_LENGTH];
    int address;
    int type;
    int is_entry;
    struct symbol_node *next;
} SymbolNode;

/* --- Public Functions for Symbol Table Management --- */

/**
 * @brief Adds a new symbol to the symbol table.
 * @param head Pointer to the head of the symbol table.
 * @param name The name of the symbol.
 * @param address The address of the symbol.
 * @param type The type of the symbol (CODE, DATA, EXTERNAL).
 * @param line_number Current line number (for error reporting).
 * @param filename Name of the source file (for error reporting).
 * @return boolean TRUE on success, FALSE if validation or allocation failed.
 */
boolean add_symbol(SymbolNode **head, const char *name, int address, int type, int line_number, const char *filename);

/**
 * @brief Searches for a symbol by name in the symbol table.
 * @param head Pointer to the head of the symbol table.
 * @param name The name of the symbol to find.
 * @return SymbolNode* Pointer to the found symbol node, or NULL if not found.
 */
SymbolNode* find_symbol(SymbolNode *head, const char *name);

/**
 * @brief Frees all dynamically allocated memory for the symbol table.
 * @param head Pointer to the head of the symbol table.
 */
void free_symbol_table(SymbolNode *head);

/**
 * @brief Updates the addresses of all data symbols at the end of Pass 1.
 * @param head Pointer to the head of the symbol table.
 * @param ICF The final Instruction Counter value.
 */
void update_data_symbols(SymbolNode *head, int ICF);

/**
 * @brief Prints the symbol table (for debugging purposes).
 * @param head Pointer to the head of the symbol table.
 */
void print_symbol_table(SymbolNode *head);

/**
 * @brief Marks an existing symbol as an entry point (.entry).
 * @param head Pointer to the head of the symbol table.
 * @param name The name of the symbol to update.
 * @return int SYMBOL_SUCCESS if found and updated, SYMBOL_FAILURE otherwise.
 */
int mark_symbol_as_entry(SymbolNode *head, const char *name);

/**
 * @brief Searches for a symbol and retrieves its address.
 * @param head Pointer to the head of the symbol table.
 * @param name The name of the symbol to find.
 * @param address Pointer to store the found address.
 * @return int SYMBOL_SUCCESS on success, SYMBOL_FAILURE if not found.
 */
int find_symbol_address(SymbolNode *head, const char *name, int *address);

#endif /* SYMBOL_TABLE_H */
