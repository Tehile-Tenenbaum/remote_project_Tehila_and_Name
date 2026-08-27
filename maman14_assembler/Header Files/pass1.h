#ifndef PASS1_H
#define PASS1_H

#include "globals.h"
#include "symbol_table.h"
#include "image.h"
#include "macro.h"

/**
 * @brief Performs the first pass of the assembler on the given assembly file (.am).
 *        Builds the symbol table, processes data and instruction lines, 
 *        updates IC (Instruction Counter) and DC (Data Counter), and populates memory images.
 * 
 * @param filename The base name of the .am file to process (without extension).
 * @param symbol_table A pointer to the head of the symbol table.
 * @param inst_head A pointer to the head of the instruction image.
 * @param data_head A pointer to the head of the data image.
 * @param ICF A pointer to store the final Instruction Counter.
 * @param DCF A pointer to store the final Data Counter.
 * @param macro_table A pointer to the head of the macro table.
 * @return boolean TRUE if Pass 1 completed successfully without errors, FALSE otherwise.
 */
boolean execute_pass1(const char *filename, SymbolNode **symbol_table, InstructionNode **inst_head, DataNode **data_head, int *ICF, int *DCF, MacroNode *macro_table);

#endif /* PASS1_H */
