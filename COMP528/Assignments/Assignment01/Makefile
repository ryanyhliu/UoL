CC = gcc -g
CCOMP = gcc -fopenmp
ICOMP = icc -qopenmp
VER = -std=c99
LM = -lm

default: ci fi comp fomp icomp ifomp

ci: cInsertion.c coordReader.c
	$(CC) $(VER) cInsertion.c coordReader.c -o ci.exe $(LM)

fi: fInsertion.c coordReader.c 
	$(CC) $(VER) fInsertion.c coordReader.c -o fi.exe $(LM)

comp: ompcInsertion.c coordReader.c
	$(CCOMP) $(VER) ompcInsertion.c coordReader.c -o comp.exe $(LM)

fomp: ompfInsertion.c coordReader.c
	$(CCOMP) $(VER) ompfInsertion.c coordReader.c  -o fomp.exe $(LM)

icomp: ompcInsertion.c coordReader.c
	$(ICOMP) $(VER) ompcInsertion.c coordReader.c -o icomp.exe $(LM)

ifomp: ompfInsertion.c coordReader.c 
	$(ICOMP) $(VER) ompfInsertion.c coordReader.c -o ifomp.exe $(LM)

clean:
	rm -f ci.exe fi.exe comp.exe fomp.exe icomp.exe ifomp.exe
