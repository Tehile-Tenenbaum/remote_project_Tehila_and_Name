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

    /* Check if the user provided any files */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1.as> <file2.as> ...\n", argv[0]);
        return 1;
    }

    /* Loop through each file provided in the command line arguments */
    for (i = 1; i < argc; i++) {
        char *full_filename = argv[i];
        char *base_filename = NULL;
        size_t len = strlen(full_filename);
        
        int ICF = 0;
        int DCF = 0;
     
        /* Data structures for the current file */
        SymbolNode *symbol_table = NULL;
        InstructionNode *instruction_image = NULL;
        DataNode *data_image = NULL;
        ExternNode *extern_list = NULL; 
        MacroNode *macro_table = NULL;
        
        /* Reset the global error flag for each new file */
        error_found = FALSE;
        
        /* 1. בדיקת סיומת .as */
        if (len < 3 || strcmp(full_filename + len - 3, ".as") != 0) {
            fprintf(stderr, "Error: File '%s' does not end with '.as'. Skipping.\n", full_filename);
            continue;
        }
        
        /* 2. הקצאה דינמית לשם הקובץ ללא הסיומת */
        /* אורך שם הבסיס הוא אורך השם המלא פחות 3 תווים של הסיומת, ועוד תו אחד לסיום המחרוזת */
        base_filename = (char *)malloc(len - 3 + 1);
        if (base_filename == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for '%s'.\n", full_filename);
            continue;
        }
        
        /* העתקת שם הבסיס ללא הסיומת .as */
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

        /* Step 4: Output Files Generation */
        printf("Compilation successful! Generating output files...\n");
        create_ob_file(base_filename, instruction_image, data_image, ICF, DCF);
        create_ent_file(base_filename, symbol_table);
        create_ext_file(base_filename, extern_list);

        /* Step 5: Clean up memory before moving to the next file */
        free_symbol_table(symbol_table);
        free_images(&instruction_image, &data_image);
        free_extern_list(&extern_list);
        free_macro_table(macro_table);
        
        /* שחרור הזיכרון הדינמי של שם הקובץ! */
        free(base_filename);
        
        printf("Finished processing '%s'.\n", full_filename);
    }

    return 0;
}
