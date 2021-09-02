
#include "Input.h"

/*The function gets the name and opening mode of the file, then allocating memory for
 * the lines and the content of them, then make the first level of the assembler. when
 * succeed, it makes the second level and return the original file*/

FILE *InputAllocator(char *name, char *mode){
    FILE *fileInput;
    lineOfCode *numLines,*token1;
    int i=0;
    int k;
    int perfectionFirstLevel;
    int counterInput = BUF;


    fileInput = fopen(name,mode);
    if (fileInput == NULL){
        fprintf(stderr, "FAILED - there is no file\n");
        exit(0);

    }
   /*Allocating memory for the file*/
   numLines = malloc(sizeof(lineOfCode)*BUF);
   numLines[i].line = malloc(sizeof(char)*INPUT);
   while (fgets(numLines[i].line,INPUT,fileInput)){
       if (i == counterInput){
           counterInput += INPUT;
            token1 = realloc(numLines, sizeof(lineOfCode)*counterInput);
            if (token1){
                numLines = token1;
            }
            else{
                printf("memry allocation failed!\n");
                exit(0);
            }
       }
       numLines[i].complete = 0;
       numLines[i].labelDefinition = 0;
       numLines[i].numOfLine = i+1;
       numLines[++i].line = malloc(sizeof(char)*INPUT);
   }
   free (numLines[i].line);
   for(k=0;k<i;k++) {
        numLines[k].backupline = numLines[k].line;
    }


   /*starting the first level of the assembler*/
    perfectionFirstLevel = firstLevelAssembler(numLines,i,name);
   if (perfectionFirstLevel!=1){
       for(k=0;k<i;k++) {
           numLines[k].line = numLines[k].backupline;
       }
       secondLevelAssembler(numLines,i,name);
   }

   for(k=0;k<i;k++){
       free(numLines[k].backupline);
   }
   for(k=0;k<i;k++){
       numLines[k].line = NULL;
       numLines[k].numOfLine = 0;
       numLines[k].backupline = NULL;
       numLines[k].labelDefinition = 0;
       numLines[k].complete = 0;
       numLines[k].targetAddressing = 0;
       numLines[k].sourceAddressing = 0;
       numLines[k].numOfoperands = 0;
   }

    clearanceLines();
    free(numLines);
    fclose(fileInput);


    return fileInput;
}

