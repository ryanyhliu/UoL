#!/bin/bash -l
#Written by Dr Maryam Abo Tabik
# Specify the current working directory as the location for executables/files
# This is the default setting.
#SBATCH -D ./

# Export the current environment to the compute node
# This is the default setting.
#SBATCH --export=ALL

# Specific course queue, exclusive use (for timings), max 1 min wallclock time
#SBATCH -p course
#SBATCH --exclusive
#SBATCH -t 1

# load modules
module load compilers/intel/2019u5 

# just 1 thread to run on
export OMP_NUM_THREADS=1

# GNU no-opt
echo GNU no-opt
gcc -O0 quad.c func1.c  -lm
time ./a.out
echo '-------'

# GNU opt
echo GNU opt
gcc -O3 quad.c func1.c  -lm
time ./a.out
echo '-------'

# INTEL no-opt
echo INTEL no-opt
icc -O0 quad.c func1.c 
time ./a.out
echo '-------'

# Intel opt
echo INTEL opt
icc -O3 quad.c func1.c 
time ./a.out
echo '-------'