#!/bin/bash
#PBS -q b534
#PBS -V
# #PBS -l  nodes=i97:ppn=8+i98:ppn=8
#PBS -l  nodes=i99:ppn=8+i100:ppn=8
#PBS -l walltime=01:00:00
#PBS -o run_pagerank_bm$PBS_JOBID.out

export JAVA_HOME=/N/u/taklwu/jdk1.6.0_31
export PATH=$JAVA_HOME/bin:$MPJ_HOME/bin:$PATH
export CLASSPATH=$CLASSPATH:$JAVA_HOME/lib

. /usr/local/bin/shutdown_vms
sleep 60
. /usr/local/bin/start_vms

echo "Waiting for the VM's to be reachable"
. /usr/local/bin/wait_for_vms $VM_NODEFILE
cat $VM_NODEFILE > nodes_vm
echo "VMs now reachable"

FILENAME="nodes_vm"
export NODES=$VM_NODEFILE
cat $NODES >> vm.out
HEADNODE="192.168.123.197"
count=0

while read LINE
do
        if test $count -eq 0
        then
			HEADNODE=$LINE	
		fi
        ssh $LINE "./startDeamon.sh &" &
        count=`expr $count + 1`
done < $FILENAME
sleep 10
cd $HOME
ssh $HEADNODE "module load openmpi;mpirun --mca btl_tcp_if_exclude lo,eth1 -hostfile nodes_vm -np 6 mpi_main -i pagerank.input.2M -n 500 -t 0.000001 >> mpi.out"
count=0
while read LINE
do
	ssh $LINE "killall -e java -u pvishwak &" &
	count=`expr $count + 1`
done < $FILENAME

echo "shutting down VM's"
. /usr/local/bin/shutdown_vms
echo "VM Job finished"