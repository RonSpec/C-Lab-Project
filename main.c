#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "Preassembler.h"
#include "transitions.h"
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_LINES 6

int main(int argc, char** argv) {
	int i = 0;
	char fileName[40];
	if(argc == 1) {
		printf("No file has been entered\n");
	}
	for(i = 1;i < argc;i++) {
		strcpy(fileName,argv[i]);
		preAssembler(fileName);
		mainInstruction(fileName);
	}
	return 0;
}

