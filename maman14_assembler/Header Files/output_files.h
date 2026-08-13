#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

#include "image.h"
#include "symbol_table.h"

/* 
 * Structure to hold information about external symbol usages.
 * This is used to build the .ext file.
 */
typedef struct ExternNode {
    char name[32];      /* The name of the external symbol */
    int address;        /* The address in memory where it was used */
    struct ExternNode *next;
} ExternNode;

/* Function Prototypes */

/* Creates the .ob (Object) file containing the machine code */
void create_ob_file(const char *filename, InstructionNode *inst_head, DataNode *data_head, int icf, int dcf);

/* Creates the .ent (Entries) file */
void create_ent_file(const char *filename, SymbolNode *sym_head);

/* Creates the .ext (Externals) file */
void create_ext_file(const char *filename, ExternNode *ext_head);

/* Helper function to add a node to the external usages list */
void add_extern_usage(ExternNode **head, const char *name, int address);

/* Helper function to free the external usages list */
void free_extern_list(ExternNode **head);

#endif /* OUTPUT_FILES_H */
