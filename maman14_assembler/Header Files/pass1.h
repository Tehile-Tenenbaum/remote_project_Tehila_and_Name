#ifndef PASS1_H
#define PASS1_H

#include "globals.h"
#include "symbol_table.h"

/*
 * Performs the first pass of the assembler on the given assembly file (.am).
 * Builds the symbol table, processes data and instruction lines, 
 * updates IC (Instruction Counter) and DC (Data Counter), and populates memory images.
 * * @param filename The name of the .am file to process.
 * @param symbol_table A pointer to the head of the symbol table.
 * @return TRUE (1) if the first pass completed successfully without errors, FALSE (0) otherwise.
 */
boolean execute_pass1(char *filename, SymbolNode **symbol_table);

#endif /* PASS1_H */
