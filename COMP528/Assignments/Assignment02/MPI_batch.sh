#!/bin/bash -l

# SLURM Batch Script for MPI Program

#SBATCH -D ./
#SBATCH --export=ALL
#SBATCH -p course
#SBATCH -t 5
#SBATCH -c 32

# Load modules
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

# Read arguments
SRC=$1
COORD_FILE=$2
OUT_FILE_1=$3
OUT_FILE_2=$4
OUT_FILE_3=$5

# Set the executable name from the source code name (remove ".c" and append ".exe")
EXE=${SRC%%.c}.exe

# Delete the existing executable, if it exists
rm -f ${EXE}

echo "Compiling $SRC to $EXE"
# ADD COMPILER INSTRUCTION HERE.
mpiicc -no-multibyte-chars $SRC -o $EXE
echo "Compilation Complete"
echo -------------------------

# Check if the executable was successfully created
if [ ! -f "$EXE" ]; then
    echo "Compilation failed, executable $EXE not found!"
    exit 1
fi

echo "Running $EXE with $COORD_FILE, $OUT_FILE_1, $OUT_FILE_2, $OUT_FILE_3"
echo -------------------------

# SLURM_NTASKS is given by the -n flag when using sbatch.
procs=${SLURM_NTASKS:-1}

# RUN THE PROGRAM HERE USING $procs WITH THE -np FLAG.
mpirun -np $procs ./$EXE $COORD_FILE $OUT_FILE_1 $OUT_FILE_2 $OUT_FILE_3
