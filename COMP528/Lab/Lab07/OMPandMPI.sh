#!/bin/bash -l

#----- Slurm Directives -----

# Specify the partition to run the job
#SBATCH -p course

# Set the maximum wallclock time for the job (format: hours:minutes:seconds)
#SBATCH -t 10:00

# Set the working directory to the directory where the job is submitted
#SBATCH -D ./

# Export the current environment to the compute node
#SBATCH --export=ALL

#----- Module Loads -----

# Load Intel compiler and Intel MPI module
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

#----- Job Information -----

echo "Node list                    : $SLURM_JOB_NODELIST"
echo "Number of nodes allocated    : $SLURM_JOB_NUM_NODES or $SLURM_NNODES"
echo "Number of tasks (MPI processes)      : $SLURM_NTASKS"
echo "Number of processes per node : $SLURM_TASKS_PER_NODE"
echo "Number of CPUs per task (OpenMP threads) : $SLURM_CPUS_PER_TASK"

#----- Compile the Program -----

# SRC = $1 is the name of the source code as an argument
SRC=$1
EXE=${SRC%%.c}.exe

# Delete the existing executable, if it exists
rm -f ${EXE}

echo "Compiling $SRC to $EXE"
echo "-------------------------"

# Compile the source code
mpiicc -qopenmp $SRC -o $EXE

# Check if the executable was created
if test -x $EXE; then 
    # Run the program
    
    # Set the number of OpenMP threads if '-c' flag is used with sbatch
    export OMP_NUM_THREADS=${SLURM_CPUS_PER_TASK:-1} 
    echo "Using ${OMP_NUM_THREADS} OpenMP threads"

    # Run the MPI program
    mpirun -np $SLURM_NTASKS ./$EXE
else
    echo "Error: $SRC did not compile to $EXE"
fi
