#!/bin/bash -l

# SLURM Batch Script for running an MPI program

#SBATCH -D ./
#SBATCH --export=ALL
#SBATCH -p course
#SBATCH -t 5
#SBATCH -c 32

# Load necessary modules
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

# Assign arguments to variables
EXE=$1
COORD_FILE=$2
OUT_FILE_1=$3
OUT_FILE_2=$4
OUT_FILE_3=$5

# Check if the executable exists
if [ ! -f "$EXE" ]; then
    echo "Executable $EXE not found!"
    exit 1
fi

echo "Running $EXE with $COORD_FILE, $OUT_FILE_1, $OUT_FILE_2, $OUT_FILE_3"
echo -------------------------

# SLURM_NTASKS is given by the -n flag when using sbatch.
procs=${SLURM_NTASKS:-1}

# Run the MPI program
mpirun -np $procs ./$EXE $COORD_FILE $OUT_FILE_1 $OUT_FILE_2 $OUT_FILE_3
