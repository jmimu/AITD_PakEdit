#ifndef ALONEBODY_H
#define ALONEBODY_H

#include "vars.h"
#include "pak.h"
#include "types.h"

#include "alonefile.h"

sBody* createBodyFromPtr(void* ptr);
bool body2ply(AloneFile *file);

#endif // ALONEBODY_H
