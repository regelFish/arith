/*
* readwrite.c
* by Rigoberto Rodriguez-Anton (rrodri08), Rebecca Lee (rlee19)
* arith
*
* Implements functions for reading compressed files (codewords) and printing
* them to stdout.
*/
#include "readwrite.h"

/* applyPrintCodewords
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
void applyPrintCodewords(int col, int row, A2Methods_UArray2 uarray2, 
                                                        void *element, void *cl)
{
        (void) cl;
        (void) row;
        (void) col;
        (void) uarray2;
        assert(element != NULL);
        for (int i = 0; i < 4; i++)
        {
                // fprintf(stderr, "block.pack.a: %ld\n", Bitpack_getu(*(uint32_t *) element, 9, 23));
                putchar(Bitpack_getu(*(uint32_t *) element, 8, i << 3));
        }
}

/* printCodeWords
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
void printCodeWords(A2Methods_UArray2 codeWords, A2Methods_T methods)
{
        methods->map_default(codeWords, applyPrintCodewords, NULL);
}

/* applyRead
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
void applyRead(int col, int row, A2Methods_UArray2 uarray2, void *element, 
                                                                       void *cl)
{
        assert(element != NULL);
        assert(cl != NULL);
        assert(uarray2 != NULL);
        (void) col;
        (void) row;
        uint32_t codeWord = 0;
        FILE *input = (FILE *) cl;
        for (int i = 0; i < 4; i++)
        {
                codeWord = Bitpack_newu(codeWord, 8, i << 3, fgetc(input));
        }
        // fprintf(stderr, "codeWord: %x\n", codeWord);
        *(uint32_t *) element = codeWord;
}


/* readCompressed
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
 A2Methods_UArray2 readCompressed(FILE *input, A2Methods_T methods)
{
        assert(input != NULL);
        assert(methods != NULL);
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                                  &width,
                                  &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');
        A2Methods_UArray2 inputData = 
                          methods->new(width / 2, height / 2, sizeof(uint32_t));
        methods->map_default(inputData, applyRead, input);
        return inputData;
}