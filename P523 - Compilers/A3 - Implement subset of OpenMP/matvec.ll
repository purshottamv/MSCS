define double* @matvec(double** %A, double* %B, double* %C, i32 %n)
{
	%matrix = alloca double**	; allocate memory for double matrix
	%vectorb = alloca double*	; allocate memory for vector B
	%vectorc = alloca double*	; allocate memory for vector C
	%numOfElements = alloca i32	; allocate memory for n
	%i = alloca i32			; allocate memory for i and j
	%j = alloca i32		

	store double** %A, double*** %matrix	; storing matrix into stack variable
	store double* %B, double** %vectorb	; storing vector b into stack
	store double* %C, double** %vectorc	; storing vector c into stack
	store i32 %n,i32* %numOfElements	; storing number of elements into stack
	store i32 0,i32* %i			; storing initial value 0 into i
	br label %1

; <label>:1
	%var1 = load i32* %i
	%var2 = load i32* %numOfElements
	%result1 = icmp slt i32 %var1, %var2	; comparing if i < n
	br i1 %result1, label %2, label %7

; <label>:2
	%i2 = load i32* %i
	%var4 = load double** %vectorc
	%Cindex = getelementptr inbounds double* %var4, i32 %i2
	store double 0.0, double* %Cindex
	store i32 0, i32* %j			; initialize j to 0
	br label %3

; <label>:3
	%jval = load i32* %j			; load the value of j
	%var5 = load i32* %numOfElements
	%result2 = icmp slt i32 %jval,%var5
	br i1 %result2, label %4, label %6

; <label>:4
	%ival = load i32* %i
	%vecc = load double** %vectorc
	%var7 = getelementptr inbounds double* %vecc, i32 %ival	
	%var8 = load double* %var7			; This is C[i]

	%var9 = load i32* %j
	%var10 = load double*** %matrix
	%var11 = getelementptr inbounds double** %var10, i32 %ival	; Get the row number of matrix
	%var12 = load double** %var11
	%var13 = getelementptr inbounds double* %var12, i32 %var9	
	%var14 = load double* %var13			; This is A[i][j]

	%var15 = load double** %vectorb
	%var16 = getelementptr inbounds double* %var15, i32 %var9
	%var17 = load double* %var16			; This is B[j]

	%mulresult = fmul double %var14, %var17		; A[i][j] * B[j]
	%addresult = fadd double %var8, %mulresult	; C[i] + (A[i][j] * B[j])
	store double %addresult, double* %var7		; Store the result in C[i]
	br label %5

; <label>:5
	%var19 = load i32* %j
	%var18 = add i32 %var19, 1
	store i32 %var18, i32* %j			; increment j
	br label %3

; <label>:6
	%var20 = load i32* %i
	%var21 = add i32 %var20, 1
	store i32 %var21, i32* %i			; increment i
	br label %1

; <label>:7
	%var23 = load double** %vectorc		; return C
	ret double* %var23
}