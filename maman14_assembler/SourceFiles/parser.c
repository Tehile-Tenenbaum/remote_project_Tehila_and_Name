#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "globals.h"
/*
 * Function: is_data_directive
 * ---------------------------
 * Checks if a given word is a data directive.
 * Supported directives are: .db, .dw, .dh, .asciz.
 * 
 * @param word: The string to check.
 * @return: TRUE if it is a data directive, FALSE otherwise.
 */
boolean is_data_directive(char *word) {
    if (strcmp(word, ".db") == 0 || strcmp(word, ".dw") == 0 ||
        strcmp(word, ".dh") == 0 || strcmp(word, ".asciz") == 0) {
        return TRUE;
    }
    return FALSE;
}
/*
 * Function: is_entry_directive
 * ----------------------------
 * Checks if a given word is an .entry directive.
 * 
 * @param word: The string to check.
 * @return: TRUE if it is an .entry directive, FALSE otherwise.
 */
boolean is_entry_directive(char *word) {
    if (strcmp(word, ".entry") == 0) {
        return TRUE;
    }
    return FALSE;
}
/*
 * Function: is_extern_directive
 * -----------------------------
 * Checks if a given word is an .extern directive.
 * 
 * @param word: The string to check.
 * @return: TRUE if it is an .extern directive, FALSE otherwise.
 */
boolean is_extern_directive(char *word) {
    if (strcmp(word, ".extern") == 0) {
        return TRUE;
    }
    return FALSE;
}

/* הנחה: יש לנו פונקציה שמכניסה את הערך בפועל למערך תמונת הנתונים */
extern void add_to_data_image(long value, int size_in_bytes, int *DC);

/*
 * Function: process_data_directive
 * --------------------------------
 * Parses the operands of a data directive (.db, .dh, .dw, .asciz)
 * and encodes them into the data image while updating the Data Counter (DC).
 * 
 * @param line: The full string of the current line.
 * @param index: Pointer to the current index in the line (right after the directive).
 * @param DC: Pointer to the Data Counter.
 * @param directive: The directive name (e.g., ".db").
 * @return: TRUE if parsing succeeded without syntax errors, FALSE otherwise.
 */
boolean process_data_directive(char *line, int *index, int *DC, char *directive) {
    int size_in_bytes = 1; /* Default size for .db */
    
    /* 1. Skip spaces after the directive */
    skip_white_spaces(line, index);

    /* 2. Handle .asciz (String directive) */
    if (strcmp(directive, ".asciz") == 0) {
        if (line[*index] != '"') {
            return FALSE; /* Error: String must start with double quotes */
        }
        (*index)++; /* Skip the opening quote */
        
        /* Read characters until the closing quote or end of line */
        while (line[*index] != '"' && line[*index] != '\0' && line[*index] != '\n') {
            add_to_data_image((long)line[*index], 1, DC);
            (*index)++;
        }
        
        if (line[*index] != '"') {
            return FALSE; /* Error: Missing closing quote */
        }
        (*index)++; /* Skip the closing quote */
        
        /* Add the null terminator '\0' at the end of the string */
        add_to_data_image(0, 1, DC); 
        return TRUE;
    }

    /* 3. Handle numeric directives (.db, .dh, .dw) */
    if (strcmp(directive, ".dh") == 0) {
        size_in_bytes = 2; /* Half-word */
    } else if (strcmp(directive, ".dw") == 0) {
        size_in_bytes = 4; /* Word */
    }

    /* Loop to read all comma-separated numbers */
    while (line[*index] != '\0' && line[*index] != '\n') {
        char *end_ptr;
        long value;
        
        skip_white_spaces(line, index);
        
        /* 4. Parse the number using strtol */
        value = strtol(&line[*index], &end_ptr, 10);
        
        /* Check if a number was actually read */
        if (&line[*index] == end_ptr) {
            return FALSE; /* Error: Expected a number */
        }
        
        /* Update the index to where strtol stopped */
        *index = end_ptr - line; 
        
        /* 5. Add the parsed number to the data image */
        add_to_data_image(value, size_in_bytes, DC);
        
        /* 6. Look for a comma or end of line */
        skip_white_spaces(line, index);
        
        if (line[*index] == ',') {
            (*index)++; /* Skip the comma and continue to the next number */
            skip_white_spaces(line, index);
            /* Check for illegal trailing comma */
            if (line[*index] == '\0' || line[*index] == '\n') {
                return FALSE; /* Error: Line ends with a comma */
            }
        } else if (line[*index] != '\0' && line[*index] != '\n') {
            return FALSE; /* Error: Missing comma between numbers */
        }
    }
    
    return TRUE;
}
/* 
 * Function: add_to_data_image
 * ---------------------------
 * Inserts a parsed numeric value or character into the data image array.
 * Updates the Data Counter (DC) accordingly.
 */
void add_to_data_image(long value, int size_in_bytes, int *DC) {
    /* נניח שמערך תמונת הנתונים הוגדר בקובץ globals.h בתור: */
    /* extern char data_image[MAX_DATA_SIZE]; */
    
    int i;
    
    /* 1. אנו עוברים על מספר הבתים שהנתון הזה תופס (1, 2 או 4) */
    for (i = 0; i < size_in_bytes; i++) {
        
        /* 2. חילוץ הבית הנוכחי מתוך המספר המלא ושמירתו במערך */
        /* אנו מבצעים הזזה ימינה (Bitwise Right Shift) וחותכים רק 8 סיביות (בית אחד) */
        data_image[*DC] = (value >> (i * 8)) & 0xFF;
        
        /* 3. קידום מונה הנתונים לבית הבא */
        (*DC)++; 
    }
}
