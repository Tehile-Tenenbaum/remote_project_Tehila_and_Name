#ifndef ERROR_H
#define ERROR_H

#include "globals.h"

/* flag to know if we hit any errors during assembly */
extern boolean error_found;

/*
 * prints an error message with the file name and line number.
 * works just like printf.
 */
void report_error(const char *filename, int line_number, const char *format, ...);

#endif
