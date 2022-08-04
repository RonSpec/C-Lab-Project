#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "scans.h"
#include "instructions.h"
#include "transitions.h"
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_LINES 6


void symbolInfoInit(SymbolInfo* symbol) {
	int i = 0;
	for(i = 0;i < 4; i++) {
		symbol->attributes[i] = 0;
	}
	symbol->value = 0;
	symbol->baseAddress = 0;
	symbol->offset = 0;
	strcpy(symbol->name,"");
	symbol->nextSymbol = NULL;
}

void symbolListInit(SymbolList* symbolList) {
	symbolList->head = NULL;
	symbolList->tail = NULL;
}

void externalInit(External* external) {
	external->base = 0;
	external->offset = 0;
	strcpy(external->name,"");
	external->nextExternal = NULL;
}

void externalListInit(ExternalList* externalList) {
	externalList->head = NULL;
	externalList->tail = NULL;
}

/*checks if the line has more than 81 chars*/
int legitLineLength(char* line) { 
	if ((strlen(line)) < (MAX_CHARS - 1)) {
		return 1;
	}
	else {
		return 0;
	}
}

/*checks if the line is empty*/
int emptyLine(char* line) { 
	int i;
	i = 0;
	while((line[i] != '\0') && (line[i] == ' ')) {
		i++;
	}
	if((line[i] == '\0') || (line[i] == '\n')) {
		return 1;
	}
	else {
		return 0;
	}
}

/*checks if the line is a comment line*/
int commentLine(char* line) { 
	if(line[0] == ';') {
		return 1;
	}
	else {
		return 0;
	}
}

/* returns a flag to the main function telling it whether to ignore the line (returns 1) or read it (returns 0) */ 
int lineIgnore(char* line) {
	if((emptyLine(line) == 1) || (commentLine(line) == 1)) {
		return 1;
	}
	return 0;
}

/*checks if the first word in the given line is a label with ": " after it*/
int isLabelLine(char* line) { 
	int legit = 0;
	int i = 0;
	int isNotRegisterOrFunc = 1;
	int isColonChar = 0; 
	int isSpace = 0;
	char* temp;
	char* fWord; /* first address of char of line*/
	temp = firstWord(line);
	fWord = whitespaceRemover(line);
	/*in case the "label" is too long,or a register/func*/
	if((isRegister(temp) == 1) || (legitFunc(temp) == 1)) {
		isNotRegisterOrFunc = 0;
	}
	free(temp);
	if ((strlen(fWord) > MAX_CHARS_LABEL) || (isNotRegisterOrFunc == 0)) {
		return 0;
	}
	if (isalpha(fWord[0]) != 0) {
		legit = 1;
	}
	while ((fWord[i] != '\0') && (legit != 0) && (fWord[i] != ':')) {
		if (isalnum(fWord[i]) == 0) {
			legit = 0;
		}
		i++;
	}
	/*checking if the last char is ':' when the rest of the chars are ok*/
	if((fWord[i] == ':') && (fWord[i + 1] == ' ')) {
		isColonChar = 1;
		isSpace = 1;
	}
	free(fWord);
	if ((legit == 1) && (isColonChar == 1) && (isSpace == 1)) {
		return 1;
	}
	return 0;
}

/*checks if the given word can be a valid label, returns 1 if it can be a label otherise 0*/
int labelCheck(char* word) {  
	int legit = 0;
	int i = 0;
	char* fWord; /* first word of line*/
	fWord = whitespaceRemover(word);
	/*in case the "label" is too long,or a register/funct/label*/
	if ((strlen(fWord) > MAX_CHARS_LABEL) || (isRegister(word) == 1) || (legitFunc(word) == 1)) {
		free(fWord);
		return 0;
	}
	if (isalpha(fWord[0]) != 0) {
		legit = 1;
	}
	while ((fWord[i] != '\0') && (legit != 0)) {
		if (isalnum(fWord[i]) == 0) {
			legit = 0;
		}
		i++;
	}
	free(fWord);
	/*checking if the last char is ':' when the rest of the chars are ok*/
	if (legit == 1) {
		return 1;
	}
	return 0;
}

/*checks if the first word in the given line is a existing (in the label table) label*/
int isInLabelTable(char* word,SymbolList* symbolList) { 
	int found = 0;
	SymbolInfo* tableptr = symbolList->head;
	while(tableptr != NULL) {
		if ((strcmp(word, tableptr->name)) == 0) {
			/*in case that there's already a label with the attribute - .extern*/
			if(tableptr->attributes[2] == 1) {
				found = 2;
			}
			else {
				found = 1;
			}
		}
		tableptr = tableptr->nextSymbol;
	}
	return found;
}

int baseAddressCalc(int value) {
	int temp = 0;
	temp = (value / 16) * 16;
	return temp;
}

SymbolInfo* addToSymbolTable(SymbolList* symbolList) {
	SymbolInfo* symbol;
	symbol = (SymbolInfo*) malloc(sizeof(SymbolInfo));
	symbolInfoInit(symbol);
	if(symbolList->head == NULL) {
		symbolList->head = symbol;
		symbolList->tail = symbol;
	}
	else {
		symbolList->tail->nextSymbol=symbol;
		symbolList->tail = symbol;
	}
	return symbol;
}
External* externalListAdd(ExternalList* externalList) {
	External* external;
	external = (External*) malloc(sizeof(External));
	externalInit(external);
	if(externalList->head == NULL) {
		externalList->head = external;
		externalList->tail = external;
	}
	else {
		externalList->tail->nextExternal = external;
		externalList->tail = external;
	}
	return external;
}

/* adds label to the label table through the line given*/
void addLabel(int IC,int DC, SymbolList* symbolList, char* line, char* symbolName,int ext) {
	char* tempWord;
	SymbolInfo* tableptr = addToSymbolTable(symbolList);
	tempWord = firstWord(line);
	/*if it's a .extern line*/
	if(ext == 1) {
		tableptr->attributes[2] = 1;
	}
	strcpy(tableptr->name, symbolName);
	/*if it's a .data line*/
	if((strcmp(tempWord, ".data") == 0) || (strcmp(tempWord, ".string") == 0)){
		tableptr->attributes[0] = 1; /* attributes = {1,0,..}*/
		tableptr->value = DC; /*setting the value for the label in the label-table*/
		tableptr->baseAddress = 0; /*setting the value for the label in the label-table*/
		tableptr->offset = 0; /*setting the value for the label in the label-table*/
	}

	/*if it's an instruction/code line*/ 
	if(legitFunc(tempWord) == 1) {
		tableptr->attributes[3] = 1; /*attributes = {0,0,..,1}*/
		tableptr->value = (IC+100); /*setting the value for the label in the label-table*/
		tableptr->baseAddress = baseAddressCalc(tableptr->value); /*setting the value for the label in the label-table*/
		tableptr->offset = (tableptr->value - tableptr->baseAddress); /*setting the value for the label in the label-table*/
	}
	free(tempWord);
}
/*recieves the first char (not including whitespaces) after .data and checks if its ok + increases DC, for example ".data 3,5,8"*/
void dataCheck(char* data,int* DC,int* OK,int row) { 
	int runningCnt = 0;
	int cnt = 0;
	int comma = 1; /* comma flag: lets us know when there's a comma between two numbers*/
	char* tempWord;
	while((data[runningCnt] != '\0') && (data[runningCnt] != '\n')) {
		/* skipping possible whitespaces before the parameter */ 
		while((data[runningCnt] != '\0') && (data[runningCnt] != '\n') && (data[runningCnt] == ' ')) {
				runningCnt++;
		}
		/*in case that we got to the end of the line */
		if((data[runningCnt] == '\0') || (data[runningCnt] == '\n')) {
			return;
		}
		if(data[runningCnt] == ',') {
			/*reading comma as first parameter*/
			if(runningCnt == 0) {
				printf("ERROR IN LINE %d: Illegal Comma, no number has been entered before the comma\n",row);
				*OK = 0;
			}
			if((comma == 1) && (runningCnt != 0)) {
				printf("ERROR IN LINE %d: Illegal Comma, more than one comma in a row\n",row);
				*OK = 0;
			}
			comma = 1;
			runningCnt++;
		}	
		/*skipping possible whitespaces after the comma*/
		while((data[runningCnt] != '\0') && (data[runningCnt] != '\n') && (data[runningCnt] == ' ')) {
				runningCnt++;
		}
		/* in case that there's a '+' or '-' sign before the number we skip it, for example: +67,-54,etc.. (important: numbers like ++62 --20 are not legit)*/
		if((data[runningCnt] == '+') || (data[runningCnt] == '-')) {
			runningCnt++;
			if(!isdigit(data[runningCnt])) {
				printf("ERROR IN ROW %d: %c is not a number!\n",row,data[runningCnt-1]);
				*OK = 0;
			}
		}
		if(comma == 0) {
			printf("ERROR IN LINE %d: theres a missing comma between the parameters \n",row);
			*OK = 0;
		}
		/* building a "number" from the data given*/
		tempWord = (char*) malloc(2*sizeof(char));
		while((data[runningCnt] != '\0') && (data[runningCnt] != '\n') && (data[runningCnt] != ' ') && (data[runningCnt] != ',')) {
			tempWord = realloc(tempWord,(cnt+2));
			tempWord[cnt] = data[runningCnt];
			runningCnt++;
			cnt++;
		}
		tempWord[cnt] = '\0';
		if(isNumber(tempWord) == 1) {
			(*DC)++;
		}
		if((tempWord[0] != '\0') && (isNumber(tempWord) == 0)) {
			if(tempWord[0] == '+') 
			printf("ERROR IN ROW %d: %s is not a number!\n",row,tempWord);
			*OK = 0;
		}
		if(cnt != 0) {
			comma = 0;
		}
		cnt = 0;
		free(tempWord);
	}
	tempWord = firstWord(data);
	if((comma == 1) && (strcmp(tempWord,",") != 0)) {
		printf("ERROR IN LINE %d: Illegal Comma, there's a comma after the last parameter\n",row);
		*OK = 0;
	}
	free(tempWord);
}
void stringCheck(char* string,int* DC,int* OK,int row) {
	int runningCnt = 0;
	int cnt = 0;
	char* temp;
	if(string[0] != '\"') {
		printf("ERROR IN LINE %d: string does not begin with a \"\n",row);
		*OK = 0;
	}
	runningCnt++;
	while((string[runningCnt] != '\0') && (string[runningCnt] != '\n') && (string[runningCnt] != '\"')) {
		runningCnt++;
		cnt++;	
	}
	if(string[runningCnt] != '\"') {
		printf("ERROR IN LINE %d: string does not end with a \"\n",row);
		*OK = 0;
	}
	if(string[runningCnt] == '\"') {
		temp = nextWord(&(string[runningCnt]));
		if((temp[0] != '\0') && (temp[0] != '\n')) {
			printf("ERROR IN LINE %d: extra parameters after \"\n",row);
			*OK = 0;
		}
		if(string[0] == '\"') {
			(*DC) = (*DC) + cnt + 1 ; /* +1 is for the '\0' char*/
		}
	}
}
/* is similar to dataCheck,checks that the comma-parameters use is correct - commas in the right places, and valid operands, returns 1 if valid, else 0  */
int instructionCommaCheck(char* instruction,char* line,int* IC,int* OK,int row) {
	/*int addressing[2];*/ /* {0-3,0-3} -> {source,destination} */
	int valid = 1;
	int runningCnt = 0;
	int cnt = 0;
	int operandsFound = 0; 
	int operands = 0;
	int comma = 1; /* comma flag: lets us know when there's a comma between two numbers*/
	char* tempWord;
	operands = operandsNum(instruction);
	/*while we didn't get to the end of the line, or not enough operands have been found*/
	while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && (operandsFound < operands)) {
		/* skipping possible whitespaces before the parameter */ 
		while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && (line[runningCnt] == ' ')) {
			runningCnt++;
		}
		if(line[runningCnt] == ',') {
			if(runningCnt == 0) {
				printf("ERROR IN LINE %d: Illegal Comma, no operand has been entered before the comma\n",row);
				*OK = 0;
				valid = 0;
			}
			if((comma == 1) && (runningCnt != 0)) {
				printf("ERROR IN LINE %d: Illegal Comma, more than one comma in a row\n",row);
				*OK = 0;
				valid = 0;
			}
			comma = 1;
			runningCnt++;
		}
		/*skipping possible whitespaces after the comma*/
		while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && (line[runningCnt] == ' ')) {
			runningCnt++;
		}		
		if(comma == 0) {
			printf("ERROR IN LINE %d: theres a missing comma between the operands \n",row);
			*OK = 0;
			valid = 0;
		}
		/* building an "operand" from the data given*/
		tempWord = (char*) malloc(2*sizeof(char));
		while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && (line[runningCnt] != ' ') && (line[runningCnt] != ',')) {
			tempWord = realloc(tempWord,(cnt+2));
			tempWord[cnt] = line[runningCnt];
			runningCnt++;
			cnt++;
		}
		tempWord[cnt] = '\0';
		if(addressingModes(tempWord) == -1) {
			*OK = 0;
			printf("ERROR IN ROW %d: %s is not a known operand!\n",row,tempWord);
			valid = 0;
		}
		operandsFound++;
		if(operandsFound > operands) {
			printf("ERROR IN LINE %d: TOO MANY OPERANDS\n",row);
			*OK = 0;
			valid = 0;
		}
		if(cnt != 0) {
			comma = 0;
		}
		cnt = 0;
		free(tempWord);
	}
	if((nextWord(&line[runningCnt])[0]) == ',') {
		printf("ERROR IN LINE %d: Illegal Comma, there's a comma after the last parameter\n",row);
		*OK = 0;
		valid = 0;
	}
	return valid;
}
/* if instructionCommaCheck was found success (returned 1), this func runs to increase IC*/ 
void instructions(char* instruction,char* line,int* IC,int* OK,int row) {
	int runningCnt = 0;
	int cnt = 0;
	int operands = -1;
	int operandsFound = 0;
	char* tempWord;
	int sourceOrDestination = 1; /*when 0, we check the source Method, when 1 we check the destination Method*/
	int method = -1;
	/*because these instructions are the only instructions to have source method*/ 
	if((strcmp(instruction,"mov") == 0) || (strcmp(instruction,"cmp") == 0) || (strcmp(instruction,"add") == 0) || (strcmp(instruction,"sub") == 0)
			|| (strcmp(instruction,"lea") == 0)) {
		sourceOrDestination = 0;
	}
	operands = operandsNum(instruction);
	/*increasing IC depending on the amount of operands as written in page 18 of instructions maman 14*/
	if(operands == 0) {
		(*IC) += 1;
	}	
	if(operands > 0) {
		(*IC) += 2;
	}
	while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && (operandsFound < operands)) {
		/* skipping whitespaces and comma */ 
		while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && ((line[runningCnt] == ' ') || (line[runningCnt] == ','))) {
			runningCnt++;
		}
		/*building an "operand" from the data given*/
		tempWord = (char*) malloc(2*sizeof(char));
		while((line[runningCnt] != '\0') && (line[runningCnt] != '\n') && (line[runningCnt] != ' ') && (line[runningCnt] != ',')) {
			tempWord = realloc(tempWord,(cnt+2));
			tempWord[cnt] = line[runningCnt];
			runningCnt++;
			cnt++;
		}
		tempWord[cnt] = '\0';
		operandsFound++;
		cnt = 0;
		method = addressingModes(tempWord);
		if(sourceOrDestination == 0) {
			if(sourceMethod(instruction,method) == 0) {
				printf("ERROR IN LINE %d: The source method %d is not made for %s \n", row, method, instruction);
				*OK = 0;
			}
			else {
				if(method == 0) {
					(*IC)++; 
				}
				if((method == 1) || (method == 2)) {
					(*IC)+=2;
				}
			}
		}
		if(sourceOrDestination == 1) {
			if(destinationMethod(instruction,method) == 0) {
				printf("ERROR IN LINE %d: The destination method %d is not made for %s \n", row, method, instruction);
				*OK = 0;
			}
			else {
				if(method == 0) {
					(*IC)++; 
				}
				if((method == 1) || (method == 2)) {
					(*IC)+=2;
				}
			}
		}
		sourceOrDestination = 1;
		free(tempWord);
	}
}

/*update the data symbols which have the attributes of data or string*/
void updateSymbolsValue(SymbolList* symbolList,int IC) {
	SymbolInfo* ptr; 
	ptr = symbolList->head;
	while(ptr != NULL) {
		/*in case that the symbol is a .data type of*/
		if(ptr->attributes[0] == 1) {
			ptr->value += (IC+100); /*setting the value for the label in the label-table*/
			ptr->baseAddress = baseAddressCalc(ptr->value); /*setting the value for the label in the label-table*/
			ptr->offset = (ptr->value - ptr->baseAddress); /*setting the value for the label in the label-table*/
		}
			ptr = ptr->nextSymbol;
	}
}

/* gets a line that has a label after .entry */
void updateSymbolsEntry(char* line,SymbolList* symbolList,int row,int* OK) {
	char* label;
	SymbolInfo* ptr;
	ptr = symbolList->head;
	/* skipping possible whitespaces in the beginning*/
	label = firstWord(line); 
	if(labelCheck(label) == 1) {
		if(numberOfParameters(line) > 1) {
			printf("ERROR IN ROW %d: there are more than 1 operand (the label) in the given line",row); 
			*OK = 0;
		}
		if(isInLabelTable(label,symbolList) == 0) {
			printf("ERROR IN ROW %d: the label %s is not familiar",row,label); 
			*OK = 0;
		}
		/* in case there is label without .entry, or the label already got the attribute of .entry*/
		else {
			while(ptr != NULL) {
				if(strcmp(ptr->name,label) == 0) {
					ptr->attributes[1] = 1;
				}
				ptr = ptr->nextSymbol;
			}
		}
	}
	free(label);
}

/*builds almost complete symbol table (without extern attributes, but with the rest of the values), updates IC and DC,
	checks validity of instructions/parameters/ and prints errors if needed,in case that OK stays 1 we can move to secondTran to encode the codes and create the required files*/  
void firstTran(char* fileName,int* IC,int* DC,int* OK,SymbolList* symbolList) {
	int row = 0;
	int firstLabel = 1;
	int instructionCheck = 0;
	int maxCount = MAX_CHARS*2;
	char* firstword; char* tempWord; char* secondTemp; char* tempLine; /*temporary pointers that will help with scans*/
	int labelFlag = 0, instructionFlag = 1; /*1)a flag that is being used in case a label has been found,2) a flag for instructions*/
	char word[MAX_CHARS]; /*a temporary word*/
	FILE* source;
	char line[MAX_CHARS];
	/* SymbolInfo* tableptr; */
	source = fopen(fileName, "r"); /*>>>need to change these<<<*/
	if (source == NULL)
	{
		printf("Error reading file\n");
		exit(0);
	}
	while (fgets(line, maxCount, source) != NULL)
	{	
		row++;
		if(legitLineLength(line) == 0) {
			*OK = 0;
			printf("ERROR IN ROW %d: THE LENGTH IS ABOVE %d\n",row,(MAX_CHARS-2)); 
		}
		/*in case that the line isn't a blank/comment line, read it*/
		if(lineIgnore(line) == 0) {
			instructionFlag = 1;
			tempLine = line;
			firstword = firstWord(line); /*first word of given line*/
			strcpy(word, firstword);
			free(firstword);
			labelFlag = isLabelLine(line);
			/*in any case that we find a label in the beginning of the line*/
			if (labelFlag == 1) {
				/*removing the ':'*/
				word[strlen(word) - 1] = '\0'; 
				/*checking if its the first label found or the label is completly new in order to add it*/
				tempLine = nextWord(line); /*skip to the next (--address of--) word after the label*/
				if ((firstLabel == 1) || (isInLabelTable(word, symbolList) == 0)) {
					addLabel(*IC,*DC, symbolList,tempLine, word,0);
					firstLabel = 0;		
				}
				else {
					printf("ERROR IN ROW %d: The label already exists in the table\n",row);
					*OK = 0;
				}
			}
			tempWord = firstWord(tempLine);
			/*in case its a .string line*/
			if(strcmp(tempWord,".string") == 0) { 
				instructionFlag = 0;
				secondTemp = nextWord(tempLine); /*the start of the string parameters address*/
				stringCheck(secondTemp,DC,OK,row);
			}
			/*in case its a .data line*/
			if(strcmp(tempWord,".data") == 0) { 
				instructionFlag = 0;
				secondTemp = nextWord(tempLine); /*the start of the string parameters address*/
				dataCheck(secondTemp,DC,OK,row);
			}	
			/*in case its a .extern line*/
			if ((strcmp(tempWord, ".extern") == 0)) {
				instructionFlag = 0;
				tempLine = nextWord(tempLine);
				secondTemp = firstWord(tempLine);
				if ((labelCheck(secondTemp)) == 1) {
					if ((firstLabel == 1) || (isInLabelTable(secondTemp, symbolList) == 0)) {
						addLabel(0,0,symbolList, tempLine, secondTemp,1);
						firstLabel = 0;
					}
					else {
						printf("ERROR IN ROW %d: The label already exists in the table\n",row);
						*OK = 0;
					}
				}
				if ((labelCheck(secondTemp)) == 0) {
					printf("ERROR IN ROW %d: The word after .extern is not a label\n",row);
					*OK = 0;
				}
				free(secondTemp);
			}
			/*in case its a .entry line*/
			if(strcmp(tempWord,".entry") == 0) { 
				instructionFlag = 0;
			}
			/*in case that the word is not an instruction*/
			if((instructionFlag == 1) && (legitFunc(tempWord) == -1)) {
				printf("ERROR IN ROW %d: %s is not an instruction \n",row,tempWord);
				*OK = 0;
			}
			/* in case that there's an instruction after .extern/.string/.data */
			if((instructionFlag == 0) && (legitFunc(tempWord) == 1)) {
				printf("ERROR IN ROW %d: invalid use of an instruction \n",row);
				*OK = 0;
			}
			/* in case that it's an instruction line */
				if(legitFunc(tempWord) == 1) {
				secondTemp = nextWord(tempLine); 
				instructionCheck = instructionCommaCheck(tempWord,secondTemp,IC,OK,row);
				if(instructionCheck == 1) {
					instructions(tempWord,secondTemp,IC,OK,row);
				}
			}
			free(tempWord);
		}
	}
	fclose(source);
	updateSymbolsValue(symbolList,*IC);
	undefinedLabels(fileName,symbolList,OK);

}
void undefinedLabels(char* fileName,SymbolList* symbolList,int* OK) {
	int row = 0;
	int maxCount = MAX_CHARS*2;
	char* tempWord; char* tempLine; /*temporary pointers that will help with scans*/
	FILE* source;
	char filename[30];
	char line[MAX_CHARS];
	char* sourceOperand;
	char* destinationOperand;
	char* label;
	strcpy(filename,fileName);
	source = fopen(fileName, "r"); 
	if (source == NULL)
		{
			printf("Error reading file\n");
			exit(0);
		}
	row = 0;
	while (fgets(line, maxCount, source) != NULL)
	{
		tempLine = line;
		row++;
		if(isLabelLine(line) == 1) {
			tempLine = nextWord(tempLine);
		}
		tempWord = firstWord(tempLine);
		if(legitFunc(tempWord) == 1) {
			tempLine = nextWord(tempLine);
			sourceOperand = (char*) malloc(50*sizeof(char));
			destinationOperand = (char*) malloc(50*sizeof(char));
			operandPick(tempLine,sourceOperand,destinationOperand);
			if((strcmp(sourceOperand,"!NULL") != 0) && ((addressingModes(sourceOperand) == 1) || (addressingModes(sourceOperand) == 2))) {
				label = (char*) malloc(MAX_CHARS_LABEL*sizeof(char));
				if(addressingModes(sourceOperand) == 1) {
					strcpy(label,sourceOperand);
				}
				else {
					label = labelExtract(sourceOperand);
				}
				if(isInLabelTable(label,symbolList) == 0) {
					printf("ERROR IN ROW %d: %s is not a defined label\n",row,label);
					*OK = 0;
				}
				free(label);
			}
			if((strcmp(destinationOperand,"!NULL") != 0) && ((addressingModes(destinationOperand) == 1) || (addressingModes(destinationOperand) == 2))) {
				label = (char*) malloc(MAX_CHARS_LABEL*sizeof(char));
				if(addressingModes(destinationOperand) == 1) {
					strcpy(label,destinationOperand);
				}
				else {
					label = labelExtract(destinationOperand);
				}
				if(isInLabelTable(label,symbolList) == 0) {
					printf("ERROR IN ROW:%d, %s is not a defined label\n",row,label);
					*OK = 0;
				}
				free(label);
			}
			free(sourceOperand);
			free(destinationOperand);
		}
	}
	fclose(source);
}

void addData(char* data,int* dataRow,char** machineCode) {
	int sign = 1; /*when theres a "-" or "+" before numbers, positive or negative*/
	int runningCnt = 0;
	int i = 0;
	char tempNumber[6];
	int number;
	int rowCode = *dataRow;
	while((data[runningCnt] != '\0') && (data[runningCnt] != '\n')) {
		if(data[runningCnt] == '-') {
			sign = -1;
			runningCnt++;
		}
		/*creating the temporary number*/
		while((data[runningCnt] != '\0') && (data[runningCnt] != '\n') && (data[runningCnt] != ' ') && (data[runningCnt] != ',')) {
			tempNumber[i] = data[runningCnt];
			i++;
			runningCnt++;
		}
		tempNumber[i] = '\0';
		i = 0;
		number = (atoi(tempNumber) * sign);
		sign = 1;
		intToBinaryCharArray(number,0,15,rowCode,machineCode);
		machineCode[rowCode][16] = '0';
		machineCode[rowCode][17] = '0';
		machineCode[rowCode][18] = '1';
		machineCode[rowCode][19] = '0';
		machineCode[rowCode][20] = '\0';
		(*dataRow)++;
		rowCode = *dataRow;
		/*skipping to the next number*/
		while((data[runningCnt] == ' ') || (data[runningCnt] == ',')) {
			runningCnt++;
		}
	}
}

void addString(char* string,int* dataRow,char** machineCode) {
	int number;
	int runningCnt = 1; /*because data[0] = '\"'*/
	int rowCode;
	rowCode = *dataRow;
	while((string[runningCnt] != '\0') && (string[runningCnt] != '\n') && (string[runningCnt] != '\"')) {
		number = (int) string[runningCnt];
		intToBinaryCharArray(number,0,15,rowCode,machineCode);
		machineCode[rowCode][16] = '0';
		machineCode[rowCode][17] = '0';
		machineCode[rowCode][18] = '1';
		machineCode[rowCode][19] = '0';
		machineCode[rowCode][20] = '\0';
		(*dataRow)++;
		rowCode = *dataRow;
		runningCnt++;
	}
	intToBinaryCharArray(0,0,15,rowCode,machineCode);
	machineCode[rowCode][16] = '0';
	machineCode[rowCode][17] = '0';
	machineCode[rowCode][18] = '1';
	machineCode[rowCode][19] = '0';
	machineCode[rowCode][20] = '\0';
	(*dataRow)++;
}

SymbolInfo* labelFind(char* word,SymbolList* symbolList) { 
	SymbolInfo* tableptr;
	tableptr = symbolList->head;
	while(tableptr != NULL) {
		if ((strcmp(word, tableptr->name)) == 0) {
			return tableptr;
		}
		tableptr = tableptr->nextSymbol;
	}
	return tableptr;
}

/*after validity has been checked in firstTran we activate secondTran*/
void secondTran(char* fileName, SymbolList* symbolList,ExternalList* externalList,int* dataRow,int* instructionRow, int* OK,char** machineCode) {
	int row = 0;
	int tempOpcode = 0;
	int tempFunct = 0;
	int maxCount = MAX_CHARS*2;
	char* firstword; char* tempLine; /*temporary pointers that will help with scans*/
	char* sourceOperand;
	char line[MAX_CHARS];
	char* destinationOperand;
	char instruction[5];
	FILE* source;
	source = fopen(fileName, "r"); /*>>>need to change these<<<*/
	if (source == NULL)
	{
		printf("Error reading file\n");
		exit(0);
	}
	while (fgets(line, maxCount, source) != NULL)
	{
		firstword = firstWord(line);
		row++;
		/*in case that the line isn't a blank/comment line, read it*/
		if(lineIgnore(line) == 0) {
			tempLine = line;
			firstword = firstWord(tempLine);
			/* in case that its a .entry line we update the label - in case we have it*/
			if(strcmp(firstword,".entry") == 0) {
				tempLine = nextWord(line);
				updateSymbolsEntry(tempLine,symbolList,row,OK);
			}
			free(firstword);

			/* in case that there's a label */ 
			if(isLabelLine(tempLine) == 1) {
				tempLine = nextWord(line);
			}
			firstword = firstWord(tempLine);
			/* in case that there's a .data line */
			if(strcmp(firstword,".data") == 0) {
				tempLine = nextWord(tempLine);
				addData(tempLine, dataRow, machineCode);
			}
			/* in case that there's a .string line */
			if(strcmp(firstword,".string") == 0) {
				tempLine = nextWord(tempLine);
				addString(tempLine, dataRow, machineCode);
			}
			/* in case that it's an instruction line */
			if(legitFunc(firstword) == 1) {	
				strcpy(instruction,firstword);
				tempOpcode = opCode(instruction);
				firstWordDecoder(tempOpcode, instructionRow, machineCode); /*add the first word >> opcode word*/
				tempFunct = funct(instruction);
				tempLine = nextWord(tempLine); /*brings us to the operands: mov r3,W >>> r3,W*/
				sourceOperand = (char*) malloc(50*sizeof(char));
				destinationOperand = (char*) malloc(50*sizeof(char));
				operandPick(tempLine,sourceOperand,destinationOperand);
				secondWordDecoder(sourceOperand,destinationOperand,tempFunct,instructionRow,machineCode);
				extraWordsDecoder(sourceOperand,destinationOperand,instructionRow,machineCode,symbolList,externalList);
				free(sourceOperand);
				free(destinationOperand);
			}
			free(firstword);
		}
	}
	fclose(source);
}

void object(int IC,int DC,char* fileName,char** machineCode) {
	int i = 0;
	int a = 0, b = 0, c = 0, d = 0, e = 0;
	char A,B,C,D,E;
	int end = (100+IC+DC);
	FILE* target;
	char file[40]; 
	strcpy(file,fileName);
	strcat(file,".ob");
	target = fopen(file, "w");
	if (target == NULL)
	{
		printf("Error reading file\n");
		exit(0);
	}	
	fprintf(target,"\t\t\t\t %d %d\n",IC,DC);
	for(i = 100;i < end; i++) {
		a = binaryCharArrayToInt(19,machineCode,i);
		b = binaryCharArrayToInt(15,machineCode,i);
		c = binaryCharArrayToInt(11,machineCode,i);
		d = binaryCharArrayToInt(7,machineCode,i);
		e = binaryCharArrayToInt(3,machineCode,i);
		A = binaryToHex(a);
		B = binaryToHex(b);
		C = binaryToHex(c);
		D = binaryToHex(d);
		E = binaryToHex(e);
		fprintf(target,"%04d\tA%c-B%c-C%c-D%c-E%c\n",i,A,B,C,D,E);
	}
	fclose(target);
}

void entries(char* fileName,SymbolList* tablehead) {
	int found = 0;
	int baseAddress = 0, offset = 0; 
	FILE* target;
	char name[MAX_CHARS_LABEL];
	SymbolInfo* tableptr;
	tableptr = tablehead->head;
	strcat(fileName,".ent");
	while(tableptr != NULL) {
		if(tableptr->attributes[1] == 1) {
			found = 1;
		}
		tableptr = tableptr->nextSymbol;
	}
	tableptr = tablehead->head;
	if(found == 1) {
		target = fopen(fileName, "w");
		if (target == NULL)
		{
			printf("Error reading file\n");
			exit(0);
		}
		while(tableptr != NULL) {
			if(tableptr->attributes[1] == 1) {
				strcpy(name,tableptr->name);
				baseAddress = tableptr->baseAddress;
				offset = tableptr->offset;
				if(tableptr->nextSymbol != NULL) {
					fprintf(target,"%s,%04d,%04d\n",name,baseAddress,offset);
				}
				else {
					fprintf(target,"%s,%04d,%04d",name,baseAddress,offset);
				}
			}
			tableptr = tableptr->nextSymbol;
		}
		fclose(target);
	}
}

void externals (char* fileName,ExternalList* externalList) {
	int found = 0;
	int baseAddress = 0, offset = 0; 
	FILE* target;
	char name[MAX_CHARS_LABEL];
	char file[40]; 
	External*  externalsPtr;
	strcpy(file,fileName);
	strcat(file,".ext");
	externalsPtr = externalList->head;
	if(externalList->head != NULL) {
		found = 1;
	}
	if(found == 1) {
		target = fopen(file, "w");
		if (target == NULL || target == NULL)
		{
			printf("Error reading file\n");
			exit(0);
		}
		while(externalsPtr->nextExternal != NULL) {
				strcpy(name,externalsPtr->name);
				baseAddress = externalsPtr->base;
				offset = externalsPtr->offset;
				fprintf(target,"%s BASE %04d\n%s OFFSET %04d\n",name,baseAddress,name,offset);
				externalsPtr = externalsPtr->nextExternal;
		}
		fprintf(target,"\n%s BASE %04d\n%s OFFSET %04d",name,baseAddress,name,offset);
		fclose(target);
	}
}

void mainInstruction(char* fileName) {
	int IC = 0;
	int DC = 0;
	int OK = 1;
	int i = 0;
	int instructionRow = 100;
	int dataRow;
	int totalRows;
	char fileNameTemp[50]; 
	char** machineCode;
	SymbolList symbolList;
	ExternalList externalList;
	symbolListInit(&symbolList);
	externalListInit(&externalList);
	strcpy(fileNameTemp,fileName);
	strcat(fileNameTemp,".txt");
	firstTran(fileNameTemp,&IC,&DC,&OK,&symbolList);
	dataRow = instructionRow+IC;
	totalRows = (100+IC+DC);
 	machineCode = (char**) malloc(totalRows * sizeof(char*));
    for (i = 0; i < totalRows; i++) {
        machineCode[i] = (char*) malloc(21 * sizeof(char));
	}
	if(OK == 1) {
		secondTran(fileNameTemp, &symbolList, &externalList, &dataRow, &instructionRow, &OK,machineCode);
	}
	else {
		printf("ERROR: The file %s is not valid\n",fileNameTemp);
	}
	if(OK == 1) {
		strcpy(fileNameTemp,fileName);
		entries(fileNameTemp,&symbolList);
		strcpy(fileNameTemp,fileName);
		externals(fileNameTemp,&externalList);
		strcpy(fileNameTemp,fileName);
		object(IC,DC,fileNameTemp,machineCode);
	}
}





