
#include "OutputBuild.h"

/*This function makes the output files of our code. it makes the object file and the
 * entris and externals files if extern and entry exist*/

void outputOperation(linkedCodeList *headBinary, linkedSymbolTable *headSymbol,
                     linkedDataTable *headData,linkedExtlList *headExt, int ICF, int DCF,
                     char *name) {

    char *nameOB;
    char *nameENT;
    char *nameEXT;
    FILE *outputOB;
    FILE *outputENT;
    FILE *outputEXT;

    linkedCodeList *tempCodeList;
    linkedDataTable *tempDataTable;
    linkedSymbolTable *tempSymbolTable;
    linkedExtlList *tempExtList;

    nameOB = strtok(name,".");
    nameOB = strcat(nameOB,".ob");
    outputOB = fopen(nameOB,"wb+");


    /*entering the title of the object file*/
    fprintf(outputOB,"    %d %d   \n",ICF-100,DCF);
    /*writing the object file*/
    for (tempCodeList=headBinary;tempCodeList!=NULL;tempCodeList = tempCodeList->next){
        fprintf(outputOB,"%04d %03X %s\n",tempCodeList->ic,tempCodeList->binaryCode,
               tempCodeList->ARE);
    }
    for (tempDataTable=headData;tempDataTable!=NULL;tempDataTable = tempDataTable->next){
        fprintf(outputOB,"%04d %03X %s\n",tempDataTable->dc+ICF,tempDataTable->value,
                tempDataTable->ARE);
    }
    fclose(outputOB);

    /*check if we need to write the entries file. if yes we write it.*/
    for(tempSymbolTable = headSymbol;tempSymbolTable!=NULL;tempSymbolTable =
            tempSymbolTable->next){
        if (tempSymbolTable->IsEntry ==1){
            nameENT = strtok(nameOB,".");
            nameENT = strcat(name,".ent");
            outputENT = fopen(nameENT,"wb+");
            for(tempSymbolTable=tempSymbolTable;tempSymbolTable!=NULL;tempSymbolTable = tempSymbolTable
                    ->next){
                if (tempSymbolTable->IsEntry==1){
                    fprintf(outputENT,"%s %04d\n",tempSymbolTable->name,
                            tempSymbolTable->value);
                }
            }
            fclose(outputENT);
            break;
        }
    }
    /*if the external exist, we write the externals file*/
    if (headExt!=NULL){
        nameEXT = strtok(name,".");
        nameEXT = strcat(name,".ext");
        outputEXT = fopen(nameEXT,"wb+");
        for (tempExtList = headExt;tempExtList!=NULL;tempExtList = tempExtList->next){
            fprintf(outputEXT,"%s %04d\n",tempExtList->name,tempExtList->ic);
        }
        fclose(outputEXT);
    }
    freeCode(headBinary);
    freeSymbol(headSymbol);
    freeData(headData);
    freeExt(headExt);

}
/*The function free the linked list of the code list via the head of the binary linked
 * list*/
void freeCode(linkedCodeList *headBinary){
    linkedCodeList *tempCode;
    while(headBinary!=NULL){
        tempCode = headBinary;
        headBinary = headBinary->next;
        tempCode->next = NULL;
        tempCode->ARE = NULL;
        tempCode->binaryCode = 0;
        tempCode->ic = 0;
        free(tempCode);
    }
}

/*This function free the linked list of the symbol table via the head of the symbol
 * linked list*/
void freeSymbol(linkedSymbolTable *headSymbol) {
    linkedSymbolTable *tempSymbol;
    while(headSymbol!=NULL){
        tempSymbol = headSymbol;
        headSymbol = headSymbol->next;
        tempSymbol->next = NULL;
        tempSymbol->name = NULL;
        tempSymbol->IsEntry = 0;
        tempSymbol->value = 0;
        tempSymbol->attribute = NULL;
        free(tempSymbol);
    }
}
/*This function free the linked list of the data table via the head of the data linked
 * list*/
void freeData(linkedDataTable *headData) {
    linkedDataTable *tempData;
    while(headData!=NULL){
        tempData = headData;
        headData = headData->next;
        tempData->value = 0;
        tempData->next = NULL;
        tempData->dc = 0;
        tempData->ARE = NULL;
        free(tempData);
    }
}

/*This function free the linked list of the external table via the head of the externals
 * linked list*/
void freeExt(linkedExtlList *headExt) {
    linkedExtlList *tempExt;
    while (headExt!=NULL){
        tempExt = headExt;
        headExt = headExt->next;
        tempExt->next = NULL;
        tempExt->name = NULL;
        tempExt->ic = 0;
        free(tempExt);
    }
}
