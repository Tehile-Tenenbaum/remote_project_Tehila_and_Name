;file test2.as  
;sample corret source code

.extern EXTVAR
MAIN: add $3,$5,$9 
mcro GEN_MAC  
  la val1 
 jmp NEXT 
  mcroend  
LOOP: ori $9,-5,$2 
  GEN_MAC

mcro INIT
    la EXTVAR
mcroend

NEXT: move $20,$4 
 bgt $4,$2,END 
 la K 
 sw $0,4,$10 
 bne $31,$9,LOOP 
 call val1 
 jmp $4

INIT

END: hlt 
STR: .asciz "aBcd" 
LIST: .db 6,-9 
 .dh 27056 
.entry K 
K: .dw 31,-12 
; Data edge limits:
MAXB: .db 127, -128
MAXH: .dh 32767, -32768
MAXW: .dw 2147483647, -2147483648
.extern val1
