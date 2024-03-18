.ORIG x3000
    ADD R0, R0, #1
    ADD R0, R0, #-1
    ADD R0, R0, #-1
    AND R0, R0, #1
    AND R0, R0, #0
    AND R7, R7, #0
    NOT R7, R7
    JSR label
    RET
label    LEA R7, cheque
    LDB R0, R7, #0
    LDB R0, R7, #1
    LDB R0, R7, #2
    LDB R0, R7, #3
    LDB R0, R7, #4
    LDB R0, R7, #5
    LDW R0, R7, #0
    LDW R0, R7, #1
    LDW R0, R7, #2
    LDW R0, R7, #0
    STB R0, R7, #7
    STW R0, R7, #5
    HALT
cheque    .FILL xFFFF
    .FILL x0001
    .FILL x0000
    .END
