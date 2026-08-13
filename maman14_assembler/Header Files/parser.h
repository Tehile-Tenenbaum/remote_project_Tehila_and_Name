#ifndef PARSER_H
#define PARSER_H
/* Assuming globals.h defines MAX_LINE_LENGTH, TRUE, FALSE, and boolean */
#include "globals.h"

boolean is_data_directive(char *word);
boolean is_entry_directive(char *word);
boolean is_extern_directive(char *word);
boolean process_data_directive(char *line, int *index, int *DC, char *directive);
boolean process_instruction(char *line, int *index, int *IC, char *operation);
#endif /* PARSER_H */
