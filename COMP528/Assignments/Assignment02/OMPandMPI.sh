#!/bin/bash -l

module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

# Specific course queue and max wallclock time
#SBATCH -p course -t 2

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
COUTPUT_FILE_NAME=$3
FOUTPUT_FILE_NAM=$4
NOUTPUT_FILE_NAME=$5

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

      PARAMS= "$COORD_FILE_NAME $COUTPUT_FILE_NAME $FOUTPUT_FILE_NAM $NOUTPUT_FILE_NAME"
      procs=${SLURM_NTASKS:-1}

      # if EXE end with 'complete', then use mpirun, else use ./${EXE}
      if [[ $EXE == *complete ]]; then
            # echo mpirun -np $procs ./${EXE} $PARAMS
            mpirun -np $procs ./${EXE} $PARAMS
      else
            # echo ./${EXE} $COORD_FILE_NAME $PARAMS
            ./${EXE} $COORD_FILE_NAME $PARAMS
      fi
      # for i in {1..3}; do ./${EXE} $COORD_FILE_NAME $COUTPUT_FILE_NAME $FOUTPUT_FILE_NAM $NOUTPUT_FILE_NAME; done
      # ./${EXE} $COORD_FILE_NAME $COUTPUT_FILE_NAME $FOUTPUT_FILE_NAM $NOUTPUT_FILE_NAME
else
      echo $SRC did not built to $EXE
fi
