.ORIG x3000
    LEA R0, here
    LDW R0, R0, #0
    AND R1, R1, #0
    ADD R1, R1, #1
    STW R1, R0, #0
    AND R3, R3, #0
    ADD R3, R3, #10
    ADD R3, R3, #10
    AND R4, R4, #0
    LEA R0, adds 
    LDW R0, R0, #0
repeat    LDB R2, R0, #0
    ADD R4, R2, R4
    ADD R0, R0, #1
    ADD R3, R3, #-1
    Brp repeat
    LEA R0, store
    LDW R0, R0, #0
    STW R4, R0, #0
    HALT
here    .Fill x4000
adds    .Fill xC000
store   .Fill x0000
.END