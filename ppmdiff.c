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
#include <compress40.h>
#include <pnm.h>
#include <a2methods.h>
#include <assert.h>


static Pnm_ppm read_from_input(char *input_file_name, A2Methods_T methods);

int main(int argc, char *argv[]) {
        
        (void)read_from_input;
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
        FILE *fp = stdin;
        if (input_file_name != NULL) {
                fp = fopen(input_file_name, "r");
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


void compare_image_size(Pnm_ppm image1, Pnm_ppm image2)
{

}

