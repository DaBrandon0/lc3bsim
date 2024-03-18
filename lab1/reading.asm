.ORIG x0000
		AND R2, R2, #0
		ADD R3, R2, #15
rep		ADD R3, R2, #1
		BRn rep
		ADD R1, R2, R3
		ADD R4, R5, #-3
		AND R6, R7, R0
		AND R1, R2, x0001
		BRn b1
b1		BRz b2
b2		BRp b3
b3		BR b4
b4		BRzp b5
b5		BRnp b6
b6		BRnz b7
b7		BRnzp b8
b8	    LEA R0, a1
        LEA R7, a2
		LEA R5, q1
        JMP R0
a1		RET; comments
a2		JSR b9 
b9		JSRR R5
q1		LDB R3, R0, #6
		LDW R0, R5, #-1
load	LEA R7, load
		NOT R0, R1
		LSHF R1, R0, #3
		RSHFL R3, R4, #10
		RSHFA R6, R3, #2
		STB R1, R2, x0001
		STW R4, R5, x0006
		XOR R1, R2, R3
		XOR R1, R2, #5
        AND R0, R0, #0
		NOP;comments
		LEA R0, big
		LEA R1, rep
		LDW R0, R0, #0
		STW R0, R1, #3
		TRAP x25
        HALT ;comments
big        .FILL xFFFF
	.END