; ============================================================================
; 2026 Assembler - Macro Declaration Edge Case Test
; File: macro_decl_test.as
;
; This file tests all edge cases for:
; 1. ERROR_CODE_9: Macro declared without a name (mcro keyword only).
; 2. ERROR_CODE_10: Extra text after macro name declaration.
; It also checks valid formats to ensure there are no false positives.
; ============================================================================

; ----------------------------------------------------------------------------
; CATEGORY 1: Valid Macro Declarations (Should NOT trigger any errors)
; ----------------------------------------------------------------------------

; Test 1.1: Standard valid macro
mcro VALID_MAC_1
    add $1, $2, $3
mcroend

; Test 1.2: Valid macro with leading spaces and tabs
    mcro   VALID_MAC_2    
    sub $4, $5, $6
mcroend

; Test 1.3: Valid macro with trailing 
mcro VALID_MAC_3         
    move $7, $8
mcroend

; Test 1.4: Macro keywords inside comments (Should be completely ignored)
; mcro IGNORED_MAC
; mcroend

; Test 1.5: Macro keyword inside string
STR: .asciz "mcro is not a macro definition here"

; ----------------------------------------------------------------------------
; CATEGORY 2: Macro without a Name (Should trigger ERROR_CODE_9)
; ----------------------------------------------------------------------------

; Test 2.1: 'mcro' keyword alone on a line
mcro
    add $1, $1, $1
mcroend

; Test 2.2: 'mcro' followed only by spaces and tabs
mcro        
    sub $2, $2, $2
mcroend

; ----------------------------------------------------------------------------
; CATEGORY 3: Extra Text after Macro Name (Should trigger ERROR_CODE_10)
; ----------------------------------------------------------------------------

; Test 3.1: Macro name followed by extra word
mcro MAC_WITH_EXTRA garbage
    or $3, $4, $5
mcroend

; Test 3.2: Macro name followed by extra punctuation
mcro MAC_WITH_PUNCT,
    and $6, $7, $8
mcroend

; Test 3.3: Macro name followed by multiple extra arguments
mcro MAC_WITH_MULTIPLE arg1 arg2
    nor $9, $10, $11
mcroend
