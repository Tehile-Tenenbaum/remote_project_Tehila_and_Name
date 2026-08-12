#include <stdio.h>
#include "globals.h"
#include "macro.h"

int main() {
    char *test_filename = "complex_test";

    printf("Starting macro processing for: %s.as\n", test_filename);

    if (process_macros(test_filename)) {
        printf("Macro processing completed successfully! Check %s.am\n", test_filename);
    } else {
        printf("Error occurred during macro processing.\n");
    }

    return 0;
}