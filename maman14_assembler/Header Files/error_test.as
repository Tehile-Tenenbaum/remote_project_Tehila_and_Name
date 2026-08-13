; --- Error test file to challenge the assembler ---

; 1. Using a label that is never defined
.entry NEVER_DEFINED

; 2. Using extern and entry on the same label (illegal in most syllabuses)
.extern SHARED_LBL
.entry SHARED_LBL

mcro broken_macro
    call UNDEFINED_FUNC
mcroend

; 3. Invalid label names (starts with a number, too long, invalid characters)
1BADLABEL: jmp LOOP
TOOLONGTOOLONGTOOLONGTOOLONGTOOLONG: la EXT_VAR
BAD@LABEL: call FUNC

; 4. Duplicate labels
DUP_LBL: .db 10
DUP_LBL: .db 20

; 5. Missing operands
    jmp 
    .db 
    .asciz 

; 6. Invalid data formats (missing quotes, consecutive commas)
    .asciz Hello_without_quotes
    .db 10, , 20
    .dh 10, 20, 

; 7. Calling an undefined macro
    fake_macro

; 8. Calling a macro before it's defined (if your pre-assembler doesn't support this)
    late_macro

mcro late_macro
    la DUP_LBL
mcroend
