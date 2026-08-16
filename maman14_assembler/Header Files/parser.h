#ifndef PARSER_H
#define PARSER_H
/* Assuming globals.h defines MAX_LINE_LENGTH, TRUE, FALSE, and boolean */
#include "globals.h"
#include "image.h"

boolean process_data_directive(char *line, int *index, int *DC, char *directive, DataNode **data_head);
boolean process_instruction(char *line, int *index, int *IC, char *operation, InstructionNode **inst_head);
void add_to_data_image(long value, int size_in_bytes, int *DC, DataNode **data_head);
void add_to_code_image(unsigned long machine_code, int *IC, InstructionNode **inst_head);
boolean is_data_directive(char *word);
boolean is_entry_directive(char *word);
boolean is_extern_directive(char *word);
#endif /* PARSER_H */
