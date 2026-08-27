; Test 4: Pre-assembler and syntax errors

; Line too long (over 80 chars):
; AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

; Macro definition errors:
mcro add
    hlt
mcroend

mcro GOOD
    hlt
mcroend

mcro GOOD
    hlt
mcroend

mcro ANOTHER extra
    hlt
mcroend

; Comma and syntax errors:
MAIN: or  ,$1, $2, $3
      and $1,, $3
      mv $1, $2
      hlt
