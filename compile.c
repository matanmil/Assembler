
#include "compile.h"
#include "system.h"
struct{
    char *name;
    unsigned int value : 12 ;
}registerim[] = {{"r0",0},{"r1",1},{"r2",2},{"r3",3},{"r4",4},{"r5",5},
                 {"r6",6},{"r7",7},{NULL,0}};

struct operation{
    char *nameOperation;
     int opcode;
     int funct;
    int numOfOperands;
}operations[] = {{"mov",0,0,2},{"cmp",1,0,2},{"add",2,10,2},{"sub",2,11,2},
                 {"lea",4,0,2},
                 {"clr",5,10,1},{"not",5,11,1},{"inc",5,12,1},{"dec",5,13,1},{"jmp",9,
                                                                              10,1},
                 {"bne",9,11,1},{"jsr",9,12,1},{"red",12,0,1},{"prn",13,0,1},{"rts\n",
                                                                               14,0,0}
                 ,{"stop\n",15,0,0},{NULL,-1,-1,-1}};

static linkedDataTable *firstData;
static linkedDataTable *lastData;

static linkedCodeList *firstBinaryLine;
static linkedCodeList *lastBinaryLine;
static  linkedCodeList *currentCodeLine;

static linkedSymbolTable *firstSymbol;

static linkedExtlList *headExternal;

int errorFlag = 0;
int IC;
int DC;
int ICF,DCF;
int i;

/*The function gets the line of the code and search for label at the start of the line.
 * the function returns the label if it exist and llegal and NULL if not*/

char *labelExtractor(lineOfCode *lineOfFile,char *nameOfFile) {

    char *tmp = lineOfFile->line;
    char *backup = lineOfFile->line;
    while ((*lineOfFile->line)!= '\0'){

        if ((*lineOfFile->line) == ':'){
            *((lineOfFile->line)++) = '\0';
            /*Now we check if the symbol is llegal*/

            while (tmp){
                if ((*tmp == ' ') || (isspace(*tmp))!=0) {
                    fprintf(stderr,"ERROR, there is a space before the : \n");
                    printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                    errorFlag = 1;
                    lineOfFile->line = backup;
                    return NULL;
                }
                else if(*tmp == '\0'){
                    break;
                }
                tmp++;
            }
            tmp = backup;
           /* if ((*lineOfFile->line) != ' '){

            }*/
            for(i=0;i<8;i++){
                if (strcmp(tmp,registerim[i].name)==0){
                    fprintf(stderr,"ERROR, The symbol cant be one of the registers\n");
                    printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                    errorFlag = 1;
                    break;
                }
            }
            for(i=0;i<16;i++){
                if (strcmp(tmp,operations[i].nameOperation)==0){
                    fprintf(stderr,"ERROR, The symbol cant be one of the operations\n");
                    printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                    errorFlag = 1;
                    break;
                }
            }
            if (!isalpha(tmp[0])){
                fprintf(stderr,"ERROR, The symbol starts with non alphabetical char\n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag = 1;

            }
            return tmp;
        }
        (lineOfFile->line)++;
    }
    lineOfFile->line = tmp;
    return NULL;

}

/*This function search for the data storaging instruction type. the function search for
 * the type of the instruction type and if it's string or data it forward it for the
 * function that gets the string/data and update the symbol table if label exist*/
void parsingGuidance(lineOfCode *lineOfFile, char *label,char *nameOfFile) {

    char *feature = "data";
    if (strstr((*lineOfFile).line,".data")!=NULL){
        if (label){
            lineOfFile->labelDefinition = 1;
            updateSymbolTable(label,DC,feature);
        }
        extractData(lineOfFile,nameOfFile);
        lineOfFile->complete = 1;

    }
    else if (strstr((*lineOfFile).line,".string")!=NULL){
        if (label){
            lineOfFile->labelDefinition = 1;
            updateSymbolTable(label,DC,feature);
        }
        extractString(lineOfFile,nameOfFile);
        lineOfFile->complete = 1;
    }
    else if (strstr((*lineOfFile).line,".extern")!=NULL){
        extractExternal(lineOfFile,nameOfFile);
        lineOfFile->complete = 1;
    }
    else if (strstr((*lineOfFile).line,".entry")!=NULL){
        return;
    }
    else{
        fprintf(stderr,"ERROR, The symbol starts with non alphabetical char\n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
        lineOfFile->complete = 1;
    }


}

/*This function extracts the data from data instruction. it passes the ".data" and
 * check and takes all the numbers and updates the data table */
void extractData(lineOfCode *lineOfFile,char *nameOfFile){
    int num,sign;
    int checkingForNum = 0;
    static linkedDataTable *tempData;

    lineOfFile->line += (sizeof(".data")-1);
    if (!isspace(lineOfFile->line[0])){
        fprintf(stderr,"ERROR, it needs to be a space after the command \n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
        return;
    }

    while ((*lineOfFile).line[0] !='\n'){
        sign=0;
        while ((isspace(*lineOfFile[0].line))){
            (void)*(lineOfFile->line)++;
        }
            if(lineOfFile->line[0] == '-'){
                sign =-1;
                (lineOfFile->line)++;
            }
        if(lineOfFile->line[0] == '+'){
            sign =1;
            (lineOfFile->line)++;
        }
            /*we check if the data starts with number*/
            if (isdigit((*lineOfFile).line[0])==0){
                fprintf(stderr,"ERROR, it doesnt start with a number! \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag = 1;
                return;
            }
            else if (sign!=-1) {
                sign = 1;
            }
                num = 0;
                while (isdigit((*lineOfFile).line[0])) {
                    num *= 10;
                    num += ((*lineOfFile).line[0]) - '0';
                    (lineOfFile->line)++;
                }
                /*now we insert the data into our data image*/
                if (firstData == NULL) {
                    firstData = (linkedDataTable *) malloc(sizeof(linkedDataTable));
                    firstData->next = NULL;
                    firstData->value = numToLine(sign*num);
                    firstData->dc = DC;
                    firstData->ARE = "A";
                    lastData =firstData;
                    DC++;

                }
                else if (lastData != NULL) {
                tempData = lastData;
                lastData = (linkedDataTable *) malloc(sizeof(linkedDataTable));
                lastData->next = NULL;
                lastData->value = numToLine( sign*num);
                lastData->dc = DC;
                lastData->ARE = "A";
                tempData->next = lastData;
                DC++;
                 }

            /*checking for the next numbers*/
            while ((isspace(*lineOfFile[0].line))&& *lineOfFile[0].line!= '\n'){
                lineOfFile->line++;
            }
            if (lineOfFile->line[0] == ','){
                checkingForNum = 1;
                (lineOfFile->line)++;
            }
            else{
                checkingForNum =0;
            }
    }
    if (checkingForNum == 1){
        fprintf(stderr,"ERROR, we don't have the next number after the comma! \n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
    }
}

/*This function extracts the data from string instruction. it passes the ".string" and
 * check and takes the string and updates the data table */

void extractString(lineOfCode *lineOfFile,char *nameOfFile) {
    static linkedDataTable *tempData;

    lineOfFile->line += (sizeof(".string")-1);
    /*checks if there any space after the command*/
    if (!isspace(lineOfFile->line[0])){
        fprintf(stderr,"ERROR, it needs to be a space after the command \n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
        return;
    }
    while ((isspace(*lineOfFile[0].line))) {
        (void) *(lineOfFile->line)++;
    }
        /*We need to check if it starts with "*/
        if (!(lineOfFile->line[0]== '\"')){
            fprintf(stderr,"ERROR, it needs to start with apostrophes \n");
            printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
            errorFlag = 1;
            return;
        }
        (lineOfFile->line)++;

        while ((lineOfFile->line[0]!='\"') &&(lineOfFile->line[0]!='\0' )){
            if (firstData ==NULL){
                firstData = (linkedDataTable *) malloc(sizeof(linkedDataTable));
                firstData->next = NULL;
                firstData->value = charToLine(lineOfFile->line[0]);
                firstData->dc = DC;
                firstData->ARE ="A";
                lastData = firstData;
                DC++;
                (lineOfFile->line)++;
            }
            else if (lastData!=NULL){
                tempData = lastData;
                lastData = (linkedDataTable *) malloc(sizeof(linkedDataTable));
                lastData->next=NULL;
                lastData->value = charToLine(lineOfFile->line[0]);
                lastData->dc = DC;
                lastData->ARE = "A";
                tempData->next = lastData;
                DC++;
                (lineOfFile->line)++;
            }

        }
        if (lineOfFile->line[0]=='\"'){
            tempData = lastData;
            lastData = (linkedDataTable *) malloc(sizeof(linkedDataTable));
            lastData->next = NULL;
            lastData->value = charToLine(0);
            lastData->dc = DC;
            lastData->ARE = "A";
            tempData->next = lastData;
            DC++;
        }
        else{
            fprintf(stderr,"ERROR, it needs to end with apostrophes \n");
            printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
            errorFlag = 1;
            return;
        }

    }

/*The function gets the label from externak instruction and updates the symbol table*/

void extractExternal(lineOfCode *lineOfFile,char *nameOfFile) {
    char *feature = "external";
    lineOfFile->line += (sizeof(".extern")-1);
    lineOfFile->line = strtok(lineOfFile->line, " \t\n");
    if (!lineOfFile->line){
        fprintf(stderr,"ERROR, there is no extern! \n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
        return;
    }
    updateSymbolTable(lineOfFile->line, 0,feature);
    lineOfFile->line = strtok(NULL, " \t\n");
    if (lineOfFile->line){
        fprintf(stderr,"ERROR, there is more than one extern! \n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
    }

}

/*The function updates the symbol table via the name,value and feature it has.
 * The function checks if the symbol is already exist and then updates the symbol table*/

linkedSymbolTable *updateSymbolTable(char *name,int value,char *feature){
    static linkedSymbolTable *lastSymbol;
    static linkedSymbolTable *tempSymbol;

    if (firstSymbol==NULL){
        firstSymbol = (linkedSymbolTable*) malloc(sizeof(linkedSymbolTable));
        firstSymbol->next = NULL;
        firstSymbol->name = name;
        firstSymbol->value = value;
        firstSymbol->attribute = feature;
        firstSymbol->IsEntry =0;
        lastSymbol = firstSymbol;
        return firstSymbol;
    }
    else if  (lastSymbol!=NULL){
        for(tempSymbol = firstSymbol;tempSymbol!=NULL;tempSymbol = tempSymbol->next){

            if (strcmp(name,tempSymbol->name)==0){
                printf("ERROR, there is a label with this name already : %s \n",
                        name);
                errorFlag = 1;
                return tempSymbol;
            }
        }
    }

    /*if not exist, we create new symbol line for out table*/
    tempSymbol = lastSymbol;
    lastSymbol = (linkedSymbolTable *) malloc(sizeof(linkedSymbolTable));
    tempSymbol->next = lastSymbol;
    lastSymbol->name = name;
    lastSymbol->next = NULL;
    lastSymbol->value = value;
    lastSymbol->attribute = feature;
    lastSymbol->IsEntry =0;

    return lastSymbol;
}

/*The function has a line of code which is command line. it search for the command and
 * the llegal operands the need to come after it. It also makes a binary code line for
 * every operand (for the label we makes the line without code)*/

void parsingInstruction(lineOfCode *lineOfFile,char *nameOfFile) {
    unsigned int argument;
    unsigned int num =0;
    unsigned int L = 0; /*this variable counts how words needed for code machine*/
    int AreTwoOperands;
    linkedCodeList *tempCodeLine =NULL;
    linkedCodeList *operationCodeLine;
    struct operation *tempOperation;
    char tmpLine[82];

    strcpy(tmpLine,lineOfFile->line);
    lineOfFile->line = strtok(lineOfFile->line," ");
    for(i=0;operations[i].nameOperation!=NULL;i++){
        if(strcmp(lineOfFile->line,operations[i].nameOperation)==0){
            tempOperation = &operations[i];
            lineOfFile->numOfoperands = operations[i].numOfOperands;
            break;
        }
    }
    /*check if it's ; or empty line*/
    if (operations[i].nameOperation ==NULL){
        if ((lineOfFile->line[0]== ';') || (lineOfFile->line[0]== '\n')){
            lineOfFile->complete = 1;
            return;
        }
        printf("ERROR, there is not a valid instruction\n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag =1;
        return;
    }
    /*insert the operation into the linked code list*/
    if(firstBinaryLine==NULL){
        operationCodeLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
        operationCodeLine->next =NULL;
        operationCodeLine->binaryCode = numToLine(0);
        operationCodeLine->ARE = "A";
        operationCodeLine->ic = IC;
        firstBinaryLine = operationCodeLine;
        lastBinaryLine = firstBinaryLine;
        tempCodeLine = firstBinaryLine;
        L++;
    }
    else{
        tempCodeLine = lastBinaryLine;
        lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
        lastBinaryLine->next = NULL;
        lastBinaryLine->binaryCode = numToLine(0);
        lastBinaryLine->ARE = "A";
        lastBinaryLine->ic = IC;
        tempCodeLine->next = lastBinaryLine;
        operationCodeLine = lastBinaryLine;
        L++;

    }
     /*now we will check for every command*/
    if (tempOperation->numOfOperands==2){
        /*getting to the next value and then get it until the ","*/
        while ((isspace(*lineOfFile[0].line)) && (*lineOfFile[0].line != '\n')){
            (*lineOfFile[0].line)++;
        }

        /*searching for the first object in the string*/

        lineOfFile->line = strtok(NULL," ,\t");
        /*check if it's a number*/
        if(lineOfFile->line[0]=='#'){
            (lineOfFile->line)++;
            if ((isdigit((lineOfFile->line[0]))==0) && (lineOfFile->line[0]!= '-') &&
                (lineOfFile->line[0]!= '+')){
                printf("ERROR, there is not a number after the # \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag =1;
                free(tempOperation);
                return;
            }
            else{
                num = strtod(lineOfFile->line,&lineOfFile->line);/*getting the num*/
                /*updating our code image table*/
                tempCodeLine =lastBinaryLine;
                lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                lastBinaryLine->next=NULL;
                lastBinaryLine->ARE = "A";
                lastBinaryLine->ic = IC+L;
                lastBinaryLine->binaryCode = numToLine(num);
                tempCodeLine->next = lastBinaryLine;

                lineOfFile->sourceAddressing = 0;
            }

        }
        /*check if its a register or a label*/
        /*lineOfFile->line = strtok(lineOfFile->line,",");*/
        /*checking if its a register*/
        else {
            for (i = 0; registerim[i].name != NULL; i++) {
                if (strcmp(lineOfFile->line, registerim[i].name) == 0) {
                    tempCodeLine = lastBinaryLine;
                    lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                    lastBinaryLine->next = NULL;
                    lastBinaryLine->binaryCode = numToLine(1<<registerim[i].value);
                    lastBinaryLine->ARE = "A";
                    lastBinaryLine->ic = IC + L;
                    tempCodeLine->next = lastBinaryLine;
                    lineOfFile->sourceAddressing = 3;
                    break;

                }
            }

            if (registerim[i].name == NULL) {
                /*it's a label, we insert it in the code list table*/
                tempCodeLine = lastBinaryLine;
                lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                lastBinaryLine->next = NULL;
                lastBinaryLine->ic = IC + L;
                lastBinaryLine->ARE = "unknown";
                lastBinaryLine->binaryCode = numToLine(0);
                tempCodeLine->next = lastBinaryLine;
                lineOfFile->sourceAddressing = 1;
            }
        }

        L++;
        /*now we need the second value of the string*/
        lineOfFile->line = strtok(NULL," \n");
        if (lineOfFile->line ==NULL){
            printf("ERROR, there is not a second value \n");
            printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
            errorFlag =1;
            return;
        }
        while (((isspace(*lineOfFile[0].line)) && (*lineOfFile[0].line != '\n')) ||
                (*lineOfFile[0].line == ',')){
            (void)*(lineOfFile[0].line)++;
        }
        /*check if it's a number*/
        if(lineOfFile->line[0]=='#'){
            (lineOfFile->line)++;
            if ((isdigit((lineOfFile->line[0]))==0) && (lineOfFile->line[0]!= '-') &&
            (lineOfFile->line[0]!= '+')) {
                    fprintf(stderr,"ERROR, there is not a number after the # \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                    errorFlag =1;
                    return;
            }
            else{
                num = strtod(lineOfFile->line,&lineOfFile->line);/*getting the num*/
                /*updating our code image table*/
                tempCodeLine =lastBinaryLine;
                lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                lastBinaryLine->next=NULL;
                lastBinaryLine->ARE = "A";
                lastBinaryLine->ic = IC+L;
                lastBinaryLine->binaryCode = numToLine(num);
                tempCodeLine->next = lastBinaryLine;

                lineOfFile->targetAddressing = 0;
            }

        }
            /*check if its a register or a label*/
            /*checking if its a register*/
        else {
            for (i = 0; registerim[i].name != NULL; i++) {
                if (strcmp(lineOfFile->line, registerim[i].name) == 0) {
                    tempCodeLine = lastBinaryLine;
                    lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                    lastBinaryLine->next = NULL;
                    lastBinaryLine->binaryCode = numToLine(1<<registerim[i].value);
                    lastBinaryLine->ARE = "A";
                    lastBinaryLine->ic = IC + L;
                    tempCodeLine->next = lastBinaryLine;
                    lineOfFile->targetAddressing = 3;
                    break;

                }
            }

            if (registerim[i].name == NULL) {
                /*it's a label, we insert it in the code list table*/
                tempCodeLine = lastBinaryLine;
                lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                lastBinaryLine->next = NULL;
                lastBinaryLine->ic = IC + L;
                lastBinaryLine->ARE = "unknown";
                lastBinaryLine->binaryCode = numToLine(0);
                tempCodeLine->next = lastBinaryLine;
                lineOfFile->targetAddressing = 1;
            }
        }

        L++;
    }
    else if (tempOperation->numOfOperands==1){
        lineOfFile->sourceAddressing =0;
        while ((isspace(*lineOfFile[0].line)) && (*lineOfFile[0].line != '\n')){
            (*lineOfFile[0].line)++;
        }
        lineOfFile->line = strtok(NULL," ");
        /*first we check if it's has the % value*/
        if(lineOfFile->line[0]=='%'){
            (lineOfFile->line)++;
            if (tempOperation->opcode != 9){
                printf("ERROR, there is not a valid opcode for the percent "
                               "symbol \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag =1;

                return;
            }
            if ((lineOfFile->line[0]=='-') || lineOfFile->line[0]=='+' || isdigit
            (lineOfFile->line[0])==1){
                printf("ERROR, there is not a valid label for the percent symbol\n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag =1;
                return;
            }
                /*it's a label*/
            else{
                tempCodeLine = lastBinaryLine;
                lastBinaryLine = (linkedCodeList *)malloc(sizeof(linkedCodeList));
                lastBinaryLine->next = NULL;
                lastBinaryLine->ic = IC+L;
                lastBinaryLine->ARE = "A";
                tempCodeLine->next = lastBinaryLine;
                lineOfFile->targetAddressing = 2;

            }

        }
        else if(lineOfFile->line[0]=='#'){
            (lineOfFile->line)++;
            if ((isdigit((lineOfFile->line[0]))==0) && (lineOfFile->line[0]!= '-') &&
                (lineOfFile->line[0]!= '+')){
                fprintf(stderr,"ERROR, there is not a number after the # \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag =1;

                return;
            }
            else{
                num = strtod(lineOfFile->line,&lineOfFile->line);/*getting the num*/
                /*updating our code image table*/
                tempCodeLine =lastBinaryLine;
                lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                lastBinaryLine->next=NULL;
                lastBinaryLine->ARE = "A";
                lastBinaryLine->ic = IC+L;
                lastBinaryLine->binaryCode = numToLine(num);
                tempCodeLine->next = lastBinaryLine;

                lineOfFile->targetAddressing = 0;
            }

        }

        else {
            strtok(lineOfFile->line,"\n");
            for (i = 0; registerim[i].name != NULL; i++) {
                if (strcmp(lineOfFile->line, registerim[i].name) == 0) {
                    tempCodeLine = lastBinaryLine;
                    lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                    lastBinaryLine->next = NULL;
                    lastBinaryLine->binaryCode = numToLine(1<<registerim[i].value);
                    lastBinaryLine->ARE = "A";
                    lastBinaryLine->ic = IC + L;
                    tempCodeLine->next = lastBinaryLine;
                    lineOfFile->targetAddressing = 3;
                    break;

                }
            }

            if (registerim[i].name == NULL) {
                /*it's a label, we insert it in the code list table*/
                tempCodeLine = lastBinaryLine;
                lastBinaryLine = (linkedCodeList *) malloc(sizeof(linkedCodeList));
                lastBinaryLine->next = NULL;
                lastBinaryLine->ic = IC + L;
                lastBinaryLine->ARE = "unknown";
                lastBinaryLine->binaryCode = numToLine(0);
                tempCodeLine->next = lastBinaryLine;
                lineOfFile->targetAddressing = 1;
            }
        }
        L++;

        AreTwoOperands = strlen(lineOfFile->line);
        lineOfFile->line = strtok(lineOfFile->line," ,");
        if (lineOfFile->line!=NULL){
            if (strlen(lineOfFile->line )!=AreTwoOperands){
                fprintf(stderr,"ERROR, there is a second value \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag =1;
                return;
            }
        }


    }
    else if (tempOperation->numOfOperands==0){
        lineOfFile->sourceAddressing=0;
        lineOfFile->targetAddressing =0;


    }
    /*updating the binary code of the argument by the data*/
     argument=0;
    argument |=tempOperation->opcode<<8;
    argument|= tempOperation->funct<<4;
    argument|= lineOfFile->sourceAddressing<<2;
    argument|= lineOfFile->targetAddressing;
    operationCodeLine->binaryCode = numToLine(argument);
    IC = IC+L;
}
/*The function starts the first level of the assembler.It checks if there is any label
 * and send the line to the suitable function of it's classify, then updates the symbol
 * table when everything is o.k.*/
int firstLevelAssembler(lineOfCode *file, int numOfLine,char *name) {
    linkedSymbolTable *tempSymbol = NULL;
    int j;
    char *label;
    char *feature = "code";
    errorFlag = 0;
    IC =100;
    DC = 0;

    for(j=0;j<numOfLine;j++){
        /*checking if there is a label in the line*/
        label = labelExtractor(file + j,name);
        /*cheking if it's instruction for data (".data" or ".string etc."*/
        while ((isspace(*file[j].line)) && (*file[j].line != '\n')){
            (void)*(file[j].line)++;
        }
        if (file[j].line[0] == '.'){
            parsingGuidance(&file[j],label,name);
        }
        else {
            if(label){
                file[j].labelDefinition = 1;
                updateSymbolTable(label,IC,feature);
            }
            parsingInstruction(&file[j],name);

        }

    }
    if (errorFlag ==1){
        freeCode(firstBinaryLine);
        freeSymbol(firstSymbol);
        freeData(firstData);
        freeExt(headExternal);
        return 1;
    }
    ICF = IC;
    DCF = DC;
    /*updating symbol table*/
    for(tempSymbol = firstSymbol;tempSymbol!=NULL;tempSymbol = tempSymbol->next){
        if (strstr(tempSymbol->attribute,"data")!=NULL){
            tempSymbol->value = tempSymbol->value + ICF;
        }
    }

    return 0;
}

/*This function starts the second level of the assembler. if a line needs to be complete,
 * is send it to completeBinary. If succeed, it sends to the output operation*/
int secondLevelAssembler(lineOfCode *file, int numOfLine,char *name) {

     int j;
     errorFlag = 0;


     for(j=0;j<numOfLine;j++){


         if (file[j].labelDefinition == 1){
             strtok(file[j].line, " \t");
             while ((*file[j].line != '\0')){
                 (file[j].line)++;
             }
             (file[j].line)++;
         }
         while ((isspace(*file[j].line)) && (*file[j].line != '\n')){
             (file[j].line)++;
         }
         if (isDataStringExtern(file +j)==1){
             continue;
         }
         if (isDataStringExtern(file +j)==2){
             addEntry(&file[j],name);
             continue;
         }
         /*checks if it's empty line or ;*/
         if (file[j].complete==1){
             continue;
         }
         /*it's an instruction line. now we need to complete the binary code*/

          completeBinary(&file[j],name);

     }
     if ((correctEntryExtern(firstSymbol,headExternal, name))==1){
         return  1;
     }
     if (errorFlag == 1){
         return 1;
     }
     else{
        outputOperation(firstBinaryLine,firstSymbol,firstData,headExternal,
                ICF,DCF,name);
     }



    return 0;
}

/*This function updates the entry filed of the symbol if it has an entry guidance*/
void addEntry(lineOfCode *lineOfFile,char *nameOfFile){
    linkedSymbolTable *tempSymbol;
    lineOfFile->line += (sizeof(".entry")-1);
    while ((isspace(*lineOfFile[0].line))) {
        lineOfFile->line++;
    }
    lineOfFile->line = strtok(lineOfFile->line,"\n");
    for(tempSymbol=firstSymbol;tempSymbol!=NULL;tempSymbol=tempSymbol->next){
        if (strcmp(lineOfFile->line,tempSymbol->name)==0){
            tempSymbol->IsEntry = 1;
            break;
        }
    }
    if (tempSymbol == NULL){
        fprintf(stderr,"ERROR, the label doesnt exist! : \n");
        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
        errorFlag = 1;
    }
}
/*this function checks which guidaince line we have during to the rules of the second
 * level*/

int isDataStringExtern(lineOfCode *lineOfFile) {
    if (strstr((*lineOfFile).line,".data")!=NULL) {
        return 1;
    }
    if (strstr((*lineOfFile).line,".string")!=NULL){
        return 1;
    }
    if (strstr((*lineOfFile).line,".extern")!=NULL){
        return 1;
    }
    if (strstr((*lineOfFile).line,".entry")!=NULL) {
        return 2;
    }
    return 0;
}

/*This function completes the binary line of the code line if needed. also, if we have
 * external label it updates it in the external table*/

linkedCodeList *completeBinary(lineOfCode *lineOfFile,char *nameOfFile) {
    linkedSymbolTable *tokenSymbol;

    static linkedExtlList *tmpExternal;

    if (currentCodeLine==NULL){
        currentCodeLine = firstBinaryLine;
    }
    /*skiping the operation*/
    while (isalpha(*(lineOfFile[0].line)) ){
        (void)*(lineOfFile[0].line)++;
    }
    (void)*(lineOfFile[0].line)++;
    while (isspace(*(lineOfFile[0].line)) ){
        lineOfFile[0].line++;
    }
    strtok(lineOfFile->line," ");
    if (lineOfFile->numOfoperands==2){
        currentCodeLine = currentCodeLine->next;
        if (strcmp(currentCodeLine->ARE,"unknown")==0 ){
            for(tokenSymbol=firstSymbol;tokenSymbol!=NULL;tokenSymbol=tokenSymbol->next){
                if (strcmp(lineOfFile->line,tokenSymbol->name)==0){
                    if (strcmp(tokenSymbol->attribute,"external")==0){
                        currentCodeLine->ARE = "E";
                         if (headExternal==NULL){
                            headExternal = (linkedExtlList*)malloc(sizeof
                                    (linkedExtlList));
                            headExternal->name = tokenSymbol->name;
                            headExternal->ic = currentCodeLine->ic;
                            headExternal->next = NULL;
                            tmpExternal = headExternal;
                        }
                        else{
                            tmpExternal->next = (linkedExtlList*)malloc(sizeof
                                    (linkedExtlList));
                            tmpExternal->next->name = tokenSymbol->name;
                            tmpExternal->next->ic = currentCodeLine->ic;
                             tmpExternal->next->next = NULL;
                            tmpExternal = tmpExternal->next;

                        }
                    }
                    else{
                        currentCodeLine->ARE = "R";
                    }
                    currentCodeLine->binaryCode = numToLine(tokenSymbol->value);
                    break;
                }
            }
            if (tokenSymbol==NULL){
                fprintf(stderr,"ERROR, the label doesnt exist! : \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag = 1;
            }
        }
        /*cheking the next value*/
        currentCodeLine = currentCodeLine->next;
        while (strcmp(lineOfFile->line,"")!=0 ){
            (void)*(lineOfFile[0].line)++;
        }
        (void)*(lineOfFile[0].line)++;
        while (isspace(*(lineOfFile[0].line)) || (strcmp(lineOfFile->line,"")==0)){
            (void)*(lineOfFile[0].line)++;
        }

        while (((isspace(*lineOfFile[0].line)) && (*lineOfFile[0].line != '\n')) ||
               (*lineOfFile[0].line == ',')){
            (void)*(lineOfFile[0].line)++;
        }

        if (strcmp(currentCodeLine->ARE,"unknown")==0 ){
            for(tokenSymbol=firstSymbol;tokenSymbol!=NULL;tokenSymbol=tokenSymbol->next){
                if (strcmp(lineOfFile->line,tokenSymbol->name)==0){
                    if (strcmp(tokenSymbol->attribute,"external")==0){
                        currentCodeLine->ARE = "E";
                        if (headExternal==NULL){
                            headExternal = (linkedExtlList*)malloc(sizeof
                                    (linkedExtlList));
                            headExternal->name = tokenSymbol->name;
                            headExternal->ic = currentCodeLine->ic;
                            headExternal->next = NULL;
                            tmpExternal = headExternal;
                        }
                        else{
                            tmpExternal->next = (linkedExtlList*)malloc(sizeof
                                    (linkedExtlList));
                            tmpExternal->next->name = tokenSymbol->name;
                            tmpExternal->next->ic = currentCodeLine->ic;
                            tmpExternal->next->next = NULL;
                            tmpExternal = tmpExternal->next;

                        }
                    }
                    else{
                        currentCodeLine->ARE = "R";
                    }
                    currentCodeLine->binaryCode = numToLine(tokenSymbol->value);

                    break;
                }
            }
            if (tokenSymbol==NULL){
                fprintf(stderr,"ERROR, the label doesnt exist! : \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag = 1;
            }
        }
        currentCodeLine = currentCodeLine->next;

    }
    else if (lineOfFile->numOfoperands ==1){
        currentCodeLine = currentCodeLine->next;
        while ((isspace(*lineOfFile[0].line)) && (*lineOfFile[0].line != '\n')){
            (*lineOfFile[i].line)++;
        }
        /*if it's operand %*/
        if (lineOfFile->targetAddressing ==2){
            (lineOfFile->line)++;/*skip the %*/
            strtok(lineOfFile->line,"\n");
            for(tokenSymbol=firstSymbol;tokenSymbol!=NULL;tokenSymbol=tokenSymbol->next){
                if (strcmp(lineOfFile->line,tokenSymbol->name)==0){
                    if(strcmp(tokenSymbol->attribute,"external")==0 ){
                        fprintf(stderr,"ERROR, external cannot be with percent char : "
                                       "\n");
                        printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                        errorFlag = 1;
                        break;
                    }
                    currentCodeLine->binaryCode = numToLine(tokenSymbol->value -
                            currentCodeLine->ic);

                    break;
                }
            }
            if (tokenSymbol==NULL){
                fprintf(stderr,"ERROR, the label doesnt exist! : \n");
                printf("Line : %d\n\n",lineOfFile->numOfLine);

                errorFlag = 1;
            }

        }
        if (strcmp(currentCodeLine->ARE,"unknown")==0 ){
            for(tokenSymbol=firstSymbol;tokenSymbol!=NULL;tokenSymbol=tokenSymbol->next){
                if (strcmp(lineOfFile->line,tokenSymbol->name)==0){
                    if (strcmp(tokenSymbol->attribute,"external")==0){
                        currentCodeLine->ARE = "E";
                        if (headExternal==NULL){
                            headExternal = (linkedExtlList*)malloc(sizeof
                                    (linkedExtlList));
                            headExternal->name = tokenSymbol->name;
                            headExternal->ic = currentCodeLine->ic;
                            headExternal->next = NULL;
                            tmpExternal = headExternal;
                        }
                        else{
                            tmpExternal->next = (linkedExtlList*)malloc(sizeof
                                    (linkedExtlList));
                            tmpExternal->next->name = tokenSymbol->name;
                            tmpExternal->next->ic = currentCodeLine->ic;
                            tmpExternal->next->next = NULL;
                            tmpExternal = tmpExternal->next;

                        }
                    }
                    else{
                        currentCodeLine->ARE = "R";
                    }
                    currentCodeLine->binaryCode = numToLine(tokenSymbol->value);

                    break;
                }
            }
            if (tokenSymbol==NULL){
                fprintf(stderr,"ERROR, the label doesnt exist! : \n");
                printf("Line : %d  File: %s\n",lineOfFile->numOfLine,nameOfFile);
                errorFlag = 1;
            }

        }
        currentCodeLine =currentCodeLine->next;


    }
    else if (lineOfFile->numOfoperands == 0){
        return currentCodeLine->next;
    }
    return currentCodeLine;

}

/*This function clears the variables for the next file*/

void clearanceLines() {
    firstData = NULL;
    lastData = NULL;
    firstBinaryLine = NULL;
    lastBinaryLine = NULL;
    currentCodeLine = NULL;
    firstSymbol = NULL;
    headExternal = NULL;
}
