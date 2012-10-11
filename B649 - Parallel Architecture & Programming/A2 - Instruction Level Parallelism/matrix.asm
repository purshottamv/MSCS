      LI R1,1        ; R1 = 1  This memory location stores the increment value 1 by which the registers need to be incremented
      LI R2,3        ; R2 = 3  This memory location stores the dimension of the matrix. 
      LI R3,8        ; R3 = 8  This memory location stores the size of the floating point value
      LI R4,24       ; R4 = 24 This memory location stores the increment value for the second matrix (8*3) 
                     ;i.e. (floating point size * matrix dimension)
      LI R5,9       ; R5 = 9  This memory location store the Total Number of matrix elements 
                     ;(i.e. 3 * 3) i.e. (matrix dimension * matrix dimension)
      ADD R9,R0,R0   ; Initialize R9 = 0. R9 is used to calculate the Starting index of second matrix
      ADD R13,R0,R0  ; Initialize R13 =0. R13 Contains the element in the resultant matrix
loop1:DIV R8,R13,R2  ; R8 = R13/3. R8 will store only the quotient
      MUL R11,R8,R3  ; R11 Contains the offset value of the first matrix
      MUL R12,R9,R3  ; R12 contains the offset value of the second matrix
      MOVE R0,F3   ; Initialize F3 =0. It contains the intermediate sum of each kth element
      ADD R14,R0,R0    ; To ensure the inner loop is run only 3 times.i.e equivalent to the dimension of the matrix.
loop2:L.D F0,100(R11)  ; The first matrix is stored from the offset 100
      L.D F1,800(R12)  ; The second matrix is stored from the offset 800
      MUL.D F2,F0,F1   ;
      ADD R14,R14,R1   ; R14 is used to ensure the inner loop is run only 3 times.i.e. 
                       ;equivalent to the dimension of the matrix
      ADD.D F3,F3,F2  ;
      ADD R11,R11,R3   ; Increment Matrix 1 by 8
      ADD R12,R12,R4   ; Increment Matrix 2 by 24
      BNEQ R14,R2,loop2 ; Run loop2 3 times.
      ADD R13,R13,R1    ; Increment the element position of the resultant matrix.
      MUL R14,R13,R3    ;
      S.D F3,1600(R14)  ; Store the element of the resultant matrix in the memory
      ADD R9,R9,R1      ; R9 is used to calculate the Starting index of second matrix
      BEQ R9,R2,loop3    ;
loop4:BNEQ R13,R5,loop1    ;
      BEQ R0,R0,end    ;
loop3:ADD R9,R0,R0    ;
      BEQ R0,R0,loop4   ;
end: NOOP   ;
