; =====================================================================
; test_first_pass_errors.as
; Comprehensive test file for the First Pass stage.
; All lines are kept strictly under 80 characters to comply with limits.
; =====================================================================

; Error 1: Label must start with an alphabetical letter
1LABEL: add $1, $2, $3

; Error 2: Label contains invalid characters (only letters and digits)
LAB_EL: add $1, $2, $3

; Error 3: Label declaration must end with a single colon ':' and no text
LABEL1:: add $1, $2, $3

; Error 4: Label exceeds maximum allowed length of 31 characters
VeryLongLabelWithMoreThanThirtyOneCharactersName: add $1, $2, $3

; Error 5: Label name cannot be a reserved keyword (register)
$1: add $1, $2, $3

; Error 6: Label name cannot be a reserved keyword (opcode)
add: add $1, $2, $3

; Error 7: Label name cannot be a reserved keyword (directive)
.db: add $1, $2, $3

; Error 8: Label declaration cannot have spaces before the colon ':'
LABEL2 : add $1, $2, $3

; Error 9: Label defined more than once in this file
LABEL3: add $1, $2, $3
LABEL3: sub $1, $2, $3

; Error 10: Unknown opcode
invalidopcode $1, $2, $3

; Error 11: Unknown or illegal directive
.validdir 1, 2, 3

; Error 12: Register name cannot contain leading zeros
add $01, $2, $3

; Error 13: Register number out of range
add $32, $1, $2

; Error 14: Non-numeric value in data directive
.db 12, 13a, 14

; Error 15: Missing comma between numbers
.db 12 14

; Error 16: Trailing comma in data directive
.db 12, 14,

; Error 17: Missing '"' in .asciz directive
.asciz hello"

; Error 18: Extra text after closing '"'
.asciz "hello" extra_text

; Error 19: Numeric value out of range for byte (.db)
.db 150

; Error 20: Numeric value out of range for half-word (.dh)
.dh 40000

; Error 21: Numeric value out of range for word (.dw)
.dw 3000000000

; Error 22: Immediate value out of range for I-type instruction (16-bit)
addi $1, 40000, $2

; Error 23: Label defined locally cannot also be declared as .extern
LOCAL_LAB: add $1, $2, $3
.extern LOCAL_LAB

; Error 24: Label declared as .extern cannot also be declared as .entry
.extern EXT_LAB
.entry EXT_LAB

; Error 25: Multiple consecutive commas are not allowed
add $1,, $2, $3

; Error 26: Comma in wrong position (leading comma)
, add $1, $2, $3

; Error 27: Comma in wrong position (trailing comma)
add $1, $2, $3,

; Error 28: Missing comma between arguments
add $1 $2, $3

; Error 29: Too many operands
add $1, $2, $3, $4

; Error 30: Missing operand
add $1, $2

; Error 31: Label declaration must be followed by a whitespace
LABEL4:add $1, $2, $3

; Error 32: Label defined without a following instruction or directive
LABEL5:

; Error 33: Line exceeds maximum length 
; 12345678901234567890123456789012
