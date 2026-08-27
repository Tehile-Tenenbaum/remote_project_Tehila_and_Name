#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

#include "image.h"
#include "symbol_table.h"

/**
 * @brief Structure to hold information about external symbol usages.
 *        This is used to build the .ext file.
 */
typedef struct ExternNode {
    char name[32];      /* The name of the external symbol */
    int address;        /* The address in memory where it was used */
    struct ExternNode *next;
} ExternNode;

/* --- Function Prototypes --- */

/**
 * @brief Creates the .ob (Object) file containing the machine code.
 * 
 * @param filename The base name of the file.
 * @param inst_head Pointer to the head of the instruction image list.
 * @param data_head Pointer to the head of the data image list.
 * @param icf The final Instruction Counter value.
 * @param dcf The final Data Counter value.
 */
void create_ob_file(const char *filename, InstructionNode *inst_head, DataNode *data_head, int icf, int dcf);

/**
 * @brief Creates the .ent (Entries) file.
 *        Only creates the file if there is at least one .entry symbol.
 * 
 * @param filename The base name of the file.
 * @param sym_head Pointer to the head of the symbol table.
 */
void create_ent_file(const char *filename, SymbolNode *sym_head);

/**
 * @brief Creates the .ext (Externals) file.
 *        Only creates the file if there is at least one external usage.
 * 
 * @param filename The base name of the file.
 * @param ext_head Pointer to the head of the external usages list.
 */
void create_ext_file(const char *filename, ExternNode *ext_head);

/**
 * @brief Adds a new node to the external usages linked list.
 * 
 * @param head Pointer to the head of the external usages list.
 * @param name The name of the external symbol.
 * @param address The address where the symbol was used.
 */
void add_extern_usage(ExternNode **head, const char *name, int address);

/**
 * @brief Frees all memory allocated for the external usages list.
 * 
 * @param head Pointer to the head of the external usages list.
 */
void free_extern_list(ExternNode **head);

#endif /* OUTPUT_FILES_H */
