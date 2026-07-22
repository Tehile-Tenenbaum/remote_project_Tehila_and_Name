; קובץ בדיקה מורכב למקרואים

MAIN:   mov r1, r3
        
mcro   macro_one   
  add r2, r4
  prn #10
endmcro

        ; כאן יש שורות ריקות והרבה רווחים
        
mcro macro_two
  sub r5, r6
  macro_one
  jsr LAB1
endmcro

        lea STR, r6
        macro_one
        
        ; קריאה למקרו שקורא למקרו אחר
        macro_two
        
LAB1:   stop