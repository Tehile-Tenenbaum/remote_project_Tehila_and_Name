#ifndef MACRO_H
#define MACRO_H

#include <stdio.h>
#include "globals.h"

/**
 * @brief Structure representing a single line of text inside a macro.
 */
typedef struct macro_line {
    char line[MAX_LINE_LENGTH];
    struct macro_line *next;
} MacroLine;

/**
 * @brief Structure representing a macro node (contains name and list of lines).
 */
typedef struct macro_node {
    char name[MAX_LINE_LENGTH];
    MacroLine *content;         /* Pointer to the head of the macro's lines list */
    struct macro_node *next;    /* Pointer to the next macro in the table */
} MacroNode;

/* --- Public function declarations --- */

/**
 * @brief Main function for the macro pre-processor stage.
 *        Receives the base filename, opens the .as file, expands macros,
 *        and creates the clean .am file.
 * 
 * @param base_filename The base name of the source file (without extension).
 * @param out_macro_head Pointer to store the head of the populated macro table.
 * @return boolean TRUE if the process succeeded without errors, FALSE otherwise.
 */
boolean process_macros(const char *base_filename, MacroNode **out_macro_head);

/**
 * @brief Frees all dynamically allocated memory in the macro table.
 * 
 * @param head Pointer to the head of the macro table.
 */
void free_macro_table(MacroNode *head);

#endif /* MACRO_H */

