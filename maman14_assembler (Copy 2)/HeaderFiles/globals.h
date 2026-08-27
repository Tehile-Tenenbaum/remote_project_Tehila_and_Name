#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* max sizes and constants */
#define MAX_DATA_SIZE 1000
#define MAX_CODE_SIZE 1000
#define IC_INIT_VALUE 100
#define MAX_LINE_LENGTH 82  /* 80 chars + newline + null terminator */
#define MAX_LABEL_LENGTH 32 /* 31 chars + null terminator */

/* boolean type definition */
typedef enum { FALSE, TRUE } boolean;

#endif /* GLOBALS_H */
