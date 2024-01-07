#!/bin/bash -l

# Specific course queue and max wallclock time
#SBATCH -p course -t 10

# Defaults on Barkla (but set to be safe)
## Specify the current working directory as the location for executables/files
#SBATCH -D ./
## Export the current environment to the compute node
#SBATCH --export=ALL

## SLURM terms
## nodes            relates to number of nodes
## ntasks-per-node  relates to MPI processes per node
## cpus-per-task    relates to OpenMP threads (per MPI process)

echo "Node list                    : $SLURM_JOB_NODELIST"
echo "Number of nodes allocated    : $SLURM_JOB_NUM_NODES or $SLURM_NNODES"
echo "Number of threads or processes          : $SLURM_NTASKS"
echo "Number of processes per node : $SLURM_TASKS_PER_NODE"
echo "Requested tasks per node     : $SLURM_NTASKS_PER_NODE"
echo "Requested CPUs per task      : $SLURM_CPUS_PER_TASK"
echo "Scheduling priority          : $SLURM_PRIO_PROCESS"

# parallel using OpenMP
EXE=$1
COORD_FILE_NAME=$2
OUTPUT_FILE_NAME_c=$3
OUTPUT_FILE_NAME_f=$4
OUTPUT_FILE_NAME_n=$5

echo
echo ------------------------------------

#this tests if the file $EXE is present and executable
if test -x $EXE; then
      # set number of threads

      # if '-c' not used then default to 1. SLURM_CPUS_PER_TASK is given by -c
      export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}
      echo using ${OMP_NUM_THREADS} OpenMP threads
      echo
      echo
      echo Multiple execution..
      echo
      echo

      # run multiple times. Because we have exported how many threads we're using, we just execute the file.
      # for i in {1..5}; do ./${EXE}; done
      for i in {1..3}; do ./${EXE} $COORD_FILE_NAME $OUTPUT_FILE_NAME_c $OUTPUT_FILE_NAME_f $OUTPUT_FILE_NAME_n; done
      # ./${EXE} $COORD_FILE_NAME $OUTPUT_FILE_NAME_c $OUTPUT_FILE_NAME_f $OUTPUT_FILE_NAME_n
else
      echo $SRC did not built to $EXE
fi