; --- valid1_basic.as ---
; Testing basic R-type instructions, macro expansions, and data directives

mcro DO_MATH
    add $1, $2, $3
    sub $4, $5, $6
    and $7, $8, $9
    or  $10, $11, $12
    nor $13, $14, $15
mcroend

.entry START

START:  DO_MATH
        move $16, $17
        mvhi $18, $19
        mvlo $20, $21
        jmp END_PROG

DATA1:  .db 10, -20, 30, 0, 127, -128
DATA2:  .dh 1000, -1000, 32767, -32768
DATA3:  .dw 100000, -100000, 2147483647

END_PROG: hlt
