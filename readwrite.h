//TODO: File contract


#ifndef READWRITE_H
#define READWRITE_H

#include "bitpack.h"
#include "a2methods.h"
#include "a2plain.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void printCodeWords(A2Methods_UArray2 codeWords, A2Methods_T methods);
A2Methods_UArray2 readCompressed(FILE *input, A2Methods_T methods);


#endif