#! /bin/bash
# Script for running the mpi_main
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.1K0 -n 10 > output
echo Finished np 2 pagerank.input.1K0
date
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
echo Finished np 2 pagerank.input.10K0
date
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
echo Finished np 2 pagerank.input.20K0
date
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
echo Finished np 2 pagerank.input.30K0
date
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
echo Finished np 2 pagerank.input.40K0
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 2 pagerank.input.50K0
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
echo Finished np 2 pagerank.input.60K0
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
echo Finished np 2 pagerank.input.70K0
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
echo Finished np 2 pagerank.input.80K0
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
echo Finished np 2 pagerank.input.90K0
mpirun --hostfile nodes -np 2 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 2
date
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 4 pagerank.input.50K0
date
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 4 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 4
date
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 6 pagerank.input.50K0
date
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 6 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 6
date
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 8 pagerank.input.50K0
date
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 8 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 8
date
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 10 pagerank.input.50K0
date
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 10 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 10
date
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 12 pagerank.input.50K0
date
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 12 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 12
date
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 14 pagerank.input.50K0
date
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 14 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
echo Finished np 14
date
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.10K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.20K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.30K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.40K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.50K0 -n 10 >> output
echo Finished np 16 pagerank.input.50K0
date
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.60K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.70K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.80K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.90K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.100K0 -n 10 >> output
mpirun --hostfile nodes -np 16 ./mpi_main -i pagerank.input.1K0 -n 10 >> output
echo Finished np 16
date
