; =============================================================================
; 2026 Assembler - Comprehensive Macro Names Test Suite
; File: macro_names_test.as
;
; This file is designed specifically to test the macro name validation of the
; 2026 pre-processor. It attempts to define macros using all forbidden names.
;
; According to the 2026 spec, a macro name CANNOT be:
; 1. Any of the 27 opcodes (e.g., add, sub, move, jmp, hlt, etc.)
; 2. Any of the 32 register names ($0 to $31)
; 3. Any of the 6 data directives (.db, .dw, .dh, .asciz, .entry, .extern)
;
; Running this file must fail at the pre-processor stage, printing detailed
; "Illegal name for a macro" (ERROR_CODE_17) errors for each case.
; =============================================================================

; -----------------------------------------------------------------------------
; CATEGORY 1: Forbidden Opcode Names (R-type, I-type, J-type opcodes)
; -----------------------------------------------------------------------------

; Test 1.1: R-type opcode 'add'
mcro add
    add $1, $2, $3
mcroend

; Test 1.2: R-type opcode 'move'
mcro move
    move $4, $5
mcroend

; Test 1.3: I-type opcode 'addi'
mcro addi
    addi $6, 10, $7
mcroend

; Test 1.4: I-type opcode 'beq'
mcro beq
    beq $8, $9, LABEL
mcroend

; Test 1.5: J-type opcode 'jmp'
mcro jmp
    jmp $10
mcroend

; Test 1.6: J-type opcode 'hlt'
mcro hlt
    hlt
mcroend

; -----------------------------------------------------------------------------
; CATEGORY 2: Forbidden Register Names ($0 to $31)
; -----------------------------------------------------------------------------

; Test 2.1: Register '$0'
mcro $0
    add $0, $0, $0
mcroend

; Test 2.2: Register '$15'
mcro $15
    move $15, $16
mcroend

; Test 2.3: Register '$31'
mcro $31
    jmp $31
mcroend

; -----------------------------------------------------------------------------
; CATEGORY 3: Forbidden Data Directives (.db, .dw, .dh, .asciz, etc.)
; -----------------------------------------------------------------------------

; Test 3.1: Directive '.db'
mcro .db
    .db 1, 2, 3
mcroend

; Test 3.2: Directive '.dw'
mcro .dw
    .dw 1000, 2000
mcroend

; Test 3.3: Directive '.asciz'
mcro .asciz
    .asciz "test"
mcroend

; Test 3.4: Directive '.entry'
mcro .entry
    .entry LABEL
mcroend

; Test 3.5: Directive '.extern'
mcro .extern
    .extern EXT_LABEL
mcroend
