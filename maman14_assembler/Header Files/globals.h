#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_DATA_SIZE 1000
#define MAX_CODE_SIZE 1000
#define IC_INIT_VALUE 100

/* הצהרה גלובלית על מערכי הזיכרון */
extern char data_image[MAX_DATA_SIZE];
extern unsigned long code_image[MAX_CODE_SIZE];

/* קבועים בסיסיים של הפרויקט */
#define MAX_LINE_LENGTH 82  /* 80 תווים + תו ירידת שורה + תו סיום מחרוזת */
#define MAX_LABEL_LENGTH 32 /* 31 תווים מקסימום לשם תווית + תו סיום מחרוזת */

typedef enum { FALSE, TRUE } boolean;

#endif /* GLOBALS_H */
