#ifndef PASS2_H
#define PASS2_H

#include <stdio.h>
#include "image.h"
#include "symbol_table.h"
#include "output_files.h"
#include "globals.h"

/* Return Codes */
#define PASS2_SUCCESS 1
#define PASS2_FAILURE 0

/**
 * @brief Executes Pass 2 of the assembler algorithm.
 *        1. Reads the source file line by line.
 *        2. Marks .entry symbols in the symbol table.
 *        3. Completes missing addresses in I-type (branches) and J-type instructions.
 * 
 * @param filename  The base name of the source assembly file (.am) to read.
 * @param sym_head  Pointer to the head of the symbol table.
 * @param inst_head Pointer to the head of the instruction image list.
 * @param ext_head  Pointer to the head of the externals list (passed by reference).
 * @return int      PASS2_SUCCESS if Pass 2 completed successfully, PASS2_FAILURE if errors occurred.
 */
int execute_pass2(const char *filename, SymbolNode *sym_head, InstructionNode *inst_head, ExternNode **ext_head);

#endif /* PASS2_H */
