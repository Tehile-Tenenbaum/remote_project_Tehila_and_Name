;file test2.as  
;sample corret source code 

; Empty macro 
mcro empty_mcro
mcroend 
mcro mcro_sub
    add $1, $2, $3
mcroend
.entry NEXT 
.extern wNumber 

.asciz "Edge Case!" 
STR: .asciz "aBcd"
empty_mcro
MAIN: add $3,$5,$9 
mcro_sub

LOOP: ori $9,-5,$2 
 la val1 
 jmp NEXT 

NEXT: move $20,$4 
LIST: .db 6,-9 
 bgt $4,$2,END 
 la K 
 sw $0,4,$10 
 bne $31,$9, LOOP 
 call val1 
 jmp $4 
 la wNumber 
.extern val1  
 .dh 27056 
K: .dw 31,-12 
END: hlt         
.entry K
