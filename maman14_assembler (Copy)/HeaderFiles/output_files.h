#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

#include "image.h"
#include "symbol_table.h"

/* linked list node to keep track of external symbol usages */
typedef struct ExternNode {
    char name[32];      /* name of the external symbol */
    int address;        /* address where it was used */
    struct ExternNode *next;
} ExternNode;

/* creates the .ob file */
void create_ob_file(const char *filename, InstructionNode *inst_head, DataNode *data_head, int icf, int dcf);

/* creates the .ent file */
void create_ent_file(const char *filename, SymbolNode *sym_head);

/* creates the .ext file */
void create_ext_file(const char *filename, ExternNode *ext_head);

/* adds an external usage to the list */
void add_extern_usage(ExternNode **head, const char *name, int address);

/* frees the external usage list */
void free_extern_list(ExternNode **head);

#endif /* OUTPUT_FILES_H */
