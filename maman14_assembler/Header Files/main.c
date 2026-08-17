#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "image.h"
#include "symbol_table.h"
#include "pass1.h"
#include "pass2.h"
#include "output_files.h"
#include "macro.h"
#include "parser.h"
#include "utils.h"

/* 
 * Assuming you will have a pre-assembler function.
 * If you haven't written it yet, this is just a placeholder signature.
 */
boolean execute_pre_assembler(const char *filename);

int main(int argc, char *argv[]) {
    int i;

    /* Check if the user provided any files */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> <file2> ...\n", argv[0]);
        return 1;
    }

    /* Loop through each file provided in the command line arguments */
    for (i = 1; i < argc; i++) {
        char *filename = argv[i];
        int ICF = 0;
        int DCF = 0;
        /* Data structures for the current file */
        SymbolNode *symbol_table = NULL;
        InstructionNode *instruction_image = NULL;
        DataNode *data_image = NULL;
        ExternNode *extern_list = NULL; /* To track .ext usages */
        MacroNode *macro_table = NULL;
        
        
        printf("\n========================================\n");
        printf("Processing file: %s.as\n", filename);
        printf("========================================\n");

        /* Step 1: Pre-assembler (Macro Unrolling) */
        /* Takes filename.as and creates filename.am */
        if (!process_macros(filename,&macro_table)) {
            printf("Errors found in pre-assembler. Skipping file '%s'.\n", filename);
            continue; /* Move to the next file */
        }

        /* Step 2: Pass 1 */
        /* Reads filename.am, builds symbol table, and partially builds images */
 if (!execute_pass1(filename, &symbol_table, &instruction_image, &data_image, &ICF, &DCF,macro_table)) {
            printf("Errors found in Pass 1. Skipping file '%s'.\n", filename);
            /* Free memory before skipping */
            free_symbol_table(symbol_table);
            free_macro_table(macro_table);
            continue;
        }

        /* Step 3: Pass 2 */
        /* Reads filename.am again, completes addresses, and builds extern_list */
        /* Note: I added extern_list to the arguments here so pass2 can populate it */
if (!execute_pass2(filename, symbol_table, instruction_image, &extern_list)) {
            printf("Errors found in Pass 2. Skipping file '%s'.\n", filename);
            /* Free memory before skipping */
            free_symbol_table(symbol_table);
            free_images(&instruction_image, &data_image);
            free_extern_list(&extern_list);
            free_macro_table(macro_table);
            continue;
        }

        /* Step 4: Output Files Generation */
        /* If we reached here, there are NO errors. We can create the files. */
        printf("Compilation successful! Generating output files...\n");
        create_ob_file(filename, instruction_image, data_image, ICF, DCF);
        create_ent_file(filename, symbol_table);
        create_ext_file(filename, extern_list);

        /* Step 5: Clean up memory before moving to the next file */
        free_symbol_table(symbol_table);
        free_images(&instruction_image, &data_image);
        free_extern_list(&extern_list);
        free_macro_table(macro_table);
        
        printf("Finished processing '%s'.\n", filename);
    }

    return 0;
}
