; =====================================================================
; comment_bug_demonstration.as
; Demonstrating why the old token_exists_in_line implementation breaks!
; =====================================================================

mcro MY_MACRO
    add $1, $2, $3
mcroend

; --------------------------------------------------------------------
; CASE 1: Macro call with a semi-colon but NO preceding whitespace.
;
; In Assembler 2026, a semi-colon is ONLY a valid comment if preceded
; by a whitespace (space or tab), or if it is at the very beginning of a line.
;
; The old preprocessor blindly slices at ';' using strchr, so it thinks 
; "MY_MACRO" is called here and expands it, producing:
;     add $1, $2, $3;this_is_not_a_valid_comment
; Since there is no space before ';', the Assembler First Pass will NOT
; treat this as a comment and will crash on the third operand.
; --------------------------------------------------------------------
MY_MACRO;this_is_not_a_valid_comment

; --------------------------------------------------------------------
; CASE 2: Correct macro call with a proper trailing comment.
; (Should expand correctly in both old and new implementations)
; --------------------------------------------------------------------
MY_MACRO ; This is a valid comment because of the space before the semi-colon
