.ORIG x3000
    ADD R0, R0, #-1
    ADD R1, R1, #1
    ADD R0, R0, R1
    NOT R0, R0
    ADD R1, R0, R0
    LEA R3, label
    STW R1, R3, #4
    STB R1, R3, #0
    HALT
label .FILL x1111
.END