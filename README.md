# UoL
University of Liverpool - Advanced Computer Science MSc 

https://github.com/ryanyhliu/UoL.git



# COMP528

## Commands



### OpenMP 

#### Run
./gomp-only 9_coords.coord my_9_cout.dat my_9_fout.dat my_9_nout.dat && cat my*
./gomp-only 16_coords.coord my_16_cout.dat my_16_fout.dat my_16_nout.dat && cat my*
./gomp-only 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat && cat my*



### MPI 
#### Module load
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

#### Run
mpirun -np 4 ./gcomplete 9_coords.coord my_9_cout.dat my_9_fout.dat my_9_nout.dat && cat my*
mpirun -np 4 ./gcomplete 16_coords.coord my_16_cout.dat my_16_fout.dat my_16_nout.dat && cat my*

#### Barkla run
sbatch -N 16 -n 32 -c 8 OpenMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat



sbatch -N 1 -n 1 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 1 -c 2 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 1 -c 4 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat 
sbatch -N 1 -n 1 -c 8 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat 
sbatch -N 1 -n 1 -c 16 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 1 -c 32 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat

sbatch -N 1 -n 1 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 2 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 4 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 8 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 16 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
sbatch -N 1 -n 32 -c 1 -t 10 0penMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat

