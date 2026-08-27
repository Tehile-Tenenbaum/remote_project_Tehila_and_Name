#ifndef PASS1_H
#define PASS1_H

#include "globals.h"
#include "symbol_table.h"
#include "image.h"
#include "macro.h"

#define FILE_EXT_LEN 4 /* length of extensions like ".am\0" */

/* 
 * executes pass 1 of the assembler: builds symbol table, handles data,
 * counts IC/DC and catches errors 
 */
boolean execute_pass1(const char *filename, SymbolNode **symbol_table, InstructionNode **inst_head, DataNode **data_head, int *ICF, int *DCF, MacroNode *macro_table);

#endif /* PASS1_H */
