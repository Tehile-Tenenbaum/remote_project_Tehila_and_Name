; Test 3: Labels, data ranges, and symbol table conflicts
; Expected: Fails in pass 1 and pass 2, no output files.

.extern REMOTE_A
.entry  REMOTE_A

; Label errors:
1BAD:   add $1, $2, $3
THISLABELNAMEISDEFINITELYWAYTOOLONGTOBEVALID: hlt

; Register and data edge errors:
MAIN: add $3, $5, $32
      add $1, $05, $2
      
BYTES:  .db 200
HALVES: .dh 40000
MSG:    .asciz "unterminated

; Missing labels for jumps:
      jmp NOWHERE
      hlt
