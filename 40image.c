/*
 * 40image.c
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Compresses or decompresses an image provided by the user
 *
 * Usage: `./40image.c [-c|-d] [filename]`
 *
 * Providing a filename is optional. If it is not provided, `40image.c` reads
 * from standard input instead.
 *
 * The given input must contain a single portable pixmap image (PPM).
 * If it does not, terminates with a CRE.
 *
 * If it does contain a valid image, when run with the flag `-c`, `40image.c`
 * will print the compressed image to `stdout`. When run with the flag `-d`, 
 * `40image.c` will print the decompressed image to `stdout`.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

/* C standard library */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* CS 40 */
#include "assert.h"

/* Student-written */
#include "compress40.h"

/*******************************************************************************
 * Functions
 ******************************************************************************/
static void (*compress_or_decompress)(FILE *input) = compress40;

/* main
 *
 * Entry point for the 40image.c program; checks if the given command-line
 * argument flags are valid and opens the file to be read if one is provided.
 *
 * Parameters
 *      int argc        (number of command-line arguments, including the
 *                      program name)
 *      char **argv     (array of strings representing command-line arguments)
 *
 * Returns
 *      int             0 (EXIT_SUCCESS) if input is valid
 *                      1 (EXIT_FAILURE) otherwise
 *
 * Notes
 *      Will CRE if more than 1 file is provided.
 *      May CRE if an error occurs while opening or reading the file.
 *      Will CRE if the provided input is not in the form of a PPM.
 */
int main(int argc, char *argv[])
{
        int i;
        
        /* Checks valid command line usage */
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */

        /* Passes input to compress_or_decompress */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}
