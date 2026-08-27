#ifndef PASS2_H
#define PASS2_H

#include <stdio.h>
#include "image.h"
#include "symbol_table.h"
#include "output_files.h"
#include "globals.h"

#define PASS2_SUCCESS 1
#define PASS2_FAILURE 0

/* 
 * executes pass 2: completes branching/jump addresses and builds the externals list 
 */
int execute_pass2(const char *filename, SymbolNode *sym_head, InstructionNode *inst_head, ExternNode **ext_head);

#endif /* PASS2_H */
