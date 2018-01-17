/* bitmap.c - bitmap handling code
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
#include <stdlib.h>
#include <string.h>
#include "bitmap.h"

res_bitmap_t* res_bitmap_create(size_t num_bits)
{
  res_bitmap_t *handle;
  void* base;
   /*allocate memory*/
    base = malloc( _res_bitmap_size(num_bits) );
    if(NULL == base)
      return(NULL);

    handle = malloc( sizeof(res_bitmap_t) );
    if(NULL == handle)
    {
      free(base);
      return(NULL);
    }

   /*create bitmap and handle*/
    _res_bitmap_set(base, num_bits);
    _res_bitmap_handle_set(handle, base, num_bits);

  return(handle);
}

size_t _res_bitmap_size(size_t num_bits)
{
  return(_res_bitmap_limit(num_bits) + 1);  /*num_bits starts at zero, so we need to add 1*/
}

void _res_bitmap_set(void *base, size_t num_bits)
{
  size_t limit;

  limit = _res_bitmap_limit(num_bits);

 /*fill bitmap with 0's*/
  memset(base, 0x00, limit + 1);
}

size_t _res_bitmap_limit(size_t num_bits)
{
  size_t limit;
   /*limit is in bytes, not bits...*/
    limit = (num_bits + (BITS_IN_A_BYTE - 1)) / BITS_IN_A_BYTE;  /*round up*/
   /*limit needs to be BITS aligned...*/
    if(0 != limit%(BITS/BITS_IN_A_BYTE))
      limit += (BITS/BITS_IN_A_BYTE) - limit%(BITS/BITS_IN_A_BYTE);
  return(limit);
}

void _res_bitmap_handle_set(res_bitmap_t *bitmap, void* base, size_t num_bits)
{
  size_t limit;
    limit = _res_bitmap_limit(num_bits);
    bitmap->base = base;
    bitmap->limit = limit;
    bitmap->num_bits = num_bits;
}


ushort res_bitmap_destroy(res_bitmap_t* bitmap_handle)
{
  free((bitmap_handle->base));
  free(bitmap_handle);
  return(0);
}

size_t res_bitmap_alloc(res_bitmap_t* bitmap_handle, size_t block_size)
{
  size_t i;
  size_t num_bits;

   /*get bitmap pointer & num_bits*/
    num_bits = bitmap_handle->num_bits;

   /*check if block_size valid*/
    if(block_size > num_bits)
    {
      errno = RES_ERR_BAD_PARAMETER;
      return(RES_BITMAP_ERR);
    }

   /*test loop*/
    for(i=0; i<=(num_bits - block_size); i++)
      if(0==res_bitmap_check(bitmap_handle, i, block_size))
      {
        res_bitmap_take(bitmap_handle, i, block_size);
        return(i);
      }

  errno = RES_ERR_NO_MATCH;
  return(RES_BITMAP_ERR);  /*nothing found :-(*/
}

ushort res_bitmap_free(res_bitmap_t* bitmap_handle, size_t base, size_t limit)
{
  #ifdef BITS_64
    uint64_t *bitmap;
  #else
    uint32_t *bitmap;
  #endif
  size_t i;
   /*check base & limit*/
    if(base > bitmap_handle->num_bits)
      return(2);
    if((base > (SIZE_MAX - limit)) || ((base+limit) > bitmap_handle->num_bits))  /*if base + limit so high they wrap around, or if base+limit out of range*/
      return(3);

   /*get bitmap pointer*/
    bitmap = bitmap_handle->base;

   /*unmark bits - unoptimised, marks each bit in turn*/
    for(i=base; i<=(base+limit); i++)
    {
      #ifdef BITS_64
        bitmap[i/BITS] &= ~(UINT64_C(1)<<(i%BITS));
      #else
        bitmap[i/BITS] &= ~(1<<(i%BITS));
      #endif
    }
  return(0);
}

ushort res_bitmap_take(res_bitmap_t* bitmap_handle, size_t base, size_t limit)
{
  #ifdef BITS_64
    uint64_t *bitmap;
  #else
    uint32_t *bitmap;
  #endif
  size_t i;
   /*check base & limit*/
    if(base > bitmap_handle->num_bits)
      return(2);
    if((base > (SIZE_MAX - limit)) || ((base+limit) > bitmap_handle->num_bits))  /*if base + limit so high they wrap around, or if base+limit out of range*/
      return(3);

   /*get bitmap pointer*/
    bitmap = bitmap_handle->base;

   /*mark bits - unoptimised, marks each bit in turn*/
    for(i=base; i<=(base+limit); i++)
    {
      #ifdef BITS_64
        bitmap[i/BITS] |= UINT64_C(1)<<(i%BITS);
      #else
        bitmap[i/BITS] |= 1<<(i%BITS);
      #endif
    }
  return(0);
}

ushort res_bitmap_check(res_bitmap_t* bitmap_handle, size_t base, size_t limit)
{
  #ifdef BITS_64
    uint64_t *bitmap;
  #else
    uint32_t *bitmap;
  #endif
  size_t i;
  ushort up=0;  /*what kind of bit we're checking for*/

   /*check base & limit*/
    if(base > bitmap_handle->num_bits)
      return(4);
    if((base > (SIZE_MAX - limit)) || ((base+limit) > bitmap_handle->num_bits))  /*if base + limit so high they wrap around, or if base+limit out of range*/
      return(5);

   /*get bitmap pointer*/
    bitmap = bitmap_handle->base;

   /*test the first bit*/
    #ifdef BITS_64
      if(0 == (bitmap[base/BITS] & (UINT64_C(1)<<(base%BITS))))  /*filter out all bits apart from the one being checked*/
    #else
      if(0 == (bitmap[base/BITS] & (1<<(base%BITS))))  /*filter out all bits apart from the one being checked*/
    #endif
      up = 0;
    else
      up = 1;

   /*test the rest*/
    for(i=base+1; i<=(base+limit); i++)
    {
        #ifdef BITS_64
          if(0 == (bitmap[i/BITS] & (UINT64_C(1)<<(i%BITS))))  /*filter out all bits apart from the one being checked. Check if it's 0*/
        #else
          if(0 == (bitmap[i/BITS] & (1<<(i%BITS))))  /*filter out all bits apart from the one being checked. Check if it's 0*/
        #endif
        {
          if(1 == up)  /*i.e. if first bit is 1, and current bit is not*/
            return(2);
        } else {
          if(0 == up)  /*i.e. if first bit is 0, and current bit is not*/
            return(2);
        }

    }
  return(up);  /*bits all the same, so return either 1 or 0*/
}

ushort res_bitmap_resize(res_bitmap_t* bitmap_handle, size_t num_bits)
{
  uint8_t* base;
  size_t limit;
  size_t old_limit;
   /*calculate limit, old limit, etc*/
    limit = _res_bitmap_limit(num_bits);
    old_limit = _res_bitmap_limit(bitmap_handle->num_bits);

   /*re-alloc memory*/
    base = realloc( bitmap_handle->base, _res_bitmap_size(num_bits) );
    if(NULL == base)
      return(2);

   /*zero-out any new memory*/
    if(limit > old_limit)
      memset(base + old_limit + 1, 0x00, limit - old_limit);
     /*^^ better than using out _set - that one calculates limit from num_bits, already been done here*/

   /*update handle*/
    _res_bitmap_handle_set(bitmap_handle, base, num_bits);

  return(0);
}

size_t res_bitmap_count(res_bitmap_t* bitmap_handle, size_t base, size_t limit)
{
  size_t i;
  size_t count=0;
   /*check parameters*/
    if(base > bitmap_handle->num_bits)
    {
      errno = RES_ERR_BAD_PARAMETER;
      return(RES_BITMAP_ERR);
    }
    if((base > (SIZE_MAX - limit)) || ((base+limit) > bitmap_handle->num_bits))  /*if base + limit so high they wrap around, or if base+limit out of range*/
    {
      errno = RES_ERR_BAD_PARAMETER;
      return(RES_BITMAP_ERR);
    }

  /*counting loop (horribly unoptimised!)*/
    for(i=0; i<=limit; i++)
    {
      if(1 == res_bitmap_check(bitmap_handle, base + i, 0))
        count++;
    }
  return(count);
}

size_t res_bitmap_get_size(res_bitmap_t* bitmap_handle)
{
 /*return num_bits*/
  return(bitmap_handle -> num_bits);
}

/**************************************************************************
NOTE - alloc function is massively inefficient (uses test and take
  functions), and should be treated as a placeholder only.
**************************************************************************/
