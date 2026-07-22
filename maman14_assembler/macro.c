#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "macro.h"

boolean process_macros(char *base_filename) {
    FILE *file_as;
    FILE *file_am;
    char filename_as[MAX_LINE_LENGTH];
    char filename_am[MAX_LINE_LENGTH];
    char line[MAX_LINE_LENGTH];
    
    /* כאן תבוא הלוגיקה */
    /* 1. בניית שמות הקבצים המלאים עם הסיומות */
    sprintf(filename_as, "%s.as", base_filename);
    sprintf(filename_am, "%s.am", base_filename);
    
    /* 2. פתיחת קובץ המקור לקריאה וקובץ הפלט לכתיבה */
    file_as = fopen(filename_as, "r");
    if (file_as == NULL) {
        printf("Error: Could not open file %s\n", filename_as);
        return FALSE;
    }
    
    file_am = fopen(filename_am, "w");
    if (file_am == NULL) {
        printf("Error: Could not create file %s\n", filename_am);
        fclose(file_as); /* נסגור את הקובץ הקודם כדי לא להשאיר פתוח */
        return FALSE;
    }
    
    /* כאן תבော בהמשך הלולאה שקוראת את השורות */
    
    /* 3. סגירת הקבצים בסיום */
    fclose(file_as);
    fclose(file_am);
    return TRUE;
}