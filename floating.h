/*
 * blockPack.h
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Allows a user to convert the values of a pixel from RGB values to video
 * component or from video component to RGB.
 */

#ifndef FLOATING_H
#define FLOATING_H

#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef A2Methods_UArray2 A2;

/*A struct to hold the video component values of a pixel*/
struct vidComp {
        float y;
        float pb;
        float pr;
};

A2 RGBtoVC(A2 pixels, A2Methods_T methods, int denominator);
A2 VCtoRGB(A2 vComp, A2Methods_T methods, int denominator);

#endif