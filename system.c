#include "system.h"

/*The function convert the num to the binary line */
unsigned int numToLine(unsigned int num){
    unsigned int mask = 0xFFF;
    unsigned int token;
    token =0;
    if (num>=0){
        token = num&mask;
    }
    else{
        token = ((num*-1)+1)&mask;
    }
    return  token;
}
/*The function convert the char to the binary line */
unsigned int charToLine(char c) {
    unsigned int result;
    result = 0;
    result = c;
    return result;
}
/*The function checks if there is no entry and extern of the exact label in the same
file*/
int correctEntryExtern(linkedSymbolTable *headSymbol,linkedExtlList *headExt,char
*nameOfFile){
    linkedSymbolTable *tempSymbol;
    linkedExtlList *tempExt;
    for(tempSymbol = headSymbol;tempSymbol!=NULL;tempSymbol = tempSymbol->next){
        if (tempSymbol->IsEntry ==1){
            for(tempExt = headExt;tempExt!=NULL;tempExt = tempExt->next){
                if (strcmp(tempSymbol->name,tempExt->name)==0){
                    fprintf(stderr,"ERROR, The symbol cant be both entry and extern!\n");
                    printf("Symbol : %s  File: %s\n",tempSymbol->name,nameOfFile);
                    return 1;
                }
            }
        }
    }
    return 0;
}
