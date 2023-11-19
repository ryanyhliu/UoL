CC = gcc -g
CCOMP = gcc -fopenmp
IC = icc -qopenmp
CFLAGS = -std=c99 -lm

default: ci fi comp fomp icomp ifomp

ci: cInsertion.c coordReader.c
	$(CC) $(CFLAGS) -o ci.exe cInsertion.c coordReader.c

fi: fInsertion.c coordReader.c 
	$(CC) $(CFLAGS) -o fi.exe fInsertion.c coordReader.c

comp: ompcInsertion.c coordReader.c
	$(CCOMP) $(CFLAGS) -o comp.exe ompcInsertion.c coordReader.c

fomp: ompfInsertion.c coordReader.c
	$(CCOMP) $(CFLAGS) -o fomp.exe ompfInsertion.c coordReader.c

icomp: ompcInsertion.c coordReader.c
	$(IC) $(CFLAGS) -o icomp.exe ompcInsertion.c coordReader.c

ifomp: ompfInsertion.c coordReader.c 
	$(IC) $(CFLAGS) -o ifomp.exe ompfInsertion.c coordReader.c

clean:
	rm -f ci.exe fi.exe comp.exe fomp.exe icomp.exe ifomp.exe
