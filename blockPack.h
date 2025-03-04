#ifndef BLOCKPACK_H
#define BLOCKPACK_H

#include "floating.h"
#include "arith40.h"


A2 pack2by2(A2 vComp, A2Methods_T methods);
A2 unpack2by2(A2 packed, A2Methods_T methods);
A2 encode(A2 packArr, A2Methods_T methods);
A2 decode(A2 codeWords, A2Methods_T methods);

#endif