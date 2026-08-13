#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "symbol_table.h"
#include "pass1.h"

/* מערכי תמונת הזיכרון הפיזיים */
char data_image[MAX_DATA_SIZE];
unsigned long code_image[MAX_CODE_SIZE];

int main(int argc, char *argv[]) {
    SymbolNode *symbol_table = NULL;
    int ICF = 0, DCF = 0;
    boolean success;

    if (argc < 2) {
        printf("Usage: %s <filename_without_extension>\n", argv[0]);
        return 1;
    }

    printf("--- Starting Pass 1 Test for file: %s.am ---\n", argv[1]);

    /* הרצת המעבר הראשון */
    success = execute_pass1(argv[1], &symbol_table, &ICF, &DCF);

    if (success) {
        printf("\n>>> Pass 1 Finished Successfully! <<<\n");
        printf("Final ICF: %d\n", ICF);
        printf("Final DCF: %d\n", DCF);

        /* הדפסת טבלת הסמלים לבדיקה */
        printf("\n--- Symbol Table Output ---\n");
        print_symbol_table(symbol_table); /* פונקציה שמדפיסה את הרשימה המקושרת */
    } else {
        printf("\n>>> Pass 1 Failed with errors. <<<\n");
    }

    /* שחרור זיכרון טבלת הסמלים */
    free_symbol_table(symbol_table);

    return 0;
}
