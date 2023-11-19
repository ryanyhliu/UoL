CC = gcc -fopenmp
IC = icc -qopenmp
CFLAGS = -O3 -std=c99 -lm

default: ci fi comp fomp icomp ifomp

ci: cInsertion.c coordReader.c
	$(CC) $(CFLAGS) -o ci cInsertion.c coordReader.c

fi: fInsertion.c coordReader.c 
	$(CC) $(CFLAGS) -o fi fInsertion.c coordReader.c

comp: ompcInsertion.c coordReader.c
	$(CC) $(CFLAGS) -o comp ompcInsertion.c coordReader.c

fomp: ompfInsertion.c coordReader.c
	$(CC) $(CFLAGS) -o fomp ompfInsertion.c coordReader.c

icomp: ompcInsertion.c coordReader.c
	$(IC) $(CFLAGS) -o icomp ompcInsertion.c coordReader.c

ifomp: ompfInsertion.c coordReader.c 
	$(IC) $(CFLAGS) -o ifomp ompfInsertion.c coordReader.c

clean:
	rm -f ci fi comp fomp icomp ifomp
