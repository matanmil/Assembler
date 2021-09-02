#include "system.h"

#ifndef ASSEMBLER_OUTPUTBUILD_H
#define ASSEMBLER_OUTPUTBUILD_H

void outputOperation(linkedCodeList *headBinary,linkedSymbolTable *headSymbol,
        linkedDataTable *headData,linkedExtlList *headExt,
        int ICF, int DCF, char *name);

void freeCode(linkedCodeList *headBinary);
void freeSymbol(linkedSymbolTable *headSymbol);
void freeData(linkedDataTable *headData);
void freeExt(linkedExtlList *headExt);

#endif
