#!/bin/bash -l

# Load required modules
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

# SLURM job submission options
#SBATCH -p course -t 2
#SBATCH -D ./
#SBATCH --export=ALL

# SLURM resource allocation
echo "Node list                    : $SLURM_JOB_NODELIST"
echo "Number of nodes allocated    : $SLURM_JOB_NUM_NODES or $SLURM_NNODES"
echo "Number of threads or processes          : $SLURM_NTASKS"
echo "Number of processes per node : $SLURM_TASKS_PER_NODE"
echo "Requested tasks per node     : $SLURM_NTASKS_PER_NODE"
echo "Requested CPUs per task      : $SLURM_CPUS_PER_TASK"
echo "Scheduling priority          : $SLURM_PRIO_PROCESS"

# Set executable and file names from arguments
EXE=$1
COORD_FILE_NAME=$2
COUTPUT_FILE_NAME=$3
FOUTPUT_FILE_NAME=$4
NOUTPUT_FILE_NAME=$5

echo
echo ------------------------------------

# Check if the executable is present and executable
if test -x "$EXE"; then
    # Set number of threads to SLURM_CPUS_PER_TASK or default to 1
    export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1}
    echo "Using ${OMP_NUM_THREADS} OpenMP threads"

    echo "Execution Parameters:"
    echo "Executable: $EXE"
    echo "Coordinate File: $COORD_FILE_NAME"
    echo "COutput File: $COUTPUT_FILE_NAME"
    echo "FOutput File: $FOUTPUT_FILE_NAME"
    echo "NOutput File: $NOUTPUT_FILE_NAME"

    PARAMS="$COORD_FILE_NAME $COUTPUT_FILE_NAME $FOUTPUT_FILE_NAME $NOUTPUT_FILE_NAME"
    procs=${SLURM_NTASKS:-1}

    # Check if EXE ends with 'complete' and run accordingly
    if [[ $EXE == *complete ]]; then
        echo "Running with mpirun: $EXE $PARAMS"
        mpirun -np $procs ./"$EXE" $PARAMS
    else
        echo "Running: $EXE $PARAMS"
        ./"$EXE" $PARAMS
    fi
else
    echo "Executable $EXE did not build or is not present"
fi
