/*
 * bitpack.c
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Implements the bitpack interface, which allows the user to pack 2-by-2 blocks
 * of pixels into 32-bit words
 */

/* C standard library */
#include <assert.h>
#include <stdio.h>

/* CS 40 */
#include "bitpack.h" 
#include "except.h"

/* Exception in the case of overflow when packing bits */
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* betterShiftL
 * 
 * Provides functionality for a bitwise left-shift operation that shifts a long
 * unsigned integer left by a provided shift amount but wraps shifts of 64 or
 * greater to 0.
 * 
 * Parameters
 *     uint64_t n        a long unsigned integer whose bits will be shifted left
 *     unsigned shift    the power of 2 of bits that `n` will be shifted left by
 *                       (so a shift of 3 will shift left by 2^3 = 8 bits)
 *
 * Returns
 *     uint64_t          the integer resulting from `n` being shifted to the
 *                       left by `shift`
 * 
 * Notes
 *     Returns 0 if `shift` is equal to or greater than 64.
 *
 */
uint64_t betterShiftL(uint64_t n, unsigned shift)
{
        if (shift >= 64) {
                return 0;
        }
        return n << shift;
}

/* betterShiftRU
 * 
 * Provides functionality for a bitwise right-shift operation that shifts a long
 * unsigned integer right by a provided shift amount but wraps shifts of 64 or
 * greater to 0.
 * 
 * Parameters
 *     uint64_t n        a long unsigned integer whose bits will be shifted
 *                       right
 *     unsigned shift    the power of 2 of bits that `n` will be shifted right
 *                       by (so a shift of 3 will shift right by 2^3 = 8 bits)
 *
 * Returns
 *     uint64_t          the integer resulting from `n` being shifted to the
 *                       right by `shift`
 * 
 * Notes
 *     Returns 0 if `shift` is equal to or greater than 64.
 *
 */
uint64_t betterShiftRU(uint64_t n, unsigned shift)
{
        if (shift >= 64) {
                return 0;
        }
        return n >> shift;
}

/* betterShiftRS
 * 
 * Provides functionality for a bitwise right-shift operation that shifts a long
 * signed integer right by a provided shift amount but wraps shifts of 64 or
 * greater to 0 if `n` is positive or -1 if `n` is negative.
 * 
 * Parameters
 *     int64_t n         a long signed integer whose bits will be shifted
 *                       right
 *     unsigned shift    the power of 2 of bits that `n` will be shifted right
 *                       by (so a shift of 3 will shift right by 2^3 = 8 bits)
 *
 * Returns
 *     uint64_t          the integer resulting from `n` being shifted to the
 *                       right by `shift`
 * 
 * Notes
 *     Returns 0 if `shift` is equal to or greater than 64 and `n` is positive.
 *     Returns -1 if `shift` is equal to or greater than 64 and `n` is positive.
 *
 */
uint64_t betterShiftRS(int64_t n, unsigned shift)
{
        if (shift >= 64) {
                if (n < 0) {
                        return -1;
                } else {
                        return 0;
                }
        }
        return n >> shift;
}

/* Bitpack_fitsu
 * 
 * Returns whether or not an unsigned integer `n` can be represented by `width`
 * bits.
 * 
 * Parameters
 *     uint64_t n        an unsigned integer to be checked whether it can be
 *                       represented by `width` bits or not.
 *     unsigned width    the number of bits that will be used to represent an
 *                       integer.
 *
 * Returns
 *     bool fits         indicator of whether or not `n` can be represented by
 *                       `width` bits (true or false)
 * 
 * Notes
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 *
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        bool fits = ((n < betterShiftL(1, width)) || !betterShiftL(1, width));
        return fits;
        
}

/* Bitpack_fitss
 * 
 * This function checks if a given integer, n, can be represented by width bits
 * in a two's complement signed representation.
 * 
 * Parameters
 *     int64_t n        an signed integer to be checked whether it can be
 *                      represented by `width` bits or not.
 *     unsigned width   the number of bits that will be used to represent an
 *                      integer. 
 *
 * Returns
 *     bool fits         indicator of whether or not `n` can be represented by
 *                       `width` bits (true or false)
 * 
 * Notes
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{

        bool fits = (((n < (int64_t) betterShiftL(1, width - 1)) && 
                      n >= (int64_t) -betterShiftL(1, width - 1)) || 
                                      !betterShiftL(1, width));
        return fits;
}

/* Bitpack_getu
 * 
 * Extracts the field from a word given the width of the field and the location
 * of the field's least significant bit for unsigned integers
 * 
 *     int64_t word     The word to extract from.
 *     unsigned width   The width of the field to be extracted. 
 *     unsigned lsb     The location of the field's least significant bit.
 *
 * Returns
 *     uint64_t field   the bits extracted from the word between width and lsb
 * 
 * Notes
 *     Will CRE if given width < 0 or if given width > 64.
 *     Will CRE if the sum of given width and lsb > 64.
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        uint64_t mask = betterShiftL((betterShiftL(1, width) - 1), lsb);
        uint64_t field = betterShiftRU((word & mask), lsb);
        
        return field;
}

/* Bitpack_gets
 * 
 * Extracts the field from a word given the width of the field and the location
 * of the field's least significant bit for signed integers
 * 
 *     int64_t word     The word to extract from.
 *     unsigned width   The width of the field to be extracted. 
 *     unsigned lsb     The location of the field's least significant bit.
 *
 * Returns
 *     int64_t       the bits extracted from the word between width and lsb
 * 
 * Notes
 *     Will CRE if given width < 0 or if given width > 64.
 *     Will CRE if the sum of given width and lsb > 64.
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        uint64_t mask = betterShiftL((betterShiftL(1, width) - 1), lsb);
        int offset = 64 - width - lsb;
        uint64_t field = betterShiftRS(
                             betterShiftL((word & mask), offset), lsb + offset);
        
        return field;
}

/* Bitpack_newu TODO
 * 
 * This function takes a 64 bit word and inserts an unsigned value into that word
 * using the given width and least significant bit.
 * 
 * Parameters
 *     int64_t n      : A copy of the unsigned integer to modify.
 *     unsigned width : The width to check with. 
 *
 * Returns
 *     Returns true if the given integer can be represented in width bits within
 *     a two's complement signed notation.
 * 
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                                                                uint64_t value)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        
        uint64_t mask = betterShiftL((betterShiftL(1, width) - 1), lsb);
        word = (word & ~mask) | betterShiftL(value, lsb);
        
        return word;
}

/* Bitpack_news TODO
 * 
 * This function checks if a given integer, n, can be represented by width bits
 * in a two's complement signed representation.
 * 
 *     int64_t word     
 *     unsigned width   
 *     unsigned lsb     
 *     int64_t value    
 *
 * Returns
 *     uint64_t
 * 
 * Notes
 *     Raises 
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                                                                int64_t value)
{
        assert(width <= 64);
        assert((width + lsb) <= 64);
        
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        uint64_t mask = betterShiftL((betterShiftL(1, width) - 1), lsb);
        value = mask & betterShiftL(value, lsb);
        word = (word & ~mask) | value;

        return word;
}
