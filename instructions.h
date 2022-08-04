#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H
#define MAX_CHARS 82
#define MAX_CHARS_LABEL 31
#define MAX_CHARS_ATTRIBUTES 20
#define MAX_LINES 6
#include "transitions.h"

int legitFunc(char* instruction); /*checks if the given func is one of the 16 legit functions (mov,cmp,add,...,stop))*/
int operandsNum(char* instruction); /*gets a instruction and returns it's number of operands*/
int opCode(char* instruction); /*gets a instruction and returns it's opcode value*/
int funct(char* instruction); /*gets a instruction and returns it's func value*/
int sourceMethod(char* instruction,int sMethod); /*gets a instruction and checks if the source method is valid*/
int destinationMethod(char* instruction,int dMethod); /*gets a instruction and checks if the destination method is valid*/
void operandPick(char* line, char* sourceOperand, char* destinationOperand); /*recieves a line and updates 2 variables with the operands*/
char* labelExtract(char* operand); /*for addressing method #2*/
int registerExtract(char* operand); /*for addressing method #2*/
void firstWordDecoder(int opcode, int* instructionRow, char** machineCode); /* decodes the first word in an instruction */
/*decodes the second word in an instruction if exists */
void secondWordDecoder(char* sourceOperand,char* destinationOperand,int funct, int* instructionRow, char** machineCode);
/*decodes the extra words in an instruction if they exist*/
void extraWordsDecoder(char* sourceOperand,char* destinationOperand,int* instructionRow, char** machineCode,SymbolList* symbolList,ExternalList* externalList);
#endif /* !INSTRUCTIONS_H */

