#ifndef UTILS_H
#define UTILS_H
/* Assuming globals.h defines MAX_LINE_LENGTH, TRUE, FALSE, and boolean */
#include "globals.h"
#include "macro.h"
/*
 * Checks if a given word is a reserved assembly word.
 * Returns TRUE if it is reserved, FALSE otherwise.
 */
boolean is_reserved_word(char *word);
/*
 * Advances the index to skip any spaces or tabs in a string.
 * Modifies the index directly via a pointer.
 */
void skip_white_spaces(char *line, int *index);
/*
 * Checks if there are any extra characters (garbage) left in the line.
 * Returns TRUE if the rest of the line is clean, FALSE if garbage is found.
 */
boolean check_no_garbage(char *line, int current_index);
boolean is_macro_name(char *name, MacroNode *macro_head);
boolean is_valid_label_name(char *name);


#endif /* UTILS_H */
