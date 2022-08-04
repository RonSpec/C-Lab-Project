#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "transitions.h"
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_LINES 6
int isNumber(char* s) /* returns 1 if given word is int, else 0*/
{ 
	int i = 0;	
	int length = strlen(s);
	if(length == 0) {
		return 0;
	}
	for(i = 0;i < length; i++) {
		if (!isdigit(s[i])) {
			    return 0; 
		}
	}
    return 1; 
}

int numberOfParameters(char* line) {  /*returns number of paramters in a "string" (not including whitespaces)*/
	int i = 0;
	int parameters = 0;
	int word = 0;
	while ((line[i] != '\0') && (line[i] != '\n')) {
		while ((line[i] != '\0') && (line[i] != '\n') && (line[i] == ' ')) {
			i++;
		}
		while ((line[i] != '\0') && (line[i] != '\n') && (line[i] != ' ')) {
			if (word == 0) {
				parameters++;
				word = 1;
			}
			i++;
		}
		word = 0;
	}
	return parameters;
}

int numberOfParametersCommas(char* line) {  /*returns number of paramters in a "string" (not including whitespaces and commas)*/
	int i = 0;
	int parameters = 0;
	int word = 0;
	while ((line[i] != '\0') && (line[i] != '\n')) {
		while ((line[i] != '\0') && (line[i] != '\n') && ((line[i] == ' ') || (line[i] == ','))) {
			i++;
		}
		while ((line[i] != '\0') && (line[i] != '\n') && (line[i] != ' ') && (line[i] != ',')) {
			if (word == 0) {
				parameters++;
				word = 1;
			}
			i++;
		}
		word = 0;
	}
	return parameters;
}
char* whitespaceRemover(char* line) { /*deletes extra whitespaces*/
	int i = 0;
	int j = 0;
	int spaces = 0; /*in case there is more than one whitespace in a row*/
	char* removed = (char*)malloc((strlen(line) + 1) * sizeof(char));
	/*cleaning the whitespaces in the beggining of the line*/
	while ((line[i] != '\0') && (line[i] == ' ')) {
		i++;
	}
	/*cleaning the rest of the extra whitespaces in the rest of the line*/
	while ((line[i] != '\0') && (line[i] != '\n')) {
		if ((spaces == 1) && (line[i] == ' ')) {
			i++;
		}
		else {
			if (line[i] == ' ') {
				removed[j] = line[i];
				spaces = 1;
			}
			if (line[i] != ' ') {
				removed[j] = line[i];
				spaces = 0;
			}
			i++;
			j++;
		}
	}
	removed = realloc(removed, (j + 1) * sizeof(char)); /*minimizing used space*/
	removed[j] = '\0';
	return removed;
}

/*returns the first word of the given line (we presume that the line is not empty)*/
char* firstWord(char* line) { 
	int i = 0;
	int j = 0;
	char* firstword;
	while((line[i] != '\0') && (line[i] == ' ')) {
		i++;
	}
	firstword = (char*)malloc(MAX_CHARS * sizeof(char));
	while((line[i] != '\0') && (line[i] != '\n') && (line[i] != ' ')) {
		firstword[j] = line[i];
		i++;
		j++;
	}
	firstword[j] = '\0';
	return firstword;
}

/* returns the --ADDRESS-- of the next word in the line - if exists  -, when seperated with whitespaces */
char* nextWord(char* line)  {
	int i = 0;
	while((line[i] != '\0') && (line[i] != ' ')) { /*finding first whitespace*/
		i++;
	}
	while ((line[i] != '\0') && (line[i] == ' ')) { /*finding first char of the next word*/
		i++;
	}
	return &(line[i]);
}

/*checks if a word is a register, for example r03*/
int isRegister(char* word) { 
	int equal = 0;
	if (word[0] == 'r') {
		if (word[2] == '\0') { /*r0,r1,...r9*/
			if(('0' <= word[1]) && (word[1] <= '9')) {
				equal = 1;
			}
		}
		else if ((word[3] == '\0') && (word[1] == '1')) {/*r10,r11,...r15*/
			if(('0' <= word[2]) && (word[2] <= '5')) {
				equal = 1;
			}
		}
	}
	if (equal == 1) {
		return 1;
	}
	return 0;
}

/*checks what is the addressing mode of the given operand, returns -1 if it isn't one of them*/
int addressingModes(char* operand) { 
	int i = 0;
	int j = 0;
	char* temp;
	char label[MAX_CHARS_LABEL];
	int labelFlag = 0;
	int numberFlag = 0;
	int directKey = labelCheck(operand);
	/*if it's an immediate addressing*/
	if (operand[0] == '#') {
		temp = firstWord(&operand[1]);
		if ((temp[0] == '+') || (temp[0] == '-')) {
			if (isNumber(&temp[1]) == 1) {
				numberFlag = 1;
			}
		}
		else {
			if (isNumber(temp) == 1) {
				numberFlag = 1;
			}
		}
		free(temp);
	}
	if (numberFlag == 1){
		return 0;
	}
	/*if it's a direct addressing*/
	if (directKey == 1) {
		return 1;
	}
	/*if it's an index addressing*/
	if ((strstr(operand, "[")) != NULL) { /*means there's a [ in the operand, which is good*/
		/*building the word before [, for example STR[r12] >>**build**>> STR*/
		while (operand[i] != '[') {
			label[j]  = operand[i];
			i++;
			j++;
		}
		label[i] = '\0';
		/*checking if the word before the char '[' is a legit label*/
		if (labelCheck(label) == 1) {
				labelFlag = 1;
		}
		/*label[i] = '['*/
		i++;
		j = 0;
		/*building the "register" before ], for example STR[r12] >>**build**>> r12*/
		while ((operand[i] != '\0') && (operand[i] != ']')) {
			label[j] = operand[i];
			i++;
			j++;
		}
		/*label[i] = ']' or '\0'*/
		label[j] = '\0';
		if ((operand[i] == ']') && (operand[i + 1] == '\0')) {
			/*check if the word between [ and ] is a register*/
			if ((labelFlag == 1) && (isRegister(label) == 1)) {
				return 2;
			}
		}
	}
	/*if its a register-direct addressing*/
	if (isRegister(operand) == 1) {
		return 3;
	}
	return -1;
}

/*gets a *end-start* digits array (not including '\0') and a number, and turns the first 16 digits of the array to the number's binary form*/ 
void intToBinaryCharArray(int number, int startIndex, int endIndex, int rowInCode,char** machineCode) {
	int one = 1; /*(0000|0000|0000|0001)*/
	int i = 0; 
	for(i = startIndex; i <= endIndex; i++) {
		if((number & one) == one) {
			machineCode[rowInCode][i] = '1';		
		}
		else {
			machineCode[rowInCode][i] = '0';
		}
		one <<= 1;
	}
}

int binaryCharArrayToInt(int startIndex,char** array,int row) {
	int cnt = 8; 
	int i = 0;
	int sum = 0;
	int endIndex = startIndex - 3;
	for(i = startIndex; i>= endIndex; i--) {
		if(array[row][i] == '1') {
			sum += cnt;
		}
		cnt = cnt/2;
	}
	return sum;
}

char binaryToHex(int number) {
	if(number == 0) {return '0';} if(number == 1) {return '1';} if(number == 2) {return '2';}
	if(number == 3) {return '3';} if(number == 4) {return '4';} if(number == 5) {return '5';}
	if(number == 6) {return '6';} if(number == 7) {return '7';} if(number == 8) {return '8';}
	if(number == 9) {return '9';} 
	if(number == 10) {return 'a';} if(number == 11) {return 'b';} if(number == 12) {return 'c';} 
	if(number == 13) {return 'd';} if(number == 14) {return 'e';}	if(number == 15) {return 'f';}
	return 'z'; 
}







