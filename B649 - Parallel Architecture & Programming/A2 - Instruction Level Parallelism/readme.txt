Deliverables :

sim_seq.cpp	: Simulator executing the program in a sequential mode .i.e. non-pipelined. It displays step by step execution of the assembly program displaying the cycles consumed by each instruction.                                                                                     
sim_p.cpp	: Simulator executing the program in a pipeline mode i.e. 5 stage pipeline.  Forwarding technique is not used.
sim_t.cpp	: Simulator executing the program using tomasulo dynamic scheduling technique. 
arch_params_file: Parameters file containing the simulator architecture parameters. 
init_mem_file	: Binary file to initialize the memory.  The two matrices are stored from the memory address 100 and 800 respectively. The results are stored starting from memory address 1608. The file size is 1 MB. All the values are 8-byte double.
matrix.asm	: The matrix multiplication program using the given instruction set. This program multiplies 3 X 3 matrices.  Couple of parameter needs to be changed inside this file in order to do matrix multiplication of different order. This program supports multiplication of only square matrices.
Createmem.cpp	: Utility program used to create as well as read the memory dump.  
Makefile	: Builds sim_seq, sim_p, sim_t and createmem executables.

Notes:
1. All the three programs give a detailed step by step information regarding execution of the assembly program. Finally, all of them display the total number of CPU cycles consumed, number of stalls encountered and CPI. 
2. All the three programs take 4 parameters and should be executed in the following format.
	./sim_seq arch_params_file ini_mem_file matrix.asm output_mem_file
	./sim_p arch_params_file ini_mem_file matrix.asm output_mem_file
	./sim_t arch_params_file ini_mem_file matrix.asm output_mem_file
3. createmem program gives you a choice of creating a new memory file to initialize the program or read an existing memory file. The output dump can be read by using this utility.
4. Please make sure the input assembly program does not contain tabs. Instead please use spaces.
5. To get the detailed execution steps and path, the output of any program can be directed to a file and the file can be analyzed later using a text editor .i.e.
              ./sim_t arch_params_file ini_mem_file matrix.asm output_mem_file > output.txt


Statistics:
Below parameter values were used to get the statistics for matrix.asm (3X3).
NOOP    1, ADD     2, ADD.D   2, DIV     5, DIV.D   5, SUB     2, SUB.D   2, MUL     5, MUL.D   5, L       2
LI      2, S       2, L.D     2, S.D     2, BEQ     2, BNEQ    1, BGTZ    1, BLTZ    1, MOVE    2, BEQ.D   2
BNEQ.D  1, BGTZ.D  1, BLTZ.D  1, MOVE.D  2
loads 3,  stores 3, intadds 3, fpadds 3, intmuls 2, fpmuls 2, intdivs 2, fpdivs 2

 				sim_seq	sim_p	sim_t
Total Cycles			811	986	472
Total Instructions Fetched	330	363	330
No. of stalls			-	652	61
CPI				2.46	2.72	1.43

Note 1: Total Instructions Fetched for sim_p is more because the simulator is considering branch untaken before the branch instruction is evaluated in the decode stage.  
Note 2: sim_p doesn’t use forwarding.


Interpreting the Output for tomasulo
1. Below Register you might find somthing like W-4 which means, in this case, value of F3 would be written by Reservation Station # 4. Each Reservation Station is assigned a unique identifier.
2. For legibility, reservation stations that are busy on a particular cycle are shown in the output and not all.
