; ============================================================================
; 2026 Assembler - Macro Redefinition & Case Sensitivity Edge Case Test
; File: macro_redef_test.as
;
; This file tests all edge cases for ERROR_CODE_13 (Macro Redefinition).
; It ensures that duplicate macros are blocked, while valid distinct or
; case-sensitive macros are allowed.
; ============================================================================

; ----------------------------------------------------------------------------
; TEST 1: Standard Consecutive Redefinition
; ----------------------------------------------------------------------------
mcro DUPMAC
    add $1, $2, $3
mcroend

; Attempting to redefine DUPMAC immediately (Should trigger ERROR_CODE_13)
mcro DUPMAC
    sub $4, $5, $6
mcroend

; ----------------------------------------------------------------------------
; TEST 2: Non-Consecutive Redefinition
; ----------------------------------------------------------------------------
mcro OTHERMAC
    move $10, $11
mcroend

mcro SECDUPMAC
    or $7, $8, $9
mcroend

; Redefining SECDUPMAC after another macro (Should trigger ERROR_CODE_13)
mcro SECDUPMAC
    and $12, $13, $14
mcroend

; ----------------------------------------------------------------------------
; TEST 3: Case Sensitivity Check (Valid Case - Should NOT trigger an error)
; In assembly/C, identifiers are case-sensitive.
; ----------------------------------------------------------------------------
mcro MYMAC
    nor $15, $16, $17
mcroend

mcro mymac
    ori $18, $19, 20
mcroend

; ----------------------------------------------------------------------------
; TEST 4: Substring/Prefix Redefinition (Valid Case - Should NOT trigger error)
; Macro names share prefixes but are distinct.
; ----------------------------------------------------------------------------
mcro TEST
    hlt
mcroend

mcro TESTMORE
    add $1, $1, $1
mcroend

; ----------------------------------------------------------------------------
; TEST 5: Macro name used inside comments/strings (Valid Case - No error)
; ----------------------------------------------------------------------------
; This comment mentions DUPMAC but is not a redefinition.
STR: .asciz "This string contains DUPMAC but is safe"
