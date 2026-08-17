#ifndef MACRO_H
#define MACRO_H
#include <stdio.h>
/* Assuming globals.h defines MAX_LINE_LENGTH and the boolean type (TRUE/FALSE) */
#include "globals.h"
/* --- Data structures for the Macro Table (Linked List of Linked Lists) --- */
/* Structure representing a single line of text inside a macro */
typedef struct macro_line {
    char line[MAX_LINE_LENGTH];
    struct macro_line *next;
} MacroLine;
/* Structure representing a macro node (contains name and list of lines) */
typedef struct macro_node {
    char name[MAX_LINE_LENGTH];
    MacroLine *content;         /* Pointer to the head of the macro's lines list */
    struct macro_node *next;    /* Pointer to the next macro in the table */
} MacroNode;
/* --- Public function declarations --- */
/*
 * Main function for the macro pre-processor stage.
 * Receives the base filename, opens the .as file, expands macros,
 * and creates the clean .am file.
 * Returns TRUE if the process succeeded without errors, FALSE otherwise.
 */
boolean process_macros(char *base_filename, MacroNode **out_macro_head);
void free_macro_table(MacroNode *head);

#endif /* MACRO_H */
