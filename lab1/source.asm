;ewfwefnwoifw
.ORIG x3000
    ;comments
		ADD R1, R2, R3;wefoiwenfwoefn
		ADD R4, R5, #-3
		ADD R4, R5, #12
		ADD R4, R5, x0004
		ADD R4, R5, x-0004
		AND R6, R7, R0
		AND R1, R2, x0001
		AND R1, R2, x-0001
		BRn b1
        ;wefnweoifwnf
b1		BRz b2
b2		BRp b3
b3		BR b4
b4		BRzp b5
b5		BRnp b6
b6		BRnz b7
b7		BRnzp b8
b8	    JMP R0
		RET; comments
		JSR b9
b9		JSRR R5
		LDB R3, R6, #6
		LDB R3, R6, x005
		LDB R3, R6, #-6
		LDB R3, R6, x-05
		LDW R0, R1, #-7
		LDW R2, R1, #7
		LDW R5, R7, x7
		LDW R0, R1, x-0007
load	LEA R7, load
		NOT R0, R1
		LSHF R1, R0, #3
		RSHFL R3, R4, #10
		RSHFA R5, R6, #2
		STB R1, R2, x0001
		STW R4, R5, x0006
		XOR R1, R2, R3
		XOR R1, R2, #5
        AND R0, R0, #0
		TRAP x25
		TRAP x00
		TRAP x0001
		TRAP x05
;comments
	RET ;comments
    HALT ;comments
    NOP;comments
	.FILL #-32768;comments
	;comments
	.FILL x8000
;comments	.FILL xFFFF
	.FILL #-1
	.FILL #32767
	.FILL xFF
	.FILL x00
	.FILL x-7F0
	.END
















