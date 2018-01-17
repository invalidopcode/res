/* bitmap.h - header for bitmap.c
 *
 * API: bitmap 1.0
 * IMPLEMENTATION: reff-1
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */
#ifndef H_RES_BITMAP
#define H_RES_BITMAP
 #include "res_config.h"
 #include "res_types.h"
 #include "res_err.h"

/*Structures:*/
 typedef struct
 {
   void *base;
   size_t limit; /*in bytes. limit of 0 = 1 byte big bitmap. base+limit = highest byte that is part of the map. Describes memory dimensions of bit map, and is not the *actual* bitmap size - should be a multiple of BITS/BITS_IN_A_BYTE*/
   size_t num_bits; /*num_bits of 0 = 1 bit in the map.*/
  } res_bitmap_t;

/*External functions*/
 res_bitmap_t* res_bitmap_create(size_t num_bits); /*creates a bitmap of size num_bits and a handle for it. Returns: pointer to handle on success, NULL on failure; errno preserved on malloc fail. NOTE - num_bits starts at 0. 0 implies a bitmap with 1 bit, etc.*/
 ushort res_bitmap_destroy(res_bitmap_t* bitmap_handle); /*frees bitmap and handle memory. Returns 0 on success, other non-zero on unknown error*/

 size_t res_bitmap_alloc(res_bitmap_t* bitmap_handle, size_t block_size); /*finds and marks a continuous set of free bits, of amount block_size. block_size of 0 = 1 bit; Returns RES_BITMAP_ERR on failure, bit number of the start of the block allocated on success, starting at 0; errno is set to a corresponding res_err defined error on failure*/
 ushort res_bitmap_free(res_bitmap_t* bitmap_handle, size_t base, size_t limit); /*unmarks bits from base to (base+limit). Limit of 0 = 1 bit. Does not check if they are already free. Returns 0 on success, 2 on base out-of-range, 3 on limit out-of-range*/
 ushort res_bitmap_take(res_bitmap_t* bitmap_handle, size_t base, size_t limit); /*marks bits from base to (base+limit). Limit of 0 = 1 bit. Does not check if they are already free. Returns 0 on success, 2 on base out-of-range, 3 on limit out-of-range*/
 ushort res_bitmap_check(res_bitmap_t* bitmap_handle, size_t base, size_t limit); /*returns 0 if all bits from base to (base+limit) are 0, 1 if all bits in this range are 1, 2 if they vary, 4 on base-out-of-range, 5 on limit-out-of-range. Limit of 0 = 1 bit*/

 ushort res_bitmap_resize(res_bitmap_t* bitmap_handle, size_t num_bits);  /*re-sizes the bitmap to new total size num_bits. Returns: 0 on success, 2 on memory error; errno preserved on realloc fail*/

 size_t res_bitmap_count(res_bitmap_t* bitmap, size_t base, size_t limit);  /*counts bits taken from base to base+limit. Limit of 0 = 1 bit to count. Returns count on success, RES_BITMAP_ERR on failure; errno is set to a corresponding res_err defined error on failure*/
 size_t res_bitmap_get_size(res_bitmap_t* bitmap);  /*Returns: number of bits (taken and free) in bitmap on success, RES_BITMAP_ERR on failure; errno is set to a res_err.h error code*/

/*Internal Functions:*/
 size_t _res_bitmap_size(size_t num_bits);
 void _res_bitmap_set(void *base, size_t num_bits);
 size_t _res_bitmap_limit(size_t num_bits);
 void _res_bitmap_handle_set(res_bitmap_t *bitmap_handle, void* base, size_t num_bits);
#endif
