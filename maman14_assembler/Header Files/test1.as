; File: test1.as
.entry NEXT
.extern val1
mcro MAC_GEN
 la val1
 jmp NEXT
mcroend

MAIN: add $3,$5,$9
LOOP: ori $9,-5,$2
 MAC_GEN
NEXT: move $20,$4
 bgt $4,$2,END
 la K
 sw $0,4,$10
 bne $31,$9,LOOP
 call val1
 jmp $4
END: hlt
STR: .asciz "aBcd"
LIST: .db 6,-9
 .dh 27056
.entry K
K: .dw 31,-12
