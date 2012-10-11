ADD     R1,R0,R0; initialize R1 to zero
LI      R2,8         ;save the increment value into R2
LI      R3,24       ;save the index at which the loop ends
                     ; use label as the offset value
loop: L.D F0,0(R1); load the first operand into F0
L.D F1,800(R1)       ; load the second operand into F1
ADD.D F2,F0,F1       ; F2 = F0 + F1
S.D F2,1600(R1)      ; store the result
ADD R1,R1,R2         ; R1 = R1 + 8
BNEQ R1,R3,loop      ; termination condition