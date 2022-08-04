#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "instructions.h"
#include "scans.h"
#include "transitions.h"
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_LINES 6


int legitFunc(char* instruction) { /*checks if the given func is one of the 16 legit functions (mov,cmp,add,...,stop)) and returns it's required number of operands*/
	if((strcmp(instruction,"mov") == 0) || (strcmp(instruction,"cmp") == 0) || (strcmp(instruction,"add") == 0) || (strcmp(instruction,"sub") == 0) || (strcmp(instruction,"lea") == 0)) {
		return 1;
	}
	if((strcmp(instruction,"clr") == 0) || (strcmp(instruction,"not") == 0) || (strcmp(instruction,"inc") == 0) || (strcmp(instruction,"dec") == 0) || (strcmp(instruction,"jmp") == 0)) {
		return 1;
	}
	if((strcmp(instruction,"bne") == 0) || (strcmp(instruction,"jsr") == 0) || (strcmp(instruction,"red") == 0) || (strcmp(instruction,"prn") == 0)) {
		return 1;
	}
	if((strcmp(instruction,"rts") == 0) || (strcmp(instruction,"stop") == 0)) {
		return 1;
	}
	/*in case that the func is not legit*/
	return -1; 
}
int operandsNum(char* instruction) {
	if((strcmp(instruction,"mov") == 0) || (strcmp(instruction,"cmp") == 0) || (strcmp(instruction,"add") == 0) || (strcmp(instruction,"sub") == 0) || (strcmp(instruction,"lea") == 0)) {
		return 2;
	}
	if((strcmp(instruction,"clr") == 0) || (strcmp(instruction,"not") == 0) || (strcmp(instruction,"inc") == 0) || (strcmp(instruction,"dec") == 0) || (strcmp(instruction,"jmp") == 0)) {
		return 1;
	}
	if((strcmp(instruction,"bne") == 0) || (strcmp(instruction,"jsr") == 0) || (strcmp(instruction,"red") == 0) || (strcmp(instruction,"prn") == 0)) {
		return 1;
	}	
	if((strcmp(instruction,"rts") == 0) || (strcmp(instruction,"stop") == 0)) {
		return 0;
	}
	return -1;
}

int opCode(char* instruction) {
	if(strcmp(instruction,"mov") == 0) {
		return 0;
	}
	if(strcmp(instruction,"cmp") == 0) {
		return 1;
	}
	if((strcmp(instruction,"add") == 0) || (strcmp(instruction,"sub") == 0)) {
		return 2;
	}
	if(strcmp(instruction,"lea") == 0) {
		return 4;
	}
	if((strcmp(instruction,"clr") == 0) || (strcmp(instruction,"not") == 0) || (strcmp(instruction,"inc") == 0) || (strcmp(instruction,"dec") == 0)) {
		return 5;
	}
	if((strcmp(instruction,"jmp") == 0) || (strcmp(instruction,"bne") == 0) || (strcmp(instruction,"jsr") == 0)) {
		return 9;
	}
	if(strcmp(instruction,"red") == 0) {
		return 12;
	}
	if(strcmp(instruction,"prn") == 0) {
		return 13;
	}
	if(strcmp(instruction,"rts") == 0) {
		return 14;
	}
	if(strcmp(instruction,"stop") == 0) {
		return 15;
	}
	return -1;
}
int funct(char* instruction) {
	if((strcmp(instruction,"add") == 0) || (strcmp(instruction,"clr") == 0) || (strcmp(instruction,"jmp") == 0)) {
		return 10;
	}
	if((strcmp(instruction,"sub") == 0) || (strcmp(instruction,"not") == 0) || (strcmp(instruction,"bne") == 0)) {
		return 11;
	}
	if((strcmp(instruction,"inc") == 0) || (strcmp(instruction,"jsr") == 0)) {
		return 12;
	}
	if(strcmp(instruction,"dec") == 0) {
		return 13;
	}
	return 0;
}


int sourceMethod(char* instruction,int sMethod) { 
	if((strcmp(instruction,"mov") == 0) || (strcmp(instruction,"cmp") == 0)|| (strcmp(instruction,"add") == 0) || (strcmp(instruction,"sub") == 0)) {
		if((0 <= sMethod) && (sMethod <= 3)) {
			return 1;
		}
			return 0;
	}
	if(strcmp(instruction,"lea") == 0) {
		if((1 <= sMethod) && (sMethod <= 2)) {
			return 1;
		}
		return 0;
	}
	if((strcmp(instruction,"clr") == 0) || (strcmp(instruction,"not") == 0) || (strcmp(instruction,"inc") == 0) || (strcmp(instruction,"dec") == 0)) {
		if(sMethod == 0) {
			return 1;
		}
		return 0;
	}
	if((strcmp(instruction,"jmp") == 0) || (strcmp(instruction,"bne") == 0) || (strcmp(instruction,"jsr") == 0) || (strcmp(instruction,"red") == 0)) {
		if(sMethod == 0) {
			return 1;
		}
		return 0;
	}
	if((strcmp(instruction,"prn") == 0) || (strcmp(instruction,"rts") == 0) || (strcmp(instruction,"stop") == 0)) {
		if(sMethod == 0) {
			return 1;
		}
		return 0;
	}
	return -1;
}
	
int destinationMethod(char* instruction,int dMethod) { /*addressing method = {0,0,0,0} (0,1,2,3), 1 - allowed, 0 - not allowed*/
	if((strcmp(instruction,"cmp") == 0) || (strcmp(instruction,"prn") == 0)){
		if((0 <= dMethod) && (dMethod <= 3)) {
			return 1;
		}
			return 0;
	}
	if((strcmp(instruction,"mov") == 0) || (strcmp(instruction,"add") == 0) || (strcmp(instruction,"sub") == 0) || (strcmp(instruction,"lea") == 0)) {
		if((1 <= dMethod) && (dMethod <= 3)) {
			return 1;
		}
		return 0;
	}
	if((strcmp(instruction,"clr") == 0) || (strcmp(instruction,"not") == 0) || (strcmp(instruction,"inc") == 0) || (strcmp(instruction,"dec") == 0) || 
		(strcmp(instruction,"red") == 0)) {
		if((1 <= dMethod) && (dMethod <= 3)) {
			return 1;
		}
		return 0;
	}
	if((strcmp(instruction,"jmp") == 0) || (strcmp(instruction,"bne") == 0) || (strcmp(instruction,"jsr") == 0)) {
		if((1 <= dMethod) && (dMethod <= 2)) {
			return 1;
		}
		return 0;
	}
	if((strcmp(instruction,"rts") == 0) || (strcmp(instruction,"stop") == 0)) {
		if(dMethod == 0) {
			return 1;
		}
		return 0;
	}
	return -1;
}

void operandPick(char* line, char* sourceOperand, char* destinationOperand) {
	int numofparameters = 0;
	int runningCnt = 0;
	int cnt = 0;
	char temp[32];
	char* operand;
	operand = firstWord(line);
	numofparameters = numberOfParametersCommas(line);
	if(numofparameters == 0) {
		strcpy(sourceOperand,"!NULL");
		strcpy(destinationOperand,"!NULL");
	}
	if(numofparameters == 1) {
		strcpy(sourceOperand,"!NULL");
		strcpy(destinationOperand,operand);
	}
	free(operand);
	if(numofparameters == 2) {
		/*getting the sourceOperand*/
		while((line[runningCnt] != ',') && (line[runningCnt] != ' ')) {
			temp[cnt] = line[runningCnt];
			cnt++;
			runningCnt++;
		}
		temp[cnt] = '\0';
		strcpy(sourceOperand,temp);
		/*getting the destinationOperand*/
		while(line[runningCnt] != ',') {
			runningCnt++;
		} 
		runningCnt++;
		operand = firstWord(&line[runningCnt]);
		strcpy(destinationOperand,operand);
		free(operand);
	}
}

char* labelExtract(char* operand) {
	int runningCnt = 0;
	char* label = (char*) malloc (MAX_CHARS_LABEL*sizeof(char));
	while(operand[runningCnt] != '[') {
		label[runningCnt] = operand[runningCnt];
		runningCnt++;
	}
	label[runningCnt] = '\0';
	return label;
}

int registerExtract(char* operand) {
	int runningCnt = 0;
	int cnt = 0;
	int regis = 0;
	char* reg = (char*) malloc (MAX_CHARS_LABEL*sizeof(char)); /*register*/
	while(operand[runningCnt] != '[') {
		runningCnt++;
	}
	runningCnt += 2; /* skipping '[' and 'r' */
	while(operand[runningCnt] != ']') {
		reg[cnt] = operand[runningCnt];
		runningCnt++;
		cnt++;
	}
	reg[cnt] = '\0';	
	regis = atoi(reg);
	free(reg);
	return regis;
}

/* decodes the first word in an instruction */
void firstWordDecoder(int opcode, int* instructionRow, char** machineCode) {
	int i = 0;
	for(i = 0; i<20; i++) {
		machineCode[*instructionRow][i] = '0';
	}
		machineCode[*instructionRow][opcode] = '1';
		machineCode[*instructionRow][18] = '1';
		machineCode[*instructionRow][20] = '\0';
	(*instructionRow)++;
}

void secondWordDecoder(char* sourceOperand,char* destinationOperand,int funct, int* instructionRow, char** machineCode) {
	int addressMethodSource = -1; /*adressing method*/
	int addressMethodDestination = -1;
	int regSource = 0;
	int regDestination = 0;	
	/*filling the "funct" area in the word (indexs 12-15) and putting A (index 18)*/
	intToBinaryCharArray(funct,12,15, *instructionRow,machineCode);
	intToBinaryCharArray(4,16,19, *instructionRow,machineCode);
	/*filling sourceOperand addressing method*/
	if(strcmp(sourceOperand,"!NULL") != 0) {
		addressMethodSource = addressingModes(sourceOperand);
		/*when there is a register in the source addressing*/
		if((addressMethodSource == 2) || (addressMethodSource == 3)) {
			if(addressMethodSource == 2) {
				regSource = registerExtract(sourceOperand);
			}
			if(addressMethodSource == 3) {
				regSource = atoi(&sourceOperand[1]);
			}
			intToBinaryCharArray(regSource,8,11, *instructionRow,machineCode);
		}
		/*there is no register in the source addressing*/
		else {
			intToBinaryCharArray(0,8,11, *instructionRow,machineCode);
		}
		intToBinaryCharArray(addressMethodSource,6,7, *instructionRow,machineCode);
	}
	/*in case there is no source operand, we fill the word's cells 6 to 11 with zeros*/
	if(strcmp(sourceOperand,"!NULL") == 0) {
		intToBinaryCharArray(0,6,11, *instructionRow,machineCode);
	}
	if(strcmp(destinationOperand,"!NULL") != 0) {
		addressMethodDestination = addressingModes(destinationOperand);
		/*when there is a register in the destination addressing*/
		if((addressMethodDestination == 2) || (addressMethodDestination == 3)) {
			if(addressMethodDestination == 2) {
				regDestination = registerExtract(destinationOperand);
			}
			if(addressMethodDestination == 3) {
				regDestination = atoi(&destinationOperand[1]);
			}
			intToBinaryCharArray(regDestination,2,5, *instructionRow,machineCode);
		}
		/*there is no register in the destination addressing*/
		else {
			intToBinaryCharArray(0,2,5, *instructionRow,machineCode);
		}
			intToBinaryCharArray(addressMethodDestination,0,1, *instructionRow,machineCode);
	}
	/*in case there is no destination operand, we fill the word's cells 0 to 5 with zeros*/
	if(strcmp(destinationOperand,"!NULL") == 0) {
		intToBinaryCharArray(0,0,5, *instructionRow,machineCode);
	}
	machineCode[*instructionRow][20] = '\0';	
	(*instructionRow)++;

}

void extraWordsDecoder(char* sourceOperand,char* destinationOperand,int* instructionRow, char** machineCode,SymbolList* symbolList,ExternalList* externalList) {
	int addressMethodSource;
	int addressMethodDestination;
	int sign = 1;
	int number = 0;
	int baseAddress = 0;
	int offset = 0;
	int attribute = 0;
	char* temp;
	SymbolInfo* tableptr;
	External* external;
	char label[MAX_CHARS_LABEL];
	if(strcmp(sourceOperand,"!NULL") != 0) {
		addressMethodSource = addressingModes(sourceOperand);
		/*Source: immediate addressing*/
		if(addressMethodSource == 0) {
			if(sourceOperand[1] == '-') {
				sign = -1;
				number = (atoi(&sourceOperand[2])*sign);
			}
			else {
				sign = 1;
				number = atoi(&sourceOperand[1]);
			}
			intToBinaryCharArray(number,0,15, *instructionRow,machineCode);
			intToBinaryCharArray(0,16,19, *instructionRow,machineCode);
			machineCode[*instructionRow][18] = '1';
			machineCode[*instructionRow][20] = '\0';
			(*instructionRow)++;
		}
		/*Source : direct or index addressing*/ 
		if((addressMethodSource == 1) || (addressMethodSource == 2)) {
			if(addressMethodSource == 1) {
				strcpy(label,sourceOperand);
			}
			else {
				temp = labelExtract(sourceOperand);
				strcpy(label,temp);
				free(temp);
			}
			tableptr = labelFind(label,symbolList);
			baseAddress = tableptr->baseAddress;
			offset = tableptr->offset;
			attribute = tableptr->attributes[2];
			intToBinaryCharArray(baseAddress,0,15, *instructionRow,machineCode);
			/*<<<<building Source: baseAddress word>>>>*/
			/*if the label is external*/
			if(attribute == 1) {
				intToBinaryCharArray(1,16,19, *instructionRow,machineCode);
				external = externalListAdd(externalList);
				strcpy(external->name,label);
				external->base = *instructionRow;
				external->offset = (*instructionRow+1);
			}
			/*if the label isn't external*/
			else {
				intToBinaryCharArray(2,16,19, *instructionRow,machineCode);
			}
			machineCode[*instructionRow][20] = '\0';
			(*instructionRow)++;			
			/*<<<<building Source: offset word>>>>*/	
			intToBinaryCharArray(offset,0,15, *instructionRow,machineCode);
			if(attribute == 1) {
				intToBinaryCharArray(1,16,19, *instructionRow,machineCode);
			}
			/*if the label isn't external*/
			else {
				intToBinaryCharArray(2,16,19, *instructionRow,machineCode);
			}
			machineCode[*instructionRow][20] = '\0';
			(*instructionRow)++;
		}
	}
	if(strcmp(destinationOperand,"!NULL") != 0) {
		addressMethodDestination = addressingModes(destinationOperand);
		/*Destination: immediate addressing*/
		if(addressMethodDestination == 0) {
			if(destinationOperand[1] == '-') {
				sign = -1;
				number = (atoi(&destinationOperand[2])*sign);
			}
			else {
				sign = 1;
				number = atoi(&destinationOperand[1]);
			}
			intToBinaryCharArray(number,0,15, *instructionRow,machineCode);
			intToBinaryCharArray(0,16,19, *instructionRow,machineCode);
			machineCode[*instructionRow][18] = '1';
			machineCode[*instructionRow][20] = '\0';
			(*instructionRow)++;
		}
		/*Destination : direct or index addressing*/ 
		if((addressMethodDestination == 1) || (addressMethodDestination == 2)) {
			if(addressMethodDestination == 1) {
				strcpy(label,destinationOperand);
			}
			else {
				temp = labelExtract(destinationOperand);
				strcpy(label,temp);
				free(temp);
			}
			tableptr = labelFind(label,symbolList);
			baseAddress = tableptr->baseAddress;
			offset = tableptr->offset;
			attribute = tableptr->attributes[2];
			intToBinaryCharArray(baseAddress,0,15, *instructionRow,machineCode);
			/*<<<<building Destination: baseAddress word>>>>*/
			/*if the label is external*/
			if(attribute == 1) {
				intToBinaryCharArray(1,16,19, *instructionRow,machineCode);
				external = externalListAdd(externalList);
				strcpy(external->name,label);
				external->base = *instructionRow;
				external->offset = (*instructionRow+1);
			}
			/*if the label isn't external*/
			else {
				intToBinaryCharArray(2,16,19, *instructionRow,machineCode);
			}
			machineCode[*instructionRow][20] = '\0';
			(*instructionRow)++;			
			/*<<<<building Destination: offset word>>>>*/	
			intToBinaryCharArray(offset,0,15, *instructionRow,machineCode);
			if(attribute == 1) {
				intToBinaryCharArray(1,16,19, *instructionRow,machineCode);
			}
			/*if the label isn't external*/
			else {
				intToBinaryCharArray(2,16,19, *instructionRow,machineCode);
			}
			machineCode[*instructionRow][20] = '\0';
			(*instructionRow)++;
		}
	}
}

