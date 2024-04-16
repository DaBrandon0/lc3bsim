.ORIG x1200
    ADD R6, R6, #-2
    STW R0, R6, #0
    ADD R6, R6, #-2
    STW R1, R6, #0
    ADD R6, R6, #-2
    STW R2, R6, #0
    ADD R6, R6, #-2
    STW R3, R6, #0


    LEA R0, here
    LEA R3, check
    LDW R3, R3, #0  
    LDW R0, R0, #0
repeat      LDW R1, R0, #0
    AND R1, R1, #-2
    STW R1, R0, #0
    ADD R0, R0, #-2
    AND R2, R0, R3
    BRnp repeat

    LDW R3, R6, #0
    ADD R6, R6, #2
    LDW R2, R6, #0
    ADD R6, R6, #2
    LDW R1, R6, #0
    ADD R6, R6, #2
    LDW R0, R6, #0
    ADD R6, R6, #2
    RTI
here    .FILL x10FE
check   .FILL xEFFF
.END