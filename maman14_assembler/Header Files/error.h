#ifndef ERROR_H
#define ERROR_H

#include "globals.h" /* Assuming boolean and TRUE/FALSE are defined here */

/* Global flag to indicate if any error was found during the assembly process */
extern boolean error_found;

/* 
 * Function: report_error
 * ----------------------
 * Prints a formatted error message to stderr and sets error_found to TRUE.
 * Uses variable arguments exactly like printf.
 */
void report_error(const char *filename, int line_number, const char *format, ...);

#endif
