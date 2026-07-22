MAIN: add r3, LIST
mcro m1
cmp r3, #-6
bne END
endmcro
LOOP: prn #48
m1
inc r6
END: stop