#ifndef UTILS_H
#define UTILS_H

#include "globals.h"
#include "macro.h"

/* checks if a word is a reserved assembly word */
boolean is_reserved_word(const char *word);

/* skips spaces and tabs in a string */
void skip_white_spaces(char *line, int *index);

/* checks if there is any garbage left at the end of the line */
boolean check_no_garbage(char *line, int current_index);

/* checks if a name is already used by a macro */
boolean is_macro_name(char *name, MacroNode *macro_head);

/* checks if a label name is valid syntactically */
boolean is_valid_label_name(char *name);

#endif /* UTILS_H */
