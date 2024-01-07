GCC = gcc
ICC = icc

OMP = -fopenmp
IOMP = -qopenmp

MPI = mpicc 
IMPI = mpiicc

VER = -std=c99
LM = -lm



default: gomp-only gcomplete iomp-only icomplete

gomp-only: main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	$(GCC) $(OMP) $(VER) main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o gomp-only $(LM)

gcomplete: main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	$(MPI) $(OMP) $(VER) main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o gcomplete $(LM)

iomp-only: main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	$(ICC) $(IOMP) $(VER) main-openmp-only.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o iomp-only $(LM)

icomplete: main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c
	$(IMPI) $(IOMP) $(VER) main-mpi.c coordReader.c ompcInsertion.c ompfInsertion.c ompnAddition.c -o icomplete $(LM)

clean:
	rm -f gomp-only gcomplete iomp-only icomplete


# na: nAddition.c coordReader.c
# 	$(OMP) $(VER) nAddition.c coordReader.c -o na $(LM)

# mo: main-only.c coordReader.c
# 	$(OMP) $(VER) main-only.c coordReader.c -o mo $(LM)