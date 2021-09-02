
#ifndef ASSEMBLER_SYSTEM_H
#define ASSEMBLER_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct symbolList{
    struct symbolList *next;
    char *name;
    int value;
    char *attribute;
    int IsEntry;
}linkedSymbolTable;

typedef struct dataTable{
    struct dataTable *next;
    unsigned int value;
    int dc;
    char *ARE;
}linkedDataTable;


typedef struct codeList{
    struct codeList *next;
    unsigned int binaryCode;
    int ic;
    char *ARE;
}linkedCodeList;

typedef struct externalList{
    struct externalList *next;
    unsigned int ic;
    char *name;
}linkedExtlList;



/*line structure with relevant fields*/
typedef struct sline {
    char *line;
    char *backupline;
    int labelDefinition;
    int numOfoperands;
    unsigned int numOfLine;
    unsigned  int sourceAddressing;
    unsigned  int targetAddressing;
    unsigned int complete;
}lineOfCode;


unsigned int numToLine(unsigned int num);
unsigned int charToLine(char c);
int correctEntryExtern(linkedSymbolTable *headSymbol,linkedExtlList *headExt,char
*nameOfFile);

#endif /*ASSEMBLER_SYSTEM_H*/
