.ORIG x3100
AND R0, R0, #0
LEA R7, LABEL2
LDW R7, R7, #0
LDW R1, R7, #0
LSHF R5, R1, #4
NOT R1, R1
LSHF R2, R1, #4
RSHFL R3, R1, #4
RSHFA R4, R1, #4
LEA R7, LABEL3
LDW R7, R7, #0
STW R1, R7, #0
STW R2, R7, #1
STW R3, R7, #2
STW R4, R7, #3
STB R5, R7, #4
HALT
LABEL2 .FILL x0FF0
LABEL3 .FILL x4100
.END
