; =====================================================================
; valid_edge.as
; =====================================================================

mcro SWAP_REGS
    move $10, $1
    move $1, $2
    move $2, $10
mcroend

mcro EMPTY_MACRO
mcroend

.entry START
.entry FWDLBL
.extern EXTFUNC
.extern EXTVAR

START:      add $1, $2, $3
            addi $15, 32767, $14
            subi $31, -32768, $0
            
            and     $4  ,   $5    ,    $6

            SWAP_REGS
            EMPTY_MACRO

BRANCHTST: blt $1, $2, FWDLBL
            bne $10, $0, START

            jmp EXTFUNC
            call EXTFUNC
            la EXTVAR

            jmp $31

FWDLBL:    nor $8, $9, $10
            
            lb $5, 0, $6
            sb $7, -15, $8
            lh $0, 100, $2
            sh $3, -32768, $4
            lw $1, 2000, $2
            sw $10, 32767, $11

            hlt

STRDATA:   .asciz "Valid string with spaces: ~!@#$%"
ARRDB:     .db 127, -128, 0, +5
ARRDH:     .dh 32767, -32768, 0, +10
ARRDW:     .dw 2147483647, -2147483648, 0, +15
