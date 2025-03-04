/*
 * bitpack.c
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Implements the bitpack interface, which allows the user to pack 2-by-2 blocks
 * of pixels into 32-bit words
 */

#include "bitpack.h" 
#include "except.h"
#include <assert.h>
#include <stdio.h>
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* betterShiftL TODO: Complete function contract
 * 
 * Returns whether or not an unsigned integer `n` can be represented by `width`
 * bits.
 * 
 *     uint64_t n        an unsigned integer to be checked whether it can be
 *                       represented by `width` bits or not.asm
 *     unsigned width    the number of bits that will be used to represent an
 *                       integer.
 *
 * Returns
 *     bool              indicator of whether or not `n` can be represented by
 *                       `width` bits 
 * 
 * Notes
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 *
 */
uint64_t betterShiftL(uint64_t n, unsigned shift)
{
        if (shift >= 64) {
                return 0;
        }
        return n << shift;
}

/* betterShiftRU TODO: Complete function contract
 * 
 * Returns whether or not an unsigned integer `n` can be represented by `width`
 * bits.
 * 
 *     uint64_t n        an unsigned integer to be checked whether it can be
 *                       represented by `width` bits or not.asm
 *     unsigned width    the number of bits that will be used to represent an
 *                       integer.
 *
 * Returns
 *     bool              indicator of whether or not `n` can be represented by
 *                       `width` bits 
 * 
 * Notes
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 *
 */
uint64_t betterShiftRU(uint64_t n, unsigned shift)
{
        if (shift >= 64) {
                return 0;
        }
        return n >> shift;
}

/* BetterShiftRS TODO: Complete function contract
 * 
 * Returns whether or not an unsigned integer `n` can be represented by `width`
 * bits.
 * 
 *     uint64_t n        an unsigned integer to be checked whether it can be
 *                       represented by `width` bits or not.asm
 *     unsigned width    the number of bits that will be used to represent an
 *                       integer.
 *
 * Returns
 *     bool              indicator of whether or not `n` can be represented by
 *                       `width` bits 
 * 
 * Notes
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
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
 *     uint64_t n        an unsigned integer to be checked whether it can be
 *                       represented by `width` bits or not.asm
 *     unsigned width    the number of bits that will be used to represent an
 *                       integer.
 *
 * Returns
 *     bool              indicator of whether or not `n` can be represented by
 *                       `width` bits 
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
 *     int64_t n      : The signed integer to check.
 *     unsigned width : The width to check with. 
 *
 * Returns
 *     Returns true if the given integer can be represented in width bits within
 *     a two's complement signed notation.
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
 *     int64_t word   : The word to extract from.
 *     unsigned width : The width of the field to be extracted. 
 *     unsigned lsb   : The location of the field's least significant bit.
 *
 * Returns
 *     uint64_t       the bits extracted from the word between width and lsb
 * 
 * Notes
 *     Will CRE if given width < 0 or if given width > 64.
 *     Will CRE if the sum of given width and lsb > 64.
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 *     TODO: Test that width is always >= 0.
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
 *     int64_t word   : The word to extract from.
 *     unsigned width : The width of the field to be extracted. 
 *     unsigned lsb   : The location of the field's least significant bit.
 *
 * Returns
 *     int64_t       the bits extracted from the word between width and lsb
 * 
 * Notes
 *     Will CRE if given width < 0 or if given width > 64.
 *     Will CRE if the sum of given width and lsb > 64.
 *     Always returns true for width 64+ because the input is always of max size
 *     64 bits.
 *     TODO: Test that width is always >= 0.
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
 * This function checks if a given integer, n, can be represented by width bits
 * in a two's complement signed representation.
 * 
 *     int64_t n      : The signed integer to check.
 *     unsigned width : The width to check with. 
 *
 * Returns
 *     Returns true if the given integer can be represented in width bits within
 *     a two's complement signed notation.
 * 
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
        // fprintf(stderr, "Value: %lx\n", value);
        value = mask & betterShiftL(value, lsb);
        // fprintf(stderr, "Mask: %lx\n", mask);

        word = (word & ~mask) | value;
        // fprintf(stderr, "Word: %lx\n", word);
        return word;
}