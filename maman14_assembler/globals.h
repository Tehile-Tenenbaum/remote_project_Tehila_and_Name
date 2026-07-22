#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* קבועים בסיסיים של הפרויקט */
#define MAX_LINE_LENGTH 82  /* 80 תווים + תו ירידת שורה + תו סיום מחרוזת */
#define MAX_LABEL_LENGTH 32 /* 31 תווים מקסימום לשם תווית + תו סיום מחרוזת */

typedef enum { FALSE, TRUE } boolean;

#endif /* GLOBALS_H */