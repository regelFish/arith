/*
 * uarray2b.c
 * by Lucie Bao (cbao02), Rebecca Lee (rlee19)
 * locality
 *
 * Implements the UArray2b data structure. It represents an unboxed 2D array
 * with fixed width, height, and element size. Array elements are stored in
 * equally sized blocks. Supports access at arbitrary indices of the array. Also
 * includes a map function for block-major iteration.
 */

 #include "stdlib.h"
 #include "stdio.h"
 #include "stdbool.h"
 #include "math.h"
 
 #include "uarray2.h"
 #include "assert.h"
 #include "mem.h"
 
 #include "uarray2b.h"
 
 #define T UArray2b_T
 
 /* The number of bytes in 64 KB, where 1 KB = 1024 bytes */
 const int SIXTY_FOUR_KB = 64 * 1024;
 
 /* struct UArray2b_T
  *
  * Holds private data for each UArray2b instance.
  * This implementation is similar to the one suggested in the spec, internally
  * using a UArray2 of UArray2. (Our UArray2 implementation itself uses a single
  * UArray.) The width, height, and element size of the array are guaranteed not
  * to change. The same applies for the distribution of elements into blocks.
  * Each block is the same size and holds the same set of elements in contiguous
  * memory locations throughout the lifetime of the UArray2b.
  *
  * Components
  *      int width       the width (number of columns) of the array
  *      int height      the height (number of rows) of the array
  *      int size        the size of each element, in bytes; must include padding
  *      int blocks_wide
  *                      the number of blocks along the width of the array
  *      int blocks_high
  *                      the number of blocks along the height of the array
  *      UArray2_T blocks
  *                      the underlying 2D array of blocks; each element of this
  *                      array is *itself* a UArray2_T of actual array elements
  */
 struct UArray2b_T {
         int width;
         int height;
         int size;
 
         int blocks_wide;
         int blocks_high;
         int blocksize;
 
         UArray2_T blocks;
 };
 
 /* UArray2b_new
  * 
  * Allocate, initialize, and return a new unboxed, blocked 2D array.
  * 
  * Parameters
  *      int width       the width (number of columns) of the array
  *      int height      the height (number of rows) of the array
  *      int size        the size of each element, in bytes; must include padding
  *                      necessary for alignment
  *      int blocksize   the number of elements along one side of a block
  *
  * Returns
  *      T               the created array
  *
  * Notes
  *      Blocks are square; `blocksize` is the side length of the square. The
  *              number of elements in a block is `blocksize * blocksize`.
  *
  *      Will CRE if width and/or height are negative.
  *      Will CRE if size and/or blocksize are not positive.
  *      Allocates memory; will CRE if allocation fails. It is the responsibility
  *              of the client to free this array with UArray2b_free().
  *      All elements are initialized to zero.
  */
 T UArray2b_new(int width, int height, int size, int blocksize)
 {
         assert(width >= 0);
         assert(height >= 0);
         assert(size > 0);
         assert(blocksize > 0);
 
         int blocks_wide = (width % blocksize > 0) /* evenly divisible? */
                 ? width / blocksize + 1           /* round up */
                 : width / blocksize;              /* no rounding required */
         int blocks_high = (height % blocksize > 0)
                 ? height / blocksize + 1
                 : height / blocksize;
 
         /* Create the outer array (of blocks) */
         UArray2b_T new_arr;
         NEW(new_arr);
 
         new_arr->width = width;
         new_arr->height = height;
         new_arr->size = size;
         new_arr->blocks_wide = blocks_wide;
         new_arr->blocks_high = blocks_high;
         new_arr->blocks = UArray2_new(blocks_wide, blocks_high,
                                       sizeof(UArray2_T *));
         new_arr->blocksize = blocksize;
 
         /* Populate the outer array with the inner block arrays */
         for (int col = 0; col < blocks_wide; col++) {
                 for (int row = 0; row < blocks_high; row++) {
                         UArray2_T *inner = UArray2_at(new_arr->blocks,
                                                       col, row);
                         *inner = UArray2_new(blocksize, blocksize, size);
                 }
         }
 
         return new_arr;
 }
 
 /* UArray2b_new_64K_block
  * 
  * Allocate, initialize, and return a new unboxed, blocked 2D array.
  * The blocksize for this array is automatically calculated such that each block
  * is as large as possible while fitting within 64 kilobytes of memory
  * (1 KB = 1024 bytes).
  *
  * Parameters
  *      int width       the width (number of columns) of the array
  *      int height      the height (number of rows) of the array
  *      int size        the size of each element, in bytes; must include padding
  *                      necessary for alignment
  *
  * Returns
  *      T               the created array
  *
  * Notes
  *      Will CRE if width and/or height are negative.
  *      Will CRE if size is not positive.
  *      Allocates memory; will CRE if allocation fails. It is the responsibility
  *              of the client to free this array with UArray2b_free().
  *      All elements are initialized to zero.
  */
 T UArray2b_new_64K_block(int width, int height, int size)
 {
         assert(width >= 0);
         assert(height >= 0);
         assert(size > 0);
 
         int blocksize = sqrt(SIXTY_FOUR_KB / size);
         if (blocksize < 1) {
                 blocksize = 1;
         }
 
         return UArray2b_new(width, height, size, blocksize);
 }
 
 /* UArray2b_free
  * 
  * Deallocate and clear the given pointer to an array.
  * 
  * Parameters
  *      T  *array2b     the address of the pointer to the array to deallocate
  *
  * Returns
  *      (Nothing.)
  *
  * Notes
  *      Will CRE if `array2b` or `*array2b` is NULL.
  *      Deallocates `*array2b`. The caller relinquishes ownership of `*array2b`.
  *      Sets `*array2b` to NULL.
  */
 void UArray2b_free(T *array2b)
 {
         assert(array2b != NULL);
 
         T referent = *array2b;
         assert(referent != NULL);
 
         /* Deallocate inner arrays */
         for (int col = 0; col < referent->blocks_wide; col++) {
                 for (int row = 0; row < referent->blocks_high; row++) {
                         UArray2_T *inner = UArray2_at(referent->blocks,
                                                       col, row);
                         UArray2_free(inner);
                 }
         }
 
         UArray2_free(&(referent->blocks));
         FREE(*array2b);
         *array2b = NULL;
 }
 
 /* UArray2b_width
  * 
  * Get the width of an array.
  * 
  * Parameters
  *      T array2b       a 2D array
  *
  * Returns
  *      int             the width of the array
  *
  * Notes
  *      Will CRE if `array2b` is NULL.
  */
 int UArray2b_width(T array2b)
 {
         assert(array2b != NULL);
         return array2b->width;
 }
 
 /* UArray2b_height
  * 
  * Get the height of an array.
  * 
  * Parameters
  *      T array2b       a 2D array
  *
  * Returns
  *      int             the height of the array
  *
  * Notes
  *      Will CRE if `array2b` is NULL.
  */
 int UArray2b_height(T array2b)
 {
         assert(array2b != NULL);
         return array2b->height;
 }
 
 /* UArray2b_size
  * 
  * Get the size of an array's elements.
  * 
  * Parameters
  *      T array2b       a 2D array
  *
  * Returns
  *      int             the size of the array's elements, in bytes
  *
  * Notes
  *      Will CRE if `array2b` is NULL.
  */
 int UArray2b_size(T array2b)
 {
         assert(array2b != NULL);
         return array2b->size;
 }
 
 /* UArray2b_blocksize
  * 
  * Get the blocksize of a blocked array.
  * 
  * Parameters
  *      T array2b       a 2D array
  *
  * Returns
  *      int             the number of elements along the side of a block of the
  *                      array
  *
  * Notes
  *      Will CRE if `array2b` is NULL.
  */
 int UArray2b_blocksize(T array2b)
 {
         assert(array2b != NULL);
         return array2b->blocksize;
 }
 
 /* UArray2b_at
  * 
  * Get a pointer to the element at the given position for a given array.
  * 
  * Parameters
  *      T array2b       a 2D array
  *      int col         the column (x-position) of the desired element
  *      int row         the row (y-position) of the desired element
  * 
  * Returns
  *      void *          a pointer to the given position in the array
  *
  * Notes
  *      Will CRE if `array2b` is NULL.
  *      Will CRE if `col` is not between 0 and one less than the array's width.
  *      Will CRE if `row` is not between 0 and one less than the array's height.
  */
 void *UArray2b_at(T array2b, int column, int row)
 {
         assert(array2b != NULL);
         assert(column >= 0 && column < array2b->width);
         assert(row >= 0 && row < array2b->height);
 
         int blocksize = array2b->blocksize;
         UArray2_T *inner = UArray2_at(array2b->blocks,
                                       column / blocksize,
                                       row / blocksize);
         return UArray2_at(*inner, column % blocksize, row % blocksize);
 }
 
 /* map_inner
  * 
  * A helper function to apply the given function to all elements of a UArray2b's
  * block.
  *
  * Parameters
  *      T outer         the original UArray2b that the map was called on
  *      UArray2_T inner
  *                      the block to apply the function to
  *      int block_col   how many blocks to the right of the top left of `outer`
  *                      `inner` is
  *      int block_row   how many blocks below the top left of the of `outer`
  *                      `inner` is
  *      void apply(int col, int row, T array2b, void *elem, void *cl)
  *                      a client-provided function called for each element of
  *                      the array
  *      void *cl        an arbitrary address provided by the client
  * 
  * Returns
  *      (Nothing.)
  *
  * Notes
  *      Will CRE if `outer`, `inner`, or `apply` are NULL.
  *      May raise exceptions if `apply` raises exceptions.
  *      For each element inside the block (`inner`), calls `apply` passing in
  *              `col`, `row`, the outer array, the corresponding address, and
  *              the client-provided pointer.
  */
 static void map_inner(
         T outer,
         UArray2_T inner,
         int block_col,
         int block_row,
         void apply(int col, int row, T array2b, void *elem, void *cl),
         void *cl)
 {
         int blocksize = outer->blocksize;
         int width = outer->width;
         int height = outer->height;
 
         /* Iterate row-major for locality */
         for (int row = 0;
              row < blocksize && block_row * blocksize + row < height;
              row++) {
                 
                 for (int col = 0;
                      col < blocksize && block_col * blocksize + col < width;
                      col++) {
 
                         void *address = UArray2_at(inner, col, row);
                         apply(block_col * blocksize + col,
                               block_row * blocksize + row,
                               outer,
                               address,
                               cl);
                 }
         }
 }
 
 /* UArray2b_map
  * 
  * Call the given function for all elements of the array. It is guaranteed that
  * the function will iterate one block of elements at a time (block-major).
  *
  * Parameters
  *      T array2b       a 2D array
  *      void apply(int col, int row, T array2b, void *elem, void *cl)
  *                      a client-provided function called for each element of
  *                      the array
  *      void *cl        an arbitrary address provided by the client
  * 
  * Returns
  *      (Nothing.)
  *
  * Notes
  *      Will CRE if `array2b` or `apply` are NULL.
  *      May raise exceptions if `apply` raises exceptions.
  *      For each position (col, row) in the array, calls `apply` passing in
  *              `col`, `row`, the array, the corresponding address, and the
  *              client-provided pointer.
  */
 void UArray2b_map(T array2b,
                   void apply(int col, int row, T array2b, void *elem, void *cl),
                   void *cl)
 {
         assert(array2b != NULL);
         assert(apply != NULL);
 
         int blocks_wide = array2b->blocks_wide;
         int blocks_high = array2b->blocks_high;
 
         /* Iterate through blocks */
         for (int block_col = 0; block_col < blocks_wide; block_col++) {
                 for (int block_row = 0; block_row < blocks_high; block_row++) {
                         UArray2_T *inner = UArray2_at(array2b->blocks,
                                                       block_col,
                                                       block_row);
                         map_inner(array2b, *inner, block_col, block_row,
                                   apply, cl);
                 }
         }
 }
 
 #undef T
 