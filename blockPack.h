/*
 * blockPack.h
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Defines the public methods of blockPack: encode and decode, which allows 
 * the user to encode video component values of a 2-by-2 block of pixels
 * into 32-bit integer codeword or decode 32-bit integer codewords into video
 * component values of a pixel.
 */

#ifndef BLOCKPACK_H
#define BLOCKPACK_H

#include "floating.h"
#include "arith40.h"

A2 decode(A2 packed, A2Methods_T methods);
A2 encode(A2 packArr, A2Methods_T methods);

#endif