#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>
#include "globals.h"

/* represents a single line of text inside a macro */
typedef struct macro_line {
    char line[MAX_LINE_LENGTH];
    struct macro_line *next;
} MacroLine;

/* represents a macro node with name and its lines */
typedef struct macro_node {
    char name[MAX_LINE_LENGTH];
    MacroLine *content;         /* pointer to the head of the macro's lines list */
    struct macro_node *next;    /* pointer to the next macro */
} MacroNode;

/* main pre-processor function */
boolean process_macros(const char *base_filename, MacroNode **out_macro_head);

/* frees all memory allocated in the macro table */
void free_macro_table(MacroNode *head);

#endif /* MACRO_H */
