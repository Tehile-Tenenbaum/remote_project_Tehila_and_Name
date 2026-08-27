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
#include "error.h"

int main(int argc, char *argv[]) {
    int i;

    /* check if files were provided */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1.as> <file2.as> ...\n", argv[0]);
        return 1;
    }

    /* loop through each file */
    for (i = 1; i < argc; i++) {
        char *full_filename = argv[i];
        char *base_filename = NULL;
        size_t len = strlen(full_filename);
        
        int ICF = 0;
        int DCF = 0;
     
        /* data structures for the current file */
        SymbolNode *symbol_table = NULL;
        InstructionNode *instruction_image = NULL;
        DataNode *data_image = NULL;
        ExternNode *extern_list = NULL; 
        MacroNode *macro_table = NULL;
        
        /* reset global error flag */
        error_found = FALSE;
        
        /* verify .as extension */
        if (len < 3 || strcmp(full_filename + len - 3, ".as") != 0) {
            fprintf(stderr, "Error: File '%s' does not end with '.as'. Skipping.\n", full_filename);
            continue;
        }
        
        /* alloc memory for the base filename */
        base_filename = (char *)malloc(len - 3 + 1);
        if (base_filename == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for '%s'.\n", full_filename);
            continue;
        }
        
        /* extract base filename without extension */
        strncpy(base_filename, full_filename, len - 3);
        base_filename[len - 3] = '\0';

        printf("\n========================================\n");
        printf("Processing file: %s\n", full_filename);
        printf("========================================\n");

        /* Step 1: Pre-assembler (Macro Unrolling) */
        if (!process_macros(base_filename, &macro_table)) {
            printf("Errors found in pre-assembler. Skipping file '%s'.\n", full_filename);
            free(base_filename);
            continue; 
        }
     
        /* Step 2: Pass 1 */
        if (!execute_pass1(base_filename, &symbol_table, &instruction_image, &data_image, &ICF, &DCF, macro_table)) {
            printf("Errors found in Pass 1. Skipping file '%s'.\n", full_filename);
            free_symbol_table(symbol_table);
            free_images(&instruction_image, &data_image);
            free_macro_table(macro_table);
            free(base_filename);
            continue;
        }

        /* Step 3: Pass 2 */
        if (!execute_pass2(base_filename, symbol_table, instruction_image, &extern_list)) {
            printf("Errors found in Pass 2. Skipping file '%s'.\n", full_filename);
            free_symbol_table(symbol_table);
            free_images(&instruction_image, &data_image);
            free_extern_list(&extern_list);
            free_macro_table(macro_table);
            free(base_filename);
            continue;
        }

        /* Step 4: Generate Output Files */
        printf("Compilation successful! Generating output files...\n");
        create_ob_file(base_filename, instruction_image, data_image, ICF, DCF);
        create_ent_file(base_filename, symbol_table);
        create_ext_file(base_filename, extern_list);

        /* Step 5: Clean up memory */
        free_symbol_table(symbol_table);
        free_images(&instruction_image, &data_image);
        free_extern_list(&extern_list);
        free_macro_table(macro_table);
        free(base_filename);
        
        printf("Finished processing '%s'.\n", full_filename);
    }

    return 0;
}
