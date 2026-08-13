; --- Valid file with extreme edge cases ---

.entry MAIN
.entry LOOP
.extern EXTVAR

mcro valid_macro
    la      EXTVAR
    jmp     LOOP
mcroend

; Lots of spaces and tabs below
MAIN:       call        FUNC
    
    ; Empty lines and comment lines above and below
    
LOOP:       valid_macro

FUNC:       .asciz      "Edge Cases!"
STRDATA:   .db         10,  -20 ,  30   , 40
MOREDATA:  .dh         32767, -32768
LASTDATA:  .dw         100000

; Entry label defined at the very end
.entry FUNC
