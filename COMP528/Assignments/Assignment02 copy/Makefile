GCC = gcc
ICC = icc

OMP = -fopenmp
IOMP = -qopenmp

MPI = mpicc 
IMPI = mpiicc

VER = -std=c99
LM = -lm

# default: ci fi comp fomp icomp ifomp

# ci: cInsertion.c coordReader.c
# 	$(CCOMP) $(VER) cInsertion.c coordReader.c -o ci $(LM)

# fi: fInsertion.c coordReader.c 
# 	$(CCOMP) $(VER) fInsertion.c coordReader.c -o fi $(LM)

# comp: ompcInsertion.c coordReader.c
# 	$(CCOMP) $(VER) ompcInsertion.c coordReader.c -o comp $(LM)

# fomp: ompfInsertion.c coordReader.c
# 	$(CCOMP) $(VER) ompfInsertion.c coordReader.c  -o fomp $(LM)

# icomp: ompcInsertion.c coordReader.c
# 	$(ICOMP) $(VER) ompcInsertion.c coordReader.c -o icomp $(LM)

# ifomp: ompfInsertion.c coordReader.c 
# 	$(ICOMP) $(VER) ompfInsertion.c coordReader.c -o ifomp $(LM)

# clean:
# 	rm -f ci fi comp fomp icomp ifomp

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