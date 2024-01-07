# UoL
University of Liverpool - Advanced Computer Science MSc 

https://github.com/ryanyhliu/UoL.git



# COMP528

## Commands



### OpenMP 

#### Run
./gomp-only 9_coords.coord my_9_cout.dat my_9_fout.dat my_9_nout.dat
./gomp-only 16_coords.coord my_16_cout.dat my_16_fout.dat my_16_nout.dat
./gomp-only 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat



### MPI 
#### Module load
module load compilers/intel/2019u5
module load mpi/intel-mpi/2019u5/bin

#### Run
mpirun -np 4 ./gcomplete 16_coords.coord my_16_cout.dat my_16_fout.dat my_16_nout.dat

#### Barkla run
sbatch -N 16 -n 32 -c 8 OpenMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat



make gomp-only && sbatch -c 32 -t 10 OpenMP_batch.sh gomp-only 512_coords.coord 512_cout_output.dat 512_fout_output.dat 512_nout_output.dat
make gcomplete && sbatch -N 16 -n 32 -c 8  -t 10 OpenMP_batch.sh gcomplete 512_coords.coord my_512_cout.dat my_512_fout.dat my_512_nout.dat
make iomp-only && sbatch -c 32 -t 10 OpenMP_batch.sh iomp-only 512_coords.coord 512_cout_output.dat 512_fout_output.dat 512_nout_output.dat
make icomplete && sbatch -N 16 -n 32 -c 16  -t 10 OpenMP_batch.sh icomplete 512_coords.coord 512_cout_output.dat 512_fout_output.dat 512_nout_output.dat

