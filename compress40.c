/*
* compress40.c
* by Rigoberto Rodriguez-Anton (rrodri08), Rebecca Lee (rlee19)
* arith
*
* Implements the compress and decompress implementaion, which compresses an 
* image using lossy compression.
*/

#include "compress40.h"
#include "floating.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "blockPack.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*A struct to hold the dimensions and pixels of a trimmed pixel raster
  that will be used to replace the original pixel raster*/
struct trimInfo {
        A2 oldPixels;
        A2Methods_T methods;
};

 /* applyTrim
  * 
  * Apply function used to copy the raster of one image into a smaller uarray2.
  * 
  * Parameters
  *     int col         the columns of the new raster
  *     int row         the rows of the new raster
  *     A2 uarray2      the new raster with trimmed height and/or width
  *     void *element   a pointer to an index in the new raster where a pixel
  *                     from the old raster will be copied to
  *     void *cl        a pointer to the old raster with untrimmed height/width
  *
  * Returns
  *     None (void)
  * 
  * Notes
  *     May CRE if element or cl are NULL.
  *
  */
void applyTrim(int col, int row, A2 uarray2, void *element, void *cl)
{
        (void)uarray2;
        assert(element != NULL);
        assert(cl != NULL);
        struct trimInfo oldInfo = *(struct trimInfo *) cl;
        A2Methods_T methods = oldInfo.methods;
        A2 oldArray = oldInfo.oldPixels;

        *(struct Pnm_rgb *) element = 
                             *(struct Pnm_rgb *)methods->at(oldArray, col, row);
}


 /* trim
  * 
  * Trims the height and/or width of a PPM image so that the resulting image has
  * even height and width.
  * 
  * Parameters
  *      Pnm_ppm image  an image that needs to be trimmed
  *
  * Returns
  *      None (void)
  *
  * Notes
  *      Will CRE if image is NULL.
  *      No effect on image if both height and width are even. In the case where
  *      no trimming is needed, a pointer to the original image is returned. 
  *      
  */
void trim(Pnm_ppm *image, A2Methods_T methods)
{
        assert(image != NULL);
        int width = (*image)->width;
        int height = (*image)->height;
        bool sizechange = false;
        if ((*image)->width & 1) {
                width = (*image)->width - 1;
                sizechange = true;
        }
        if ((*image)->height & 1) {
                height = (*image)->height - 1;
                sizechange = true;
        }
        
        if (sizechange == true)
        {
                A2 newVelociraster = 
                            methods->new(width, height, sizeof(struct Pnm_rgb));
                struct trimInfo trimInfo;
                trimInfo.oldPixels = (*image)->pixels;
                trimInfo.methods = methods;
                
                methods->map_default(newVelociraster, applyTrim, &trimInfo);
                methods->free(&(*image)->pixels);
                (*image)->pixels = newVelociraster;
                (*image)->width = width;
                (*image)->height = height;
        }
}

 /* compress40
  * 
  * TODO: Description
  * 
  * Parameters
  *      
  *
  * Returns
  *      None (void)
  *
  * Notes
  *      
  *      
  */
extern void compress40(FILE *input)
{
        assert(input != NULL);
        /* default to UArray2 methods */
        /* TODO: decide whether we need other methods*/
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        /* Reading the given image */
        Pnm_ppm image = Pnm_ppmread(input, methods);

        /* trimmng the image to an even height and/or width */
        trim(&image, methods);

        /* converting the image to video component */
        A2 vComp = RGBtoVC(image->pixels, methods, image->denominator);
        methods->free(&(image->pixels));

        /* prepare video component values to be packed into a 32-bit word */
        A2 pack = pack2by2(vComp, methods);
        methods->free(&vComp);
        
        /* TODO: Remove testing functions here.*/
        A2 unpack = unpack2by2(pack, methods);
        methods->free(&pack);
        A2 rgb = VCtoRGB(unpack, methods, image->denominator);
        methods->free(&unpack);
        image->pixels = rgb;
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}

extern void decompress40(FILE *input)
{
        assert(input != NULL);
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        Pnm_ppm image = Pnm_ppmread(input, methods);
        Pnm_ppmwrite(stdout, image);
        Pnm_ppmfree(&image);
}

