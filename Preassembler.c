#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "scans.h"
#include "instructions.h"
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_LINES 6

typedef struct macro {
	char name[MAX_CHARS_LABEL];
	char lines[MAX_LINES*MAX_CHARS];
	struct macro* nextMacro;
}Macro;

typedef struct macroList {
	Macro* head;
	Macro* tail;
}MacroList;

void macroInit(Macro* macro) {
	strcpy(macro->name,"");
	strcpy(macro->lines,"");
	macro->nextMacro = NULL;
}

void macroListInit(MacroList* macroList) {
	macroList->head = NULL;
	macroList->tail = NULL;
}
Macro* addToMacroTable(MacroList* macroList) {
	Macro* macro;
	macro = (Macro*) malloc(sizeof(Macro));
	macroInit(macro);
	if(macroList->head == NULL) {
		macroList->head = macro;
		macroList->tail = macro;
	}
	else {
		macroList->tail->nextMacro=macro;
		macroList->tail = macro;
	}
	return macro;
}

int macroNameAddress(char* line) {
	int i = 0;
	/*skipping the whitespaces in the beginning (in anycase there are any)*/
	while ((line[i] != '\0') && (line[i] == ' ')) {
		i++;
	}
	/*skipping "macro"*/
	while ((line[i] != '\0') && (line[i] != ' ')) {
		i++;
	}
	return i;
}

int isThereAMacro(char* line) { /*checks if the line has a macro definition in the beginning*/
	int isMacro = 0;
	char* first;
	first = firstWord(line);
	/*in case that a macro has been found*/
	if(strcmp(first, "macro") == 0) {
		isMacro = 1;
	}
	free(first);
	if (isMacro == 1) {
		return 1;
	}
	return 0;
}
int isInMacroTable(char* input, MacroList* macrosList) {  /*checks if the macro exists in the macro table*/
	Macro* macro;
	macro = macrosList->head;
	while (macro != NULL) {
		if (strcmp(input, macro->name) == 0) {
			return 1;
		}
		macro = macro->nextMacro;
	}
	return 0;
}

Macro* macroFinder(char* macro, MacroList* macrosList) {
	Macro* macroptr;
	Macro* temp;
	macroptr = macrosList->head;
	while(macroptr != NULL) {
		if(strcmp(macro,macroptr->name) == 0) {
			temp = macroptr;
		}
		macroptr = macroptr->nextMacro;
	}
	return temp;
}

void preAssembler(char* fileName) {
	MacroList macroList;
	Macro* macro;
	int maxCount = MAX_CHARS;
	int mFlag = 0; /*a flag that is being used incase a macro definition has been found*/
	int isMacro = 0;
	int macAdress = 0;
	int skip = 0;
	char* firstword;
	char* macroName;
	char fileNameTemp[40];
	FILE* source;
	FILE* target;
	char line[MAX_CHARS];
	macroListInit(&macroList);
	strcpy(fileNameTemp,fileName);
	strcat(fileNameTemp,".txt");
	source = fopen(fileNameTemp, "r");
	/*strncpy(targetName,("%s.am",argv[1]),10);*/
	strcpy(fileNameTemp,fileName);
	strcat(fileNameTemp,".am");
	target = fopen(fileNameTemp, "w");
	if (source == NULL || target == NULL)
	{
		printf("Error reading file\n");
		exit(0);
	}
	while (fgets(line, maxCount, source) != NULL)
	{
		firstword = firstWord(line);
		isMacro = isInMacroTable(firstword,&macroList);
		/*in case were found a known macro that has been found and needs to be unpackaged */
		if (strcmp(firstword, "endm") == 0) {
			mFlag = 0;
			skip = 1;
		}
		if ((skip == 0) && (strcmp(firstword, "macro") == 0)) {
			mFlag = 1;
			macAdress = macroNameAddress(line); /*place in line where the name of the macro begins*/
			macroName = firstWord(&line[macAdress]); /*getting the macro name through it's address*/
			macro = addToMacroTable(&macroList);
			strcpy(macro->name,macroName);
			free(macroName);
			skip = 1;
		}
		if((skip == 0) && (isMacro == 1)) {
			macro = macroFinder(firstword,&macroList); 
			skip = 1; /* means that were in the declaration line of the macro*/
			fputs(macro->lines,target);
		}
		/*endm has been found*/
		/*in case a line with the word "macro" has been found)*/
		/*in case we're still inside a macro (not including opening and closing lines)*/
		if((skip == 0) && (mFlag == 1) && (strcmp(firstword,"endm") != 0)) { 
			strcat(macro->lines, line);
		}
		/*in case we're not inside a macro, and not opening one*/
		if ((skip == 0) && (mFlag == 0) && (strcmp(firstword,"endm") != 0)) { 
			fputs(line, target); /*copy data to .am file*/
		}
		skip = 0;
		free(firstword);
	}
	/*free(macros.name); <<<<<<<<<<TODO>>>>>>>>>>>
	free(macros.lines);
	free(macros);*/
	fclose(source);
	fclose(target);
}





