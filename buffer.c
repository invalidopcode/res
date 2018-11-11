/* buffer.c - safe buffer handling code
 *
 * API: buffer 1.0
 * IMPLEMENTATION: reff-2
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "res_err.h"
#include "buffer.h"

/*define macro for growing limit by 50%. Adds +8 to make very small arrays grow at reasonable rate. Adds +1 to buffer limit because limits start at 0 and without this growth will be just under 50%. Macro because used in two places and the result MUST be EXACTLY the same - avoids programming error if algorithm is tuned*/
# define RES_BUFFER_LIMIT_GROW(buffer_h)  ((buffer_h->limit) + (((buffer_h->limit) + 1) / 2) + 8)

res_buffer_t* res_buffer_create(size_t limit)
{
  res_buffer_t* handle;
  void* buffer;
   /*allocate buffer*/
    buffer = calloc(limit + 1, sizeof(int8_t));
    if ( NULL == buffer )
      return (NULL);  /*errno set by calloc*/

   /*allocate handle*/
    handle = calloc(1, sizeof(res_buffer_t));
    if ( NULL == handle )
      return (NULL);  /*errno set by calloc*/

   /*add details*/
    handle->base = buffer;
    handle->position = buffer;
    handle->limit = limit;
    handle->end_cached = (void*)( ((uint8_t*)buffer) + limit );

  return (handle);
}

ushort res_buffer_destroy(res_buffer_t* buffer)
{
    free( buffer->base );
    free( buffer );
  return (0);
}

void* res_buffer_get(res_buffer_t* buffer)
{
  return (buffer->position);
}

ushort res_buffer_next(res_buffer_t* buffer, size_t n)

{
   /*test limit will not be exceeded*/
    if ( (( (uintptr_t)(buffer->position) - (uintptr_t)(buffer->base) ) + n) > buffer->limit )
      return (1);

   /*increase position pointer*/
    buffer->position = (uint8_t*)buffer->position + n;
  return (0);
}

ushort res_buffer_prev(res_buffer_t* buffer, size_t n)
{
   /*test won't go back past buffer start*/
    if ( ((uintptr_t)(buffer->base) + n) > ((uintptr_t)(buffer->position)) )
      return (1);

   /*decrease position pointer*/
    buffer->position = (uint8_t*)buffer->position - n;
  return (0);
}

size_t res_buffer_get_n(res_buffer_t* buffer)
{
  return ( (uintptr_t)((uint8_t*)(buffer->end_cached) - (uint8_t*)(buffer->position)) + 1 );
}

ushort res_buffer_reset(res_buffer_t* buffer)
{
  buffer->position = buffer->base;
  return(0);
}

ushort res_buffer_appendf(res_buffer_t* buffer, const char* format, ...)
{
  ushort return_value;
  va_list args;
  va_start(args, format);  /*to pass on to vsnprintf*/
   /*internal function does the heavy lifting here, we just grab the va_list*/
    return_value = _res_buffer_vappendf(buffer, format, &args);
  va_end(args);
  return (return_value);
}

ushort _res_buffer_vappendf(res_buffer_t* buffer, const char* format, va_list* args)
{
  int snprintf_result;
  void* new_buffer;
  va_list tmp_args;
  ptrdiff_t position_offset;
  size_t n_chars = res_buffer_get_n( buffer );
 
   /*attempt to write full format string to buffer*/
    va_copy(tmp_args, *args);  /*temporary va_list*/
    snprintf_result = vsnprintf( (char*)res_buffer_get(buffer),
                                  n_chars,
                                  format,
                                  tmp_args);
    va_end(tmp_args);

   /*if it failed, increase size of buffer by 50% and then try again, loop until it fits or error occurs*/
    assert(n_chars < INT_MAX);  /*shouldn't be that high, and needs to be lower for next comparison*/
    while (snprintf_result >= (signed)n_chars)
    {
     /*save previous position & attempt to allocate bigger buffer*/
      position_offset = (uint8_t*)(buffer->position) - (uint8_t*)(buffer->base);
      new_buffer = realloc( buffer->base,
                            RES_BUFFER_LIMIT_GROW(buffer) + 1); /*+1 because going from limit to size*/
      if (NULL == new_buffer)
        return (1);

     /*recalculate values in buffer handle*/
      buffer->limit = RES_BUFFER_LIMIT_GROW(buffer);
      buffer->base = new_buffer;
      buffer->position = (uint8_t*)(new_buffer) + position_offset;
      buffer->end_cached = (uint8_t*)(new_buffer) + buffer->limit;
      n_chars = res_buffer_get_n( buffer );

     /*retry snprintf*/
      va_copy(tmp_args, *args);  /*temporary va_list*/
      snprintf_result = vsnprintf( (char*)res_buffer_get(buffer),
                                  n_chars,
                                  format,
                                  tmp_args);
      va_end(tmp_args);
    }

   /*was there an error writing the format string?*/
    if ( snprintf_result < 0 )
      return (2);

   /*advance buffer position to the string terminator*/
    if (0 != res_buffer_next(buffer, (unsigned)snprintf_result))  /*based on previous checks, we know that result is 0 or greater, and less than n_chars -> the maximum writeable*/
      return (3);  /*should ONLY be reached if error in buffer.c code somewhere, or struct has been tampered with...*/

  return (0);
}

