#include <stdio.h>
#include <stdarg.h>
#include "error.h"

/* initialize global error flag */
boolean error_found = FALSE;

void report_error(const char *filename, int line_number, const char *format, ...) {
    va_list args;
    
    /* set the flag so we know an error happened */
    error_found = TRUE;
    
    /* print where the error is */
    fprintf(stderr, "Error in %s at line %d: ", filename, line_number);
    
    /* print the actual error message */
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
}
