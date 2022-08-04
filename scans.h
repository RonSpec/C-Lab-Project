#ifndef SCANS_H
#define SCANS_H
int isNumber(char* s); /* returns 1 if given word is int, else 0*/ 
int numberOfParameters(char* line); /*returns number of paramters in a "string" (not including whitespaces)*/
int numberOfParametersCommas(char* line); /*returns number of paramters in a "string" (not including whitespaces and commas)*/
char* whitespaceRemover(char* line); /*deletes extra whitespaces*/
char* firstWord(char* line); /*returns the first word of the given line*/
char* nextWord(char* line); /* returns the --ADDRESS-- of the next word in the line - if exists  -, when seperated with whitespaces */
int isRegister(char* word); /*checks if a word is a register, for example r03,returns 1 when its a register*/
int addressingModes(char* operand); /*checks what is the addressing mode of the given operand, returns -1 if it isn't one of them*/
/*gets an *amount* digits array (not including '\0') and a number, and turns the array to the number's binary form*/ 
void intToBinaryCharArray(int number, int startIndex, int endIndex, int rowInCode,char** machineCode); 
/*translates a binary char to an int number, for example converts 0101 to 5*/ 
int binaryCharArrayToInt(int startIndex,char** array,int row);
/*converts an int to a binary char*/
char binaryToHex(int number);
#endif /* !SCANS_H */

