assembler: main.o scans.o Preassembler.o transitions.o instructions.o
	gcc -g -ansi -pedantic -Wall main.o scans.o Preassembler.o transitions.o instructions.o -o assembler 
Source.o: main.c 
	gcc -g -c -ansi -pedantic -Wall main.c -o main.o
scans.o: scans.c 
	gcc -g -c -ansi -pedantic -Wall scans.c -o scans.o
Preassembler.o: Preassembler.c 
	gcc -g -c -ansi -pedantic -Wall Preassembler.c -o Preassembler.o
transitions.o: transitions.c 
	gcc -g -c -ansi -pedantic -Wall transitions.c -o transitions.o
instructions.o: instructions.c
	gcc -g -c -ansi -pedantic -Wall instructions.c -o instructions.o
