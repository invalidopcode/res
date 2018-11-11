/* buffer.h - header for buffer.c
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
#ifndef H_RES_BUFFER
#define H_RES_BUFFER
 #include <stdarg.h>
 #include "res_config.h"
 #include "res_types.h"
 #include "res_err.h"

/*Structures*/
 typedef struct {
   void* base;
   void* position;  /*current position*/
   size_t limit;
   void* end_cached;  /*for easy check if @ end*/
 } res_buffer_t;

/*External functions*/
 res_buffer_t* res_buffer_create(size_t limit);  /*creates a buffer of size limit+1 bytes, and a handle for it. Returns: pointer to handle on success, NULL on failure; errno preserved on malloc fail.*/
 ushort res_buffer_destroy(res_buffer_t* buffer);  /*frees buffer and handle memory. Returns 0 on success, other non-zero on unknown error*/

 void* res_buffer_get(res_buffer_t* buffer);  /*returns pointer to current position in buffer. Returns pointer on success, NULL on failure*/
 ushort res_buffer_next(res_buffer_t* buffer, size_t n);  /*attempts to add n bytes to buffer position. Returns 0 on success, 1 on buffer limit reached, other non-zero on unknown error*/
 ushort res_buffer_prev(res_buffer_t* buffer, size_t n);  /*attempts to move buffer position back n bytes. Returns 0 on success, 1 on buffer start reached, other non-zero on unknown error*/
 ushort res_buffer_reset(res_buffer_t* buffer);  /*moves buffer back to original position, Returns 0 on success, other non-zero on unknown error*/
 size_t res_buffer_get_n(res_buffer_t* buffer);  /*returns an appropriate value for n in strn... functions (eg strnprintf). On failure returns 0 and sets errno*/

 ushort  res_buffer_appendf(res_buffer_t* buffer, const char* format, ...);  /*attempts to sprintf string to buffer, and then advance till the new string terminator. If buffer is too small, tries to realloc 50% of limit at a time until either it fits, or realloc fails. Returns 0 on success, 1 on realloc failure (errno preserved), 2 on printf writing error, other non-zero on unknown error. Buffer position changes only on successful write*/

/*Internal functions*/
 ushort _res_buffer_vappendf(res_buffer_t* buffer, const char* format, va_list* args);
#endif

