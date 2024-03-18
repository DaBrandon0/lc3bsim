.ORIG x3100
AND R0, R0, #0      
LEA R7, LABEL4      
LDW R7, R7, #0      
ADD R0, R0, #5                    
JSR labe2          
labe2 STW R0, R7, #-10    

HALT                
LABEL4 .FILL x310C
.END
