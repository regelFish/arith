/*
* floating.c
* by Rigoberto Rodriguez-Anton (rrodri08), Rebecca Lee (rlee19)
* arith
*
* Implements functionality to convert rgb values with a given denominator into
* component video representation.
*/
#include "floating.h"


/* A struct to use in our apply functions */
struct mappingCl {
        A2 array;
        A2Methods_T methods;
        int denom;
};

 /* toFloat
  * 
  * Converts an integer RGB value into their floating-point representations
  * 
  * Parameters
  *     int num         an RGB value to be converted to floating-point
  *     int denom       the maxval of the PPM image whose pixels are being
  *                     converted (will be used as denominator to divide num)
  *
  * Returns
  *     float           the floating-point representation of inputted num
  * 
  * Notes
  *     None
  *
  */
float toFloat(int num, int denom)
{
        return (float)num / (float)denom;
}

 /* unFloat
  * 
  * Converts an integer RGB value into their floating-point representations
  * 
  * Parameters
  *     float num       a float video component value to be converted into an
  *                     integer
  *     int denom       the maxval of the PPM image whose pixels are being
  *                     converted (will be used to multiply num)
  *
  * Returns
  *     int             the integer representation of num
  * 
  * Notes
  *     None
  *
  */
int unFloat(float num, int denom)
{
        return num * denom;
}

 /* toVideoComponent
  * 
  * Converts a pixel's RGB values into their floating-point representations
  * 
  * Parameters
  *     struct Pnm_rgb rgb      the Pnm_rgb struct containing rgb values for a
  *                             pixel
  *
  * Returns
  *     struct vidComp          a struct of video component values of the
  *                             converted pixel
  * 
  * Notes
  *     None
  *
  */
struct vidComp toVideoComponent(struct Pnm_rgb rgb, int denom)
{
        struct vidComp newVidComp;
        newVidComp.y =                  0.299 * toFloat(rgb.red, denom) + 
                                        0.587 * toFloat(rgb.green, denom) + 
                                        0.114 * toFloat(rgb.blue, denom);

        newVidComp.pb =                 -0.168736 * toFloat(rgb.red, denom) - 
                                        0.331264 * toFloat(rgb.green, denom) + 
                                        0.5 * toFloat(rgb.blue, denom);

        newVidComp.pr =                 0.5 * toFloat(rgb.red, denom) - 
                                        0.418688 * toFloat(rgb.green, denom) -
                                        0.081312 * toFloat(rgb.blue, denom);
        return newVidComp;
}

 /* toRGB
  * 
  * Converts a pixel's video component values into its rgb values
  * 
  * Parameters
  *     struct vidComp vComp    the video component values of a pixel that will
  *                             be converted into RGB values
  *     int denom               the maxval of the PPM image whose pixels are
  *                             being converted (will be used as to convert
  *                             floats into ints)
  *
  * Returns
  *     struct Pnm_rgb          a struct containing the converted rgb values of
  *                             the pixel
  * 
  * Notes
  *     None
  *
  */
struct Pnm_rgb toRGB(struct vidComp vComp, int denom)
{
        struct Pnm_rgb rgb;
        rgb.red = unFloat(1.0 * vComp.y + 
                        0.0 * vComp.pb + 
                        1.402 * vComp.pr, denom);
        rgb.green = unFloat(1.0 * vComp.y - 
                        0.344136 * vComp.pb - 
                        0.714136 * vComp.pr, denom);
        rgb.blue = unFloat(1.0 * vComp.y + 
                        1.772 * vComp.pb + 
                        0.0 * vComp.pr, denom);
        return rgb;
}

 /* applyRGBtoVC
  *   
  * Calls toVideoComponent to convert an RGB pixel to a video component pixel
  * 
  * Parameters
  *     int col         the column of the pixel to be converted
  *     int row         the row of the pixel to be converted
  *     A2 uarray2      the raster of pixels that will be converted
  *     void *element   a pointer to the RGB pixel that will be converted into
  *                     video component
  *     void *cl        a struct to hold the converted pixels and other
  *                     information needed for mapping such as methods
  *
  * Returns
  *     None (void)
  * 
  * Notes
  *     Will CRE if element is NULL.
  *     Will CRE if cl is NULL.
  *
  */
void applyRGBtoVC(int col, int row, A2 uarray2, void *element, void *cl)
{
        (void) uarray2;
        assert(element != NULL);
        assert(cl != NULL);
        struct mappingCl bundle = *(struct mappingCl *) cl;
        assert(bundle.methods != NULL);
        A2Methods_T methods = bundle.methods;
        A2 newArr = bundle.array;
        int denom = bundle.denom;

        *(struct vidComp *)methods->at(newArr, col, row) = 
                           toVideoComponent(*(struct Pnm_rgb *) element, denom);
}

 /* RGBtoVC
  * 
  * Takes a uarray2 with image data formated in rgb and returns a uarray2 
  * formated in video component.
  * 
  *     A2 pixels               : An array of image data formated in the rgb 
  *                               format.
  *     A2Methods_T methods     : The methods we will use to manipulate the 
  *                               uarray2.
  *     int denomintator        : The maximum value associated with our input
  *                               rgb data.
  *
  * Returns
  *     A2Methods_Uarray2: A Uarray2 with image data formated in the video 
  *                        component format.
  * 
  * Notes
  *     Will CRE if any of the following occur:
  *     pixels is NULL
  *     methods is NULL
  *     denominator is less than 1
  *
  */
 A2 RGBtoVC(A2 pixels, A2Methods_T methods, int denominator)
 {
         assert(pixels != NULL);
         assert(methods != NULL);
         assert(denominator > 0);
         int width = methods->width(pixels);
         int height = methods->height(pixels);
         A2 vComp = methods->new(width, height, sizeof(struct vidComp));
         struct mappingCl bundle;
         bundle.array = vComp;
         bundle.methods = methods;
         bundle.denom = denominator;
         methods->map_default(pixels, applyRGBtoVC, &bundle);
 
         return vComp;
 }

 /* applyVCtoRGB
  *   
  * Calls toRGB to convert a video component pixel into an RGB pixel
  * 
  * Parameters
  *     int col         the column of the pixel to be converted
  *     int row         the row of the pixel to be converted
  *     A2 uarray2      the raster of pixels that will be converted
  *     void *element   a pointer to the video component pixel that will be
  *                     converted into RGB
  *     void *cl        a struct to hold the converted pixels and other
  *                     information needed for mapping such as methods
  *
  * Returns
  *     None (void)
  * 
  * Notes
  *     Will CRE if element is NULL.
  *     Will CRE if cl is NULL.
  *
  */
void applyVCtoRGB(int col, int row, A2 uarray2, void *element, void *cl)
{
        (void) uarray2;
        assert(element != NULL);
        assert(cl != NULL);
        struct mappingCl bundle = *(struct mappingCl *) cl;
        assert(bundle.methods != NULL);
        A2Methods_T methods = bundle.methods;
        A2 newArr = bundle.array;
        int denom = bundle.denom;

        *(struct Pnm_rgb *)methods->at(newArr, col, row) =
                                      toRGB(*(struct vidComp *) element, denom);  
}

 /* VCtoRGB
  * 
  * Takes a uarray2 with image data formated in video component and returns a 
  * uarray2 formated in rgb.
  * 
  *     A2 vComp                : An array of image data formated in the video
  *                               component format.
  *     A2Methods_T methods     : The methods we will use to manipulate the 
  *                               uarray2.
  *     int denomintator        : The maximum value that our rgb array will take
  *                               on. 
  *
  * Returns
  *     A2Methods_Uarray2: A Uarray2 with image data formated in the rgb format.
  * 
  * Notes
  *     Will CRE if any of the following occur:
  *     vComp is NULL, methods is NULL, denominator is less than 1.
  *
  */
 A2 VCtoRGB(A2 vComp, A2Methods_T methods, int denominator)
 {
         assert(vComp != NULL);
         assert(methods != NULL);
         assert(denominator > 0);
         int width = methods->width(vComp);
         int height = methods->height(vComp);
         A2 pixels = methods->new(width, height, sizeof(struct Pnm_rgb));
         struct mappingCl bundle;
         bundle.array = pixels;
         bundle.methods = methods;
         bundle.denom = denominator;
         methods->map_default(vComp, applyVCtoRGB, &bundle);
 
         return pixels;
 }