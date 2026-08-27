#include <stdio.h>
#include <stdarg.h>
#include "error.h"

/* Global error flag initialization */
boolean error_found = FALSE;

void report_error(const char *filename, int line_number, const char *format, ...) {
    va_list args;
    
    /* Set the global error flag automatically */
    error_found = TRUE;
    
    /* Print the standard error prefix */
    fprintf(stderr, "Error in %s at line %d: ", filename, line_number);
    
    /* Print the specific error message using variable arguments */
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}
