#ifndef PASS2_H
#define PASS2_H

/* --- Required Includes --- */
#include <stdio.h>
#include "image.h"
#include "symbol_table.h"
#include "output_files.h"

/* 
 * ---------------------------------------------------------
 * Executes Pass 2 of the assembler algorithm.
 * 
 * @param filename  - The source assembly file (.as) to read.
 * @param inst_head - Pointer to the head of the instruction image list.
 * @param sym_head  - Pointer to the head of the symbol table.
 * @return int      - 1 if Pass 2 completed successfully, 0 if errors occurred.
 * ---------------------------------------------------------
 */
int execute_pass2(const char *filename, SymbolNode *sym_head, InstructionNode *inst_head, ExternNode **ext_head);

#endif /* PASS2_H */
