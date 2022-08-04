#ifndef TRANSITIONS_H
#define TRANSITIONS_H
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_CHARS_ATTRIBUTES 20
#define MAX_LINES 6

typedef struct symbolInfo {
	char name[MAX_CHARS_LABEL + 1];
	int value;
	int baseAddress;
	int offset;
	int attributes[4]; /*attribute = {0,0,0,0} (.data,.entry,.extern,code)*/
	struct symbolInfo* nextSymbol; 
}SymbolInfo;

typedef struct symbolList {
	SymbolInfo* head;
	SymbolInfo* tail;
}SymbolList;


typedef struct externals {
	char name[MAX_CHARS_LABEL];
	int base;
	int offset;
	struct externals* nextExternal; 
}External;

typedef struct externalList {
	External* head;
	External* tail;
}ExternalList;

void symbolInfoInit(SymbolInfo* symbol); /* initializes a SymbolInfo struct*/
void symbolListInit(SymbolList* symbolList);  /* initializes a SymbolList struct*/ 
void externalInit(External* external);  /* initializes an External struct*/
void externalListInit(ExternalList* externalList); /* initializes an ExternalList struct*/
int legitLineLength(char* line); /*checks if the line has more than 81 chars*/
int emptyLine(char* line); /*checks if the line is empty*/
int commentLine(char* line); /*checks if the line is a comment line*/
int lineIgnore(char* line); /* returns a flag to the main function telling it whether to ignore the line (returns 1) or read it (returns 0) */ 
int isLabelLine(char* line); /* checks if the line begins with a label definition*/
int labelCheck(char* word); /*checks if the given word can be a valid label name, returns 1 if it can be a label otherise 0*/
int isInLabelTable(char* word, SymbolList* symbolList); /*checks if the first word in the given line is a existing (in the label table) label*/
int baseAddressCalc(int value); /* calculates baseAddress */
SymbolInfo* addToSymbolTable(SymbolList* symbolList); /*adds a new symbol to the symbol table*/
External* externalListAdd(ExternalList* externalList); /*adds a new extern symbol to the extern table*/
void addLabel(int IC,int DC, SymbolList* symbolList, char* line, char* symbolName,int ext); /* adds label to the label table*/
/*--only relevant to the first transition -- , recieves the first char (not including whitespaces) after .data and checks if its ok + increases DC, for example ".data 3,5,8"*/
void dataCheck(char* data,int* DC,int* OK,int row); 
/*--only relevant to the first transition -- recieves the first char (not including whitespaces) after .string and checks if its ok + increases DC, for example ".string "abcd"*/
void stringCheck(char* string,int* DC,int* OK,int row);
/*--only relevant to the first transition -- ,works similar to dataCheck,checks that the comma-parameters use is correct - commas in the right places, and valid operands, returns 1 if valid, else 0  */
int instructionCommaCheck(char* instruction,char* line,int* IC,int* OK,int row);
/* if instructionCommaCheck was found success (returned 1), this func runs to increase IC*/ 
void instructions(char* instruction,char* line,int* IC,int* OK,int row);
/* updates only relevant symbols --if they exist-- to have .data attributes */ 
void updateSymbolsValue(SymbolList* symbolList,int IC);
/* updates only relevant symbols --if they exist-- to have .entry attributes */ 
void updateSymbolsEntry(char* line,SymbolList* symbolList,int row,int* OK);
void firstTran(char* fileName,int* IC,int* DC,int* OK,SymbolList* symbolList);
void undefinedLabels(char* fileName,SymbolList* symbolList,int* OK);
/*--only relevant to the second transition-- ,decodes .data lines and adds them to the machine code*/
void addData(char* data,int* dataRow,char** machineCode);
/*--only relevant to the second transition-- ,decodes .string lines and adds them to the machine code*/
void addString(char* string,int* dataRow,char** machineCode);
SymbolInfo* labelFind(char* word,SymbolList* symbolList); /*--only relevant to the second transition-- , finds the label and returns it's address*/
void secondTran(char* fileName, SymbolList* symbolList,ExternalList* externalHead,int* dataRow,int* instructionRow, int* OK,char** machineCode); /*second transition*/
void object(int IC,int DC,char* fileName,char** machineCode);  /*creates the .ob file*/
void entries(char* fileName,SymbolList* tablehead); /*creates the .ent file*/
void externals (char* fileName,ExternalList* externalList); /*creates the .ext file*/
void mainInstruction(char* fileName); /*the main func for first and second transitions*/
#endif /*!TRANSITIONS_H*/

