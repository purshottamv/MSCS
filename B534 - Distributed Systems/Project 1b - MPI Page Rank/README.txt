********* ReadMe file for Project 1 Part 2 B534 Course *********
The format for running the program is :

	mpirun -np 10 ./mpi_main -i pagerank.input -t 0.001 -n 10 -o pagerank.output

Below are the command line options that are accepted by the program:

-d : debug mode
-n : Number of iterations
-t : Threshold Value
-i : Input File name containing the adjacency matrix
-o : Output Timings (default = yes)
-h : Help

Please note that -h displays the help and will not execute the program. Remove -h to execute the program.

Damping Factor is constant and is specified as 0.85 in the program
If not specified the default threshold value take in 0.001
If not specified the default number of iterations is equal to 10
If not specified by default the timings are displayed.

The program creates two files:
1. pagerank.output - This contains top 10 Urls.
2. pagerank.output.all - This contains pagerank of all the pages.