/*
* ppmdiff.c
* by Rigoberto Rodriguez-Anton (rrodri08), Rebecca Lee (rlee19)
* arith
*
* Implements the ppmdiff program, which compares two images to get a
* quantitative estimate of how different they are
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compress40.h"
#include "pnm.h"
#include "a2methods.h"
#include "a2plain.h"
#include "assert.h"



typedef A2Methods_UArray2 A2;

struct dimensions {
        int width;
        int height;
};



static Pnm_ppm read_from_input(char *input_file_name, A2Methods_T methods);
static void compare_image(Pnm_ppm image1, Pnm_ppm image2);
float find_E(Pnm_ppm image1, Pnm_ppm image2, struct dimensions limits);


int main(int argc, char *argv[]) 
{
        
        assert(argc == 3);
        assert((strcmp(argv[1], "-") != 0) || (strcmp(argv[2], "-") != 0));
        Pnm_ppm image1 = read_from_input(argv[1], uarray2_methods_plain);
        Pnm_ppm image2 = read_from_input(argv[2], uarray2_methods_plain);
        compare_image(image1, image2);
        Pnm_ppmfree(&image1);
        Pnm_ppmfree(&image2);
}

/* read_from_input
 * 
 * Returns the single PPM image contained in the file specified.
 * If `input_file_name` is NULL, reads from standard input.
 *
 * Parameters
 *      char *input_file_name
 *                      the file name of a PPM image
 *      A2Methods_T methods
 *                      the methods suite with which to initialize the image's
 *                      pixel array
 *
 * Returns
 *      Pnm_ppm         a struct containing the image's pixel array, the
 *                      appropriate methods suite for the array, and
 *                      supporting image metadata
 *
 * Notes
 *      Prints a message to `stderr` and exits with code 1 (`EXIT_FAILURE`) if
 *              the file could not be opened.
 *      May CRE if the provided image is of an invalid format.
 *      Allocates heap memory for the returned `Pnm_ppm`. May CRE if allocation
 *              fails. It is the responsibility of the caller to free this
 *              memory using `Pnm_ppmfree()`.
 */
static Pnm_ppm read_from_input(char *input_file_name, A2Methods_T methods)
{
        assert(input_file_name != NULL);
        // assert(*input_file_name != NULL);
        FILE *fp = NULL;
        if ((strcmp(input_file_name, "-") != 0)) {
                fp = fopen(input_file_name, "r");
        } else {
                fp = stdin;
        }
 
        if (fp == NULL) {
                fprintf(stderr,
                        "Error: file %s could not be opened.\n",
                        input_file_name);
                exit(EXIT_FAILURE);
        }
 
        Pnm_ppm imagep = Pnm_ppmread(fp, methods);
 
        if (input_file_name != NULL) {
                fclose(fp);
        }
 
        return imagep;
}

/* compare_image
 * 
 * Asserts that the difference in width and height between the two images is at
 * at most 1 and prints an error message to standard error otherwise.
 *
 * Parameters
 *      Pnm_ppm image1
 *                      an image in the ppm format to be compared to another
                        image
 *      Pnm_ppm image2
 *                      an image in the ppm format to be compared to another
                        image
 *
 * Returns
 *      int             returns float 1.0 if the width or height of the images
 *                      differ by more than 1.
 *
 * Notes
 *      Prints a message to `stderr` and prints the number 1.0 to `stdout`
 */
static void compare_image(Pnm_ppm image1, Pnm_ppm image2)
{
        if ((abs((int)image1->width - (int)image2->width) > 1) || 
            (abs((int)image1->height - (int)image2->height) > 1)) {
                fprintf(stderr, "Image size differs by more than 1\n");
                fprintf(stdout, "1.0\n");
                exit(EXIT_FAILURE);
        }
        
        struct dimensions smaller;
        if ((image1->width > image2->width)) {
                smaller.width = image2->width;
        } else {
                smaller.width = image1->width;
        }

        if ((image1->height > image2->height)) {
                smaller.height = image2->height;
        } else {
                smaller.height = image1->height;
        }

        fprintf(stdout, "Diff is: %.4f\n", find_E(image1, image2, smaller));
}

/* find_E
 * 
 * Calculates the root mean square difference of the pixel values in two images
 * and prints it out to `stdout` with four digits after the decimal point.
 *
 * Parameters
 *      Pnm_ppm image1, image2
 *                      
 *                      images whose root mean square difference in pixel values
 *                      will be calculated
 *       
 *      struct dimensions
 *                      the struct containing the smaller width and height of 
 *                      the two images
 *
 * Returns
 *      float           the root mean square difference
 *
 * Notes
 *      Prints the root mean square difference (E) to `stdout` with four digits
 *      after the decimal point
 */
float find_E(Pnm_ppm image1, Pnm_ppm image2, struct dimensions limits)
{
        A2Methods_T methods = uarray2_methods_plain; 
        long msdNum = 0;


        for (int i = 0; i < limits.width; i++)
        {
                for (int j = 0; j < limits.height; j++)
                {
                        struct Pnm_rgb *rgb1 = 
                                              methods->at(image1->pixels, i, j);
                        struct Pnm_rgb *rgb2 = 
                                              methods->at(image2->pixels, i, j);

                        int dred = rgb1->red - rgb2->red;
                        int dgreen = rgb1->green - rgb2->green;
                        int dblue = rgb1->blue - rgb2->blue;

                        //fprintf(stderr, "[%d, %d, %d]\n", dred, dgreen, dblue);
                        //fprintf(stderr, "[%d, %d]\n", rgb1->red, rgb2->red);

                        int deltaMSDN = dred * dred;
                        deltaMSDN += dgreen * dgreen;
                        deltaMSDN += dblue * dblue;

                        msdNum += deltaMSDN;
                }
                
        }
        
        //fprintf(stderr, "width = %d, height = %d\n", limits.width, limits.height);
        double msdDenom = 3 * limits.width * limits.height * image1->denominator * image2->denominator;
        double msd = msdNum / msdDenom;
        double rmsd = sqrt(msd);
        //fprintf(stderr, "[msdNum, msdDenom, msd] : [%ld, %.0f, %f]\n", msdNum, msdDenom, msd);
        return rmsd;
}