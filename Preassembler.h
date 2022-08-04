#ifndef PREASSEMBLER_H
#define PREASSEMBLER_H
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_CHARS_ATTRIBUTES 20
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


void macroInit(Macro* macro); /*initializes a Macro struct*/
void macroListInit(MacroList* macroList); /*initializes a MacroList struct*/
Macro* addToMacroTable(MacroList* macroList);  /*adds a macro to the macro list*/
int macroNameAddress(char* line);  /*returns the index where the name of the macro begins*/
int isThereAMacro(char* line);   /*checks if the line has a macro definition in the beginning*/
int isInMacroTable(char* input, MacroList* macrosList); /*searches if the macro exists in a table*/
Macro* macroFinder(char* macro, MacroList* macrosList); /*gets a macro name and searchs for the macro struct in the macro table , returns a pointer*/
void preAssembler(char* fileName); /*main preAssembler func*/
#endif /* !PREASSEMBLER_H */
