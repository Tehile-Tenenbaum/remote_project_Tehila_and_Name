#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "globals.h"


/* מבנה עבור צומת בטבלת הסמלים (רשימה מקושרת) */
typedef struct symbol_node {
    char name[MAX_LABEL_LENGTH]; /* שם התווית */
    int address;                 /* הכתובת בזיכרון שבה התווית מוגדרת */
    int is_code;                 /* דגל: 1 אם זה סמל של קוד, 0 אחרת */
    int is_data;                 /* דגל: 1 אם זה סמל של נתונים, 0 אחרת */
    int is_external;             /* דגל: 1 אם הסמל חיצוני, 0 אחרת */
    int is_entry;                /* דגל: 1 אם הסמל הוגדר כ-entry, 0 אחרת */
    struct symbol_node *next;    /* מצביע לאיבר הבא ברשימה */
} SymbolNode;

/* הצהרות על פונקציות לניהול הטבלה (מימוש יכתב בקובץ .c) */
/*
void add_symbol(SymbolNode **head, char *name, int address, int is_code, int is_data, int is_external, int is_entry);
SymbolNode* find_symbol(SymbolNode *head, char *name);
void free_table(SymbolNode *head);
*/

#endif /* SYMBOL_TABLE_H */