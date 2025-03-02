/*
 * bitpack.c
 * by Rigoberto Rodriguez-Anton (rrodi08), Rebecca Lee (rlee19)
 * arith
 *
 * Implements the bitpack interface, which allows the user to pack 2-by-2 blocks
 * of pixels into 32-bit words
 */

 #include <stdbool.h>
 #include <stdint.h>
 #include <except.h>

 bool Bitpack_fitsu(uint64_t n, unsigned width);

 