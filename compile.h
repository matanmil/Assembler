
#ifndef ASSEMBLER_FIRST_H
#define ASSEMBLER_FIRST_H
#include "system.h"
#include "OutputBuild.h"



int firstLevelAssembler(lineOfCode *file, int numOfLine,char *name);
int secondLevelAssembler(lineOfCode *file, int numOfLine,char *name);
void clearanceLines();
char *labelExtractor(lineOfCode *lineOfFile,char *nameOfFile);
void parsingGuidance(lineOfCode *lineOfFile, char *label,char *name);
linkedSymbolTable *updateSymbolTable(char *name, int ic, char *feature);
void extractData(lineOfCode *lineOfFile,char *nameOfFile);
void extractString(lineOfCode *lineOfFile,char *nameOfFile);
void extractExternal(lineOfCode *lineOfFile,char *nameOfFile);
void parsingInstruction(lineOfCode *lineOfFile,char *nameOfFile);
int isDataStringExtern(lineOfCode *lineOfFile);
void addEntry(lineOfCode *lineOfFile,char *NameOfFile);
linkedCodeList *completeBinary(lineOfCode *lineOfFile,char *nameOfFile);



#endif /*ASSEMBLER_FIRST_H*/
