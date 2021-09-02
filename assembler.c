#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Input.h"
#define LENGTH 100



int main(int argc, char *argv[]) {
    int i;
    char fileName[LENGTH];


    if (argc<2){
        fprintf(stderr,"ERROR, there is no file entered\n");
    }
    for(i=1;i<argc;i++){

        strcpy(fileName,argv[i]);
        strcat(fileName,".as");
        InputAllocator(fileName,"r+");
    }
    return 0;
}