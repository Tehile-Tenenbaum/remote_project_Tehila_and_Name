; bug_db_empty.as
; Checks whether an empty .db directive is reported as an error.

MAIN:   la A
        la B
        hlt
A:      .db
B:      .dw 5
