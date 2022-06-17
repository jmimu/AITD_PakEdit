#ifndef _PAK_
#define _PAK_

#include "types.h"

char* loadPak(const char* name, int index);
int loadPakToPtr(const char* name, int index, char* ptr);
int getPakSize(const char* name, int index);
unsigned int PAK_getNumFiles(const char* name);
void dumpPak(const char* name);

struct pakInfoStruct // warning: alignement unsafe
{
    s32 discSize;
    s32 uncompressedSize;
    char compressionFlag;
    char info5;
    s16 offset;
};


void readPakInfo(pakInfoStruct* pPakInfo, FILE* fileHandle);

#endif
