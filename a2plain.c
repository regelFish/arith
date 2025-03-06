/*
 * a2plain.c
 * by Lucie Bao (cbao02), Rebecca Lee (rlee19)
 * arith (originally for locality)
 *
 * Implements the A2Methods interface for plain `UArray2`s. A2Methods is a
 * methods suite that allows for 2D array operations to be agnostic of the
 * underlying array design (plain vs. blocked). It includes functions to create
 * new arrays, observe their properties (e.g., width and height), access their
 * elements, and perform mapping operations.
 */

#include <string.h>

#include <a2plain.h>
#include "uarray2.h"
#include "assert.h"

typedef A2Methods_UArray2 A2;

/* new
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
 *      A2              the created array
 *
 * Notes
 *      Will CRE if width and/or height are negative.
 *      Will CRE if size is not positive.
 *      Allocates memory; will CRE if allocation fails. It is the responsibility
 *              of the client to free this array with `free()`.
 *      All elements are initialized to zero.
 */
static A2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}
 
/* new_with_blocksize
 * 
 * Allocate, initialize, and return a new unboxed 2D array.
 * For the plain 2D array, this is functionally equivalent to `new()`.
 *
 * Parameters
 *      int width       the width (number of columns) of the array
 *      int height      the height (number of rows) of the array
 *      int size        the size of each element, in bytes; must include padding
 *                      necessary for alignment
 *
 * Returns
 *      A2              the created array
 *
 * Notes
 *      Will CRE if width and/or height are negative.
 *      Will CRE if size is not positive.
 *      Allocates memory; will CRE if allocation fails. It is the responsibility
 *              of the client to free this array with `free()`.
 *      All elements are initialized to zero.
 */
static A2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void)blocksize;
        return UArray2_new(width, height, size);
}

/* a2free
 * 
 * Deallocate and clear the given pointer to an array.
 * 
 * Parameters
 *      A2 *array2p     the address of the pointer to the array to deallocate
 *
 * Returns
 *      (Nothing.)
 *
 * Notes
 *      Will CRE if `array2p` or `*array2p` is NULL.
 *      Deallocates `*array2p`. The caller relinquishes ownership of `*array2p`.
 *      Sets `*array2p` to NULL.
 *      Named `a2free` so as not to conflict with the built-in `free` function.
 */
static void a2free(A2 *array2p)
{
        UArray2_free((UArray2_T *)array2p);
}

/* width
 * 
 * Get the width of an array.
 * 
 * Parameters
 *      A2 uarray2      a 2D array
 *
 * Returns
 *      int             the width of the array
 *
 * Notes
 *      Will CRE if `array2` is NULL.
 */
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/* height
 * 
 * Get the height of an array.
 * 
 * Parameters
 *      A2 uarray2      a 2D array
 *
 * Returns
 *      int             the height of the array
 *
 * Notes
 *      Will CRE if `array2` is NULL.
 */
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/* size
 * 
 * Get the size of an array.
 * 
 * Parameters
 *      A2 uarray2      a 2D array
 *
 * Returns
 *      int             the size of the array
 *
 * Notes
 *      Will CRE if `array2` is NULL.
 */
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/* blocksize
 * 
 * Get the blocksize of an array.
 * 
 * Parameters
 *      A2 uarray2      a 2D array
 *
 * Returns
 *      int             the blocksize of the array; always 1
 *
 * Notes
 *      Will CRE if `array2` is NULL.
 *      For a plain UArray2, the blocksize is defined to be 1.
 */
static int blocksize(A2 array2)
{
        assert(array2 != NULL);
        return 1;
}

/* at
 * 
 * Get a pointer to the element at the given position for a given array.
 * 
 * Parameters
 *      A2 uarray2      a 2D array
 *      int col         the column (x-position) of the desired element
 *      int row         the row (y-position) of the desired element
 * 
 * Returns
 *      A2Methods_Object *
 *                      a pointer to the given position in the array
 *
 * Notes
 *      Will CRE if `array2` is NULL.
 *      Will CRE if `col` is not between 0 and one less than the array's width.
 *      Will CRE if `row` is not between 0 and one less than the array's height.
 */
static A2Methods_Object *at(A2 array2, int col, int row)
{
        return UArray2_at(array2, col, row);
}

typedef void UArray2_applyfun(int i, int j, UArray2_T array2b,
                              void *elem, void *cl);

/* map_row_major
 *
 * Call the given function for all elements of the array, iterating in row-major
 * order.m
 *
 * Parameters
 *      A2 uarray2      a 2D array
 *      A2Methods_applyfun apply
 *                      a function to apply to each element of the array
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
static void map_row_major(A2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/* map_col_major
 *
 * Call the given function for all elements of the array, iterating in
 * column-major order.
 *
 * Parameters
 *      A2 uarray2      a 2D array
 *      A2Methods_applyfun apply
 *                      a function to apply to each element of the array
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
static void map_col_major(A2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/* uarray2_methods_plain_struct
 *
 * An implementation of the A2Methods interface for plain 2D arrays.
 */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,

        width,
        height,
        size,
        blocksize,

        at,

        map_row_major,
        map_col_major,
        NULL,                           /* map_block_major */
        map_row_major,                  /* map_default */

        small_map_row_major,
        small_map_col_major,
        NULL,                           /* small_map_block_major */
        small_map_row_major             /* small_map_default */
};

/* The exported struct */
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
