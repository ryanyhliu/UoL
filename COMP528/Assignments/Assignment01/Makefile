CC = gcc -g
CCOMP = gcc -fopenmp
ICOMP = icc -qopenmp
CFLAGS = -std=c99
LM = -lm

default: ci fi comp fomp icomp ifomp

ci: cInsertion.c coordReader.c
	$(CC) $(CFLAGS) -o ci.exe cInsertion.c coordReader.c $(LM)

fi: fInsertion.c coordReader.c 
	$(CC) $(CFLAGS) -o fi.exe fInsertion.c coordReader.c $(LM)

comp: ompcInsertion.c coordReader.c
	$(CCOMP) $(CFLAGS) -o comp.exe ompcInsertion.c coordReader.c $(LM)

fomp: ompfInsertion.c coordReader.c
	$(CCOMP) $(CFLAGS) -o fomp.exe ompfInsertion.c coordReader.c $(LM)

icomp: ompcInsertion.c coordReader.c
	$(ICOMP) $(CFLAGS) -o icomp.exe ompcInsertion.c coordReader.c $(LM)

ifomp: ompfInsertion.c coordReader.c 
	$(ICOMP) $(CFLAGS) -o ifomp.exe ompfInsertion.c coordReader.c $(LM)

clean:
	rm -f ci.exe fi.exe comp.exe fomp.exe icomp.exe ifomp.exe
