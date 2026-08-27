; =====================================================================
; errors.as
; =====================================================================

; Macro with reserved name
mcro add
    move $1, $2
mcroend

; Extraneous text after macro definition and end
mcro VALID_NAME extra_text
    jmp $5
mcroend extra_text

; Label starts with a digit
1BAD_LABEL: add $1, $2, $3

; Label exceeds 31 characters
VERYLONGLABELNAMETHATDEFINITELYEXCEEDSMAXLENGTH: hlt

; Reserved word as a label
hlt: jmp $5
sub: .db 5, 6

; Space before colon
BADCOLON : mvhi $5, $6

; Missing comma
add $1 $2, $3

; Double comma
sub $1,, $2

; Leading comma
, and $1, $2, $3

; Trailing comma
or $1, $2, $3,

; Missing operand
mvlo $4

; Invalid register
addi $32, 5, $1
addi $-1, 5, $1

; Immediate value out of bounds
addi $1, 40000, $2
subi $1, -32769, $2

; Invalid J-Type syntax
jmp $1, $2
la $5

; Data Directives Out of bounds
.db 128, -129
.dh 32768, -32769

; Missing closing quote in string
.asciz "Unclosed string

.extern EXTERNALVAR
.extern EXTERNALLBL

; Conditional branch to an external label (ILLEGAL)
beq $1, $2, EXTERNALLBL

; Cannot be both extern and entry
.entry EXTERNALVAR

; Using undefined label
jmp NOWHERETOBEFOUND

; Double definition of a label
DUPLBL: add $1, $2, $3
DUPLBL: hlt

; Extraneous text after hlt
hlt extra_text
