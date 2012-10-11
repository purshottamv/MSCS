#!/bin/bash
#PBS -q b534
#PBS -V
#PBS -l  nodes=i97:ppn=8+i98:ppn=8
#PBS -l walltime=01:00:00
#PBS -o run_pagerank_bm$PBS_JOBID.out
export NODES=$PBS_NODEFILE
cat $PBS_NODEFILE | uniq > nodes
FILENAME="nodes"
count=0
while read LINE
do
	if test $count -gt 0
	then
		ssh $LINE "cd $HOME/B534/P2/DeamonCode;./startDeamon.sh &" &
	else
		cd $HOME/B534/P2/DeamonCode;./startDeamon.sh &
	fi
	count=`expr $count + 1`
done < $FILENAME
cd $HOME/B534/P2
module load openmpi
mpirun --mca btl_tcp_if_exclude lo,eth1 -hostfile $PBS_NODEFILE -np 4 mpi_main -i pagerank.input.2M  -n 500  -t 0.000001
count=0
while read LINE
do
        if test $count -gt 0
        then
        	ssh $LINE "killall -e java -u pvishwak &" &
        else
			killall -e java -u pvishwak &   
        fi
        count=`expr $count + 1`
done < $FILENAME
echo "Bare Metal Job Finished"