#ifndef MACRO_H
#define MACRO_H

#include "globals.h"

/* מבנה המייצג שורת טקסט בתוך מאקרו */
typedef struct macro_line {
    char line[MAX_LINE_LENGTH];
    struct macro_line *next;
} MacroLine;

/* מבנה המייצג מאקרו שלם */
typedef struct macro_node {
    char name[MAX_LABEL_LENGTH];
    MacroLine *content;
    struct macro_node *next;
} MacroNode;

/* הצהרה על הפונקציה הראשית של הקדם-אסמבלר */
boolean process_macros(char *base_filename);

#endif /* MACRO_H */