/*
 * uarray2.h
 * by Lucie Bao (cbao02), Rebecca Lee (rlee19)
 * locality (originall for iii)
 *
 * Defines the UArray2 data structure. It represents an unboxed 2D array with
 * fixed width, height, and element size. Supports access at arbitrary indices
 * of the array. Also includes map functions for row- and column-major
 * iteration.
 */

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

/* UArray2_new
 * 
 * Allocate, initialize, and return a new unboxed 2D array.
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
 *      of the client to free this array with UArray2_free().
 *      All elements are initialized to zero.
 */
extern T UArray2_new(int width, int height, int size);

/* UArray2_free
 * 
 * Deallocate and clear the given pointer to an array.
 * 
 * Parameters
 *      T  *uarray2     the address of the pointer to the array to deallocate
 *
 * Returns
 *      (Nothing.)
 *
 * Notes
 *      Will CRE if `uarray2` or `*uarray2` is NULL.
 *      Deallocates `*uarray2`. The caller relinquishes ownership of `*uarray2`.
 *      Sets `*uarray2` to NULL.
 */
extern void UArray2_free(T *uarray2);

/* UArray2_width
 * 
 * Get the width of an array.
 * 
 * Parameters
 *      T uarray2       a 2D array
 *
 * Returns
 *      int             the width of the array
 *
 * Notes
 *      Will CRE if `uarray2` is NULL.
 */
extern int UArray2_width(T uarray2);

/* UArray2_height
 * 
 * Get the height of an array.
 * 
 * Parameters
 *      T uarray2       a 2D array
 *
 * Returns
 *      int             the height of the array
 *
 * Notes
 *      Will CRE if `uarray2` is NULL.
 */
extern int UArray2_height(T uarray2);

/* UArray2_size
 * 
 * Get the size of an array's elements.
 * 
 * Parameters
 *      T uarray2       a 2D array
 *
 * Returns
 *      int             the size of the array's elements, in bytes
 *
 * Notes
 *      Will CRE if `uarray2` is NULL.
 */
extern int UArray2_size(T uarray2);

/* UArray2_at
 * 
 * Get a pointer to the element at the given position for a given array.
 * 
 * Parameters
 *      T uarray2       a 2D array
 *      int col         the column (x-position) of the desired element
 *      int row         the row (y-position) of the desired element
 * 
 * Returns
 *      void *          a pointer to the given position in the array
 *
 * Notes
 *      Will CRE if `uarray2` is NULL.
 *      Will CRE if `col` is not between 0 and one less than the array's width.
 *      Will CRE if `row` is not between 0 and one less than the array's height.
 */
void *UArray2_at(T uarray2, int col, int row);

/* UArray2_map_row_major
 *
 * Call the given function for all elements of the array, iterating in row-major
 * order.
 *
 * Parameters
 *      T uarray2       a 2D array
 *      void apply(int col, int row, T uarray2, void *addr, void *cl)
 *                      a client-provided function called for each element of
 *                      the array
 *      void *cl        an arbitrary address provided by the client
 *
 * Returns
 *      (Nothing.)
 *
 * Notes
 *      Will CRE if `uarray2` or `apply` are NULL.
 *      May raise exceptions if `apply` raises exceptions.
 *      For each position (col, row) in the array, calls `apply` passing in
 *              `col`, `row`, the array, the corresponding address, and the
 *              client-provided pointer.
 */
extern void UArray2_map_row_major(T uarray2,
                void apply(int col, int row, T uarray2, void *addr, void *cl),
                void *cl);

/* UArray2_map_col_major
 * 
 * Call the given function for all elements of the array, iterating in column-
 * major order.
 * 
 * Parameters
 *      T uarray2       a 2D array
 *      void apply(int col, int row, T uarray2, void *addr, void *cl)
 *                      a client-provided function called for each element of
 *                      the array
 *      void *cl        an arbitrary address provided by the client
 * 
 * Returns
 *      (Nothing.)
 *
 * Notes
 *      Will CRE if `uarray2` or `apply` are NULL.
 *      May raise exceptions if `apply` raises exceptions.
 *      For each position (col, row) in the array, calls `apply` passing in
 *              `col`, `row`, the array, the corresponding address, and the
 *              client-provided pointer.
 */
extern void UArray2_map_col_major(T uarray2,
                void apply(int col, int row, T uarray2, void *addr, void *cl),
                void *cl);

#undef T
#endif
