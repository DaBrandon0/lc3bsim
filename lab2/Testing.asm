.ORIG x3000 
    AND R0, R0, #0
    AND R1, R1, #0
    LEA R7, testspot
    LDW R7, R7, #0
    ADD R0, R0, #10
    ADD R1, R0, x-000E
    AND R0, R0, #-10
    AND R1, R1, x-0005
    LSHF R0, R0, #4
    RSHFL R0, R0, #2
    RSHFA R0, R0, #1
    XOR R0, R0, #6
    XOR R1, R1, #15
    NOT R1, R1
    STB R0, R7, #0
    STB R1, R7, #1
    STW R0, R7, #1
    STW R1, R7, #2
    LDW R5, R7, #0
    LDB R6, R7, #1
    HALT
testspot    .Fill x5000
.END