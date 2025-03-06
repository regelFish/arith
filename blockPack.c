/*
 * blockPack.c
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Implements the blockPack interface which allows quantization and
 * dequantization of the video component values of an array of pixels
 * 
 */
#include "blockPack.h"
#include "bitpack.h"

struct abcd {
        float a, b, c, d;
};

struct myYs {
        float Y1, Y2, Y3, Y4;
};

struct pack {
        unsigned a;
        int b, c, d;
};

struct fullPack {
        struct pack pack;
        unsigned pb, pr;
};

/* A struct to use in our apply function */
struct mappingCl {
        A2 array;
        A2Methods_T methods;
};

A2 unEncode(A2 codeWords, A2Methods_T methods);

/* quantabcd
 *
 * Quantizes the cosine coefficient values of a 2-by-2 block of pixels, turning
 * them into scaled integers.
 *
 * Parameters
 *      struct abcd vals       a struct containing cosine coefficient values of 
 *                             a 2-by-2 block of pixels.
 *
 * Returns
 *      struct pack quants     a struct containing the quantized, scaled int
 *                             values of a, b, c, and d.
 *
 * Notes
 *      None.
 */
  struct pack quantabcd(struct abcd vals)
  {
        unsigned aQuant = vals.a * 511;
        if (vals.b > 0.3) {
                vals.b = 0.3;
        } else if (vals.b < -0.3) {
                vals.b = -0.3;
        }
        if (vals.c > 0.3) {
                vals.c = 0.3;
        } else if (vals.c < -0.3) {
                vals.c = -0.3;
        }
        if (vals.d > 0.3) {
                vals.d = 0.3;
        } else if (vals.d < -0.3) {
                vals.d = -0.3;
        }
        int bQuant = vals.b * 50;
        int cQuant = vals.c * 50;
        int dQuant = vals.d * 50;
        
        struct pack quants = {aQuant, bQuant, cQuant, dQuant};
        return quants;
  }

/* discreteTrans
 *
 * Uses a discrete cosine transformation to transform the four Y values of
 * pixels into cosine coefficients a, b, c, and d.
 *
 * Parameters
 *      struct myYs y          a struct containing Y values of a 2-by-2 block
 *                             of pixels.
 *
 * Returns
 *      struct pack quant(abcdTrans)   a struct containing the quantized,
 *                                     scaled int values of a, b, c, and d.
 *
 * Notes
 *      None.
 */
struct pack discreteTrans(struct myYs y)
{
        float a = (y.Y4 + y.Y3 + y.Y2 + y.Y1) / 4.0;
        float b = (y.Y4 + y.Y3 - y.Y2 - y.Y1) / 4.0;
        float c = (y.Y4 - y.Y3 + y.Y2 - y.Y1) / 4.0;
        float d = (y.Y4 - y.Y3 - y.Y2 + y.Y1) / 4.0;

        struct abcd abcdTrans = {a, b, c, d};
        return quantabcd(abcdTrans);
}

/* discreteDetrans
 *
 * Uses an inverse discrete cosine transformation to transform the four cosine
 * coefficient values of pixels into their corresponding Y luma values.
 *
 * Parameters
 *      struct abcd vals       a struct containing cosine coefficient values of
 *                             a 2-by-2 block of pixels.
 *
 * Returns
 *      struct myYs yDetrans   a struct containing the Y luma values of a
 *                             2-by-2 block of pixels.
 *
 * Notes
 *      None.
 */
struct myYs discreteDetrans(struct abcd vals)
{
        float Y1 = (vals.a - vals.b - vals.c + vals.d);
        float Y2 = (vals.a - vals.b + vals.c - vals.d);
        float Y3 = (vals.a + vals.b - vals.c - vals.d);
        float Y4 = (vals.a + vals.b + vals.c + vals.d);

        struct myYs yDetrans = {Y1, Y2, Y3, Y4};
        return yDetrans;
}

/* unQuantabcd
 *
 * Reverses the quantization of quantized, scaled integers by unscaling them
 * and casting into floats.
 *
 * Parameters
 *      struct pack quant      a struct containing the quantized, scaled int
 *                             values of a, b, c, and d.
 *
 * Returns
 *      struct abcd unQuant    a struct containing cosine coefficient values of
 *                             a 2-by-2 block of pixels.
 *
 * Notes
 *      None.
 */
struct abcd unQuantabcd(struct pack quant)
{
        float a = (float)quant.a / 511;
        float b = (float)quant.b / 50;
        float c = (float)quant.c / 50;
        float d = (float)quant.d / 50;

        struct abcd unQuant = {a, b, c, d};
        return unQuant;
}

/* unCalc2by2
 *
 * Sets the video component values for a 2-by-2 block of pixels from its
 * compressed state.
 * 
 * Parameters
 *      A2 packed              An array of fullpack structs that contain the 
 *                             the compressed versions of video component values
 *                             for a 2-by-2 block of pixels.
 *      A2Methods_T methods    a methods suite for accessing values in the 
 *                             A2 packed.
 *      int col                the column corresponding to a fullpack struct in
 *                             packed.
 *      int row                the row corresponding to a fullpack struct in
 *                             packed.
 *
 * Returns
 *      struct vidComp myVidComps
 *                             a struct vidComp that contains lossy video
 *                             component values for four pixels in a 2-by-2
 *                             block after decompression
 *
 * Notes
 *      Will CRE if packed is NULL.
 *      Will CRE if methods is NULL.
 *      Will CRE if the malloc'd array of vidComp structs is NULL.
 *      The video component values returned in the vidComp struct will not be
 *      equivalent to the values of the original image, since the average pb
 *      and pr values will be used for each pixel and the Y values will be 
 *      rounded.
 */
struct vidComp *unCalc2by2(A2 packed, A2Methods_T methods, int col, int row)
{
        assert(packed != NULL);
        assert(methods != NULL);
        assert(col >= 0);
        assert(row >= 0);
        struct fullPack compPixel = 
                              *(struct fullPack *)methods->at(packed, col, row);

        unsigned avgPb = compPixel.pb;
        unsigned avgPr = compPixel.pr;
        float pb = Arith40_chroma_of_index(avgPb);
        float pr = Arith40_chroma_of_index(avgPr); 

        struct abcd components = unQuantabcd(compPixel.pack);
        struct myYs yBlock = discreteDetrans(components);

        struct vidComp *myVidComps = malloc(sizeof(struct vidComp) * 4);
        assert(myVidComps != NULL);
        
        myVidComps[0].y = yBlock.Y1;
        myVidComps[1].y = yBlock.Y2;
        myVidComps[2].y = yBlock.Y3;
        myVidComps[3].y = yBlock.Y4;

        for (int i = 0; i < 4; i++)
        {
                myVidComps[i].pb = pb;
                myVidComps[i].pr = pr;
        }

        return myVidComps;
}


/* calc2by2
 *
 * Gets the video component values of a 2-by-2 block of pixels, takes the 
 * average of pb and pr, quantizes the average pb and pr and Y luma values,
 * then returns a struct containing the quantized values.
 * 
 * Parameters
 *      A2 vComp               a struct containing the video component values
 *                             of the pixels in a raster.
 *      A2Methods_T methods    a methods suite for accessing values in the 
 *                             A2 vComp.
 *      int col                the column of the top-left pixel in the 2-by-2
 *                             block to be taken.
 *      int row                the row of the top-left pixel in the 2-by-2
 *                             block to be taken.
 *
 * Returns
 *      struct abcd unQuant    a struct containing cosine coefficient values of
 *                             a 2-by-2 block of pixels
 *
 * Notes
 *      Will CRE if vComp is NULL.
 *      Will CRE if methods is NULL.
 *      Will CRE if col or row are out of bounds or if col or row are uneven.
 */
struct fullPack calc2by2(A2 vComp, A2Methods_T methods, int col, int row)
{
        assert(vComp != NULL);
        assert(methods != NULL);
        assert(col >= 0);
        assert(row >= 0);
        assert(!(col & 1));
        assert(!(row & 1));
          
        struct vidComp one = 
                        *(struct vidComp *)methods->at(vComp, col, row);
        struct vidComp two = 
                        *(struct vidComp *)methods->at(vComp, col + 1, row);
        struct vidComp three = 
                        *(struct vidComp *)methods->at(vComp, col, row + 1);
        struct vidComp four = 
                        *(struct vidComp *)methods->at(vComp, col + 1, row + 1);

        

        float avgPb = (one.pb + two.pb + three.pb + four.pb) / 4.0;
        float avgPr = (one.pr + two.pr + three.pr + four.pr) / 4.0;

        unsigned pb = Arith40_index_of_chroma(avgPb);
        unsigned pr = Arith40_index_of_chroma(avgPr); 

        struct myYs yBlock = {one.y, two.y, three.y, four.y};

        struct pack trans = discreteTrans(yBlock);
        struct fullPack components = {trans, pb, pr};

        return components;
}

/* apply2by2
 *
 * Apply function that uses calc2by2 to convert 2by2 blocks of pixels to pb, pr
 * a, b, c, and d values. This executes the main compression algorithm of this 
 * program.
 *
 * Parameters
 *      int col                the column of the top-left pixel in a 2-by-2
 *                             block of pixels.
 *      int row                the row of the top-left pixel in a 2-by-2
 *                             block of pixels.
 *      A2 uarray2             the array of pixels of a PPM image in video
 *                             component format.
 *      void *element          A pointer to the current element worked on.
 *      void *cl               A bundle containing the array we are writing to,
 *                             as well as the methods to manipulate the arrays.
 *
 * Returns
 *      None (void)
 *
 * Notes
 *      Modifies the array provided in the cl parameter.
 *      Will CRE if element is NULL.
 *      Will CRE if cl is NULL.
 *      Will CRE if the array or methods contained in the cl are NULL.
 *      Will CRE if uarray2 is NULL.
 *      
 */
void apply2by2(int col, int row, A2 uarray2, void *element, void *cl)
{
        assert(element != NULL);
        assert(cl != NULL);
        assert(uarray2 != NULL);
        struct mappingCl bundle = *(struct mappingCl *) cl;
        assert(bundle.array != NULL);
        assert(bundle.methods != NULL);
        A2Methods_T methods = bundle.methods;
        A2 newArray = bundle.array;
        if (!(col & 1) && !(row & 1)) {
                *(struct fullPack *)methods->at(newArray, col / 2, row / 2) = 
                                           calc2by2(uarray2, methods, col, row);
        }
}

/* unApply2by2
 *
 * Apply function that uses unCalc2by2 to convert compressed pixel data into
 * 2by2 blocks of pixels in the video component format. 
 *
 * Parameters
 *      int col                the column of the element being worked on.
 *      int row                the row of the element being worked on.
 *
 *      A2 uarray2             the array of compressed pixels of a PPM image in 
 *                             a custom format containing average Pb and Pr
 *                             values as well as discrete cosign transformation
 *                             of the Y values.
 *
 *      void *element          A pointer to the current element worked on.
 *      void *cl               A bundle containing the array we are writing to,
 *                             as well as the methods to manipulate the arrays.
 *
 * Returns
 *      None (void)
 *
 * Notes
 *      Modifies the array provided in the cl parameter.
 *      Will CRE if element is NULL.
 *      Will CRE if cl is NULL.
 *      Will CRE if the array or methods contained in the cl are NULL.
 *      Will CRE if uarray2 is NULL.
 *      
 */
void unApply2by2(int col, int row, A2 uarray2, void *element, void *cl)
{
        assert(element != NULL);
        assert(cl != NULL);
        assert(uarray2 != NULL);
        struct mappingCl bundle = *(struct mappingCl *) cl;
        assert(bundle.array != NULL);
        assert(bundle.methods != NULL);
        A2Methods_T methods = bundle.methods;
        A2 newArray = bundle.array;
        struct vidComp *comps = unCalc2by2(uarray2, methods, col, row);
        for (int i = 0; i < 2; i++)
        {
                for (int j = 0; j < 2; j++)
                {
                        *(struct vidComp *) methods->at(newArray, 
                                                        col * 2 + j, 
                                                        row * 2 + i) = 
                                                               comps[i * 2 + j];
                }
                
        }
        free(comps);
}

/* pack2by2
 *
 * Calls the functions of blockPack to quantize the video component values of 
 * a PPM image's pixel raster an prepare them to be packed into a 32-bit word.
 *
 * Parameters
 *      A2 vComp               an array of vidComp structs that holds the video
 *                             component values of the pixels of a PPM image.
 *      A2Methods_T methods    a methods suite for creating new and accessing
 *                             the values of a UArray2.
 *
 * Returns
 *      Returns a Uarray2 containing the compressed image data in a struct.
 *      
 *
 * Notes
 *      Will CRE if vComp is NULL.
 *      Will CRE if methods is NULL.
 *      
 */
A2 pack2by2(A2 vComp, A2Methods_T methods)
{
        assert(vComp != NULL);
        assert(methods != NULL);

        int width = methods->width(vComp);
        int height = methods->height(vComp);
        
        A2 newArr = 
                   methods->new(width / 2, height / 2, sizeof(struct fullPack));
        struct mappingCl bundle = {newArr, methods};
        methods->map_default(vComp, apply2by2, &bundle);
        
        return newArr;
}

/* decode
 *
 * Calls the functions of blockPack to return the quantized values of video
 * components back into their unquantized states
 *
 * Parameters
 *      A2 codeWords           an array of unsigned 32-bit codewords containing
 *                             the values of a, b, c, d, pb, and pr.
 *      A2Methods_T methods    a methods suite for creating new and accessing
 *                             the values of a UArray2.
 *
 * Returns
 *      A2 newArr              an array of vidComp structs containing the lossy
 *                             video component values of a pixel in the image
 *                             raster. The array contains all the pixels in the
 *                             image.
 *
 * Notes
 *      Will CRE if packed is NULL.
 *      Will CRE if methods is NULL.
 *      
 */
A2 decode(A2 codeWords, A2Methods_T methods)
{
        assert(codeWords != NULL);
        assert(methods != NULL);

        A2 packArr = unEncode(codeWords, methods);
        
        int width = methods->width(packArr);
        int height = methods->height(packArr);
        
        A2 newArr = methods->new(width * 2, height * 2, sizeof(struct vidComp));
        struct mappingCl bundle = {newArr, methods};
        methods->map_default(packArr, unApply2by2, &bundle);
        methods->free(&packArr);
        
        return newArr;
}

/* packCodeword
 *
 * Calls the functions of bitPack to pack the quantized and transformed versions
 * of the video components of a 2-by-2 block of pixels into a 32-bit codeword
 *
 * Parameters
 *      struct fullPack block  a struct containing the compressed video
 *                             component values of a 2-by-2 block of pixels
 *
 * Returns
 *      uint32_t codeword      the 32-bit word representing the 6 values a, b, 
 *                             c, d, pb, and pr.
 *
 * Notes
 *      None.
 *      
 */
uint32_t packCodeword(struct fullPack block)
{
        uint32_t codeword = 0;
        codeword = Bitpack_newu(codeword, 9, 23, block.pack.a);
        codeword = Bitpack_news(codeword, 5, 18, block.pack.b);
        codeword = Bitpack_news(codeword, 5, 13, block.pack.c);
        codeword = Bitpack_news(codeword, 5, 8, block.pack.d);
        codeword = Bitpack_newu(codeword, 4, 4, block.pb);
        codeword = Bitpack_newu(codeword, 4, 0, block.pr);
        return codeword;
}

/* unPackCodeword
 *
 * Calls the functions of bitPack to unpack the quantized and transformed
 * video components of a 2-by-2 block of pixels from a 32-bit codeword
 *
 * Parameters
 *      uint32_t codeword      a 32-bit word encoding the values of a, b, c, d,
 *                             pb, and pr to be extracted from.
 *
 * Returns
 *      fullPack newPack       a fullPack struct containing the extracted,
 *                             quantized a, b, c, d, pb, and pr.
 *
 * Notes
 *      None.
 *      
 */
struct fullPack unPackCodeword(uint32_t codeword) {
        struct fullPack newPack;
        newPack.pack.a = Bitpack_getu(codeword, 9, 23);
        
        newPack.pack.b = Bitpack_gets(codeword, 5, 18);
        
        newPack.pack.c = Bitpack_gets(codeword, 5, 13);

        newPack.pack.d = Bitpack_gets(codeword, 5, 8);

        newPack.pb = Bitpack_getu(codeword, 4, 4);
        newPack.pr = Bitpack_getu(codeword, 4, 0);
        return newPack;
}

/* applyEncode TODO: Complete function contract
 *
 * Apply function that uses encode to pack the values of a, b, c, d, pb, and pr
 * into a 32-bit word
 *
 * Parameters
 *      int col                the column of the top-left pixel in a 2-by-2
 *                             block of pixels.
 *      int row                the row of the top-left pixel in a 2-by-2
 *                             block of pixels.
 *      A2 uarray2             the array of pixels of a PPM image in video
 *                             component format.
 *      void *element          A pointer to the current element worked on.
 *      void *cl               A bundle containing the array we are writing to,
 *                             as well as the methods to manipulate the arrays.
 *
 * Returns
 *      None (void)
 *
 * Notes
 *      Modifies the array provided in the cl parameter.
 *      Will CRE if element is NULL.
 *      Will CRE if cl is NULL.
 *      Will CRE if the array or methods contained in the cl are NULL.
 *      Will CRE if uarray2 is NULL
 *      
 */
void applyEncode(int col, int row, A2 uarray2, void *element, void *cl)
{
        assert(element != NULL);
        assert(cl != NULL);
        assert(uarray2 != NULL);
        struct mappingCl bundle = *(struct mappingCl *) cl;
        assert(bundle.array != NULL);
        assert(bundle.methods != NULL);
        A2Methods_T methods = bundle.methods;
        A2 newArray = bundle.array;
        
        *(uint32_t *) methods->at(newArray, col, row) = 
                                     packCodeword(*(struct fullPack *) element);
}

/* encode
 *
 * Calls the packCodeword function to pack all 2-by-2 block of pixel's values
 * into 32-bit codewords
 *
 * Parameters
 *      A2 vComp               an array of vidComp structs containing the video
 *                             component values of the pixels of an image
 *      A2Methods_T methods    a methods suite for creating new and accessing
 *                             the values of a UArray2.
 *
 * Returns
 *      Returns a Uarray2 containing the 32-bit code words representing the full
 *      raster of a compressed image.
 *      
 *
 * Notes
 *      Will CRE if packArr is NULL.
 *      Will CRE if methods is NULL.
 *      
 */
A2 encode(A2 vComp, A2Methods_T methods)
{
        assert(vComp != NULL);
        assert(methods != NULL);

        
        A2 packArr = pack2by2(vComp, methods);

        int width = methods->width(packArr);
        int height = methods->height(packArr);
        
        A2 codeWords = methods->new(width, height, sizeof(uint32_t));
        struct mappingCl bundle = {codeWords, methods};
        methods->map_default(packArr, applyEncode, &bundle);
        methods->free(&packArr);
        
        return codeWords;
}

/* applyunEncode TODO: Complete function contract
 *
 * Apply function that uses unencode to unpack the values of a, b, c, and d, pb,
 * and pr from a 32-bit code word.
 *
 * Parameters
 *      int col                the column of the top-left pixel in a 2-by-2
 *                             block of pixels.
 *      int row                the row of the top-left pixel in a 2-by-2
 *                             block of pixels.
 *      A2 uarray2             the array of pixels of a PPM image in video
 *                             component format.
 *      void *element          A pointer to the current element worked on.
 *      void *cl               A bundle containing the array we are writing to,
 *                             as well as the methods to manipulate the arrays.
 *
 * Returns
 *      None (void)
 *
 * Notes
 *      Modifies the array provided in the cl parameter.
 *      Will CRE if element is NULL.
 *      Will CRE if cl is NULL.
 *      Will CRE if the array or methods contained in the cl are NULL.
 *      Will CRE if uarray2 is NULL
 *      
 */
void applyunEncode(int col, int row, A2 uarray2, void *element, void *cl)
{
        assert(element != NULL);
        assert(cl != NULL);
        assert(uarray2 != NULL);
        struct mappingCl bundle = *(struct mappingCl *) cl;
        assert(bundle.array != NULL);
        assert(bundle.methods != NULL);
        A2Methods_T methods = bundle.methods;
        A2 newArray = bundle.array;
        
        *(struct fullPack *) methods->at(newArray, col, row) = 
                                          unPackCodeword(*(uint32_t *) element);
}

/* unEncode
 *
 * Reads 32-bit code words in sequence into an array of fullPack structs,
 * extracting the values of a, b, c, d, pb, and pr.
 *
 * Parameters
 *      A2 codeWords           an array of 32-bit codewords containing the 
 *                             compressed information of a raster of pixels.
 *      A2Methods_T methods    a methods suite for creating new and accessing
 *                             the values of a UArray2.
 *
 * Returns
 *      Returns a Uarray2 containing the array of fullPack structs. These
 *      fullPack structs contain the unpacked values of a, b, c, d, pb, and pr.
 *      
 *
 * Notes
 *      Will CRE if codeWords is NULL.
 *      Will CRE if methods is NULL.
 *      
 */
A2 unEncode(A2 codeWords, A2Methods_T methods)
{
        assert(codeWords != NULL);
        assert(methods != NULL);
        
        int width = methods->width(codeWords);
        int height = methods->height(codeWords);
        
        A2 packArr = methods->new(width, height, sizeof(struct fullPack));
        struct mappingCl bundle = {packArr, methods};
        methods->map_default(codeWords, applyunEncode, &bundle);
        
        return packArr;
}