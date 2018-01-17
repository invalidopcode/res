/* types.h - type definitions used by reusable resource code
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */
#ifndef _H_RES_TYPES
#define _H_RES_TYPES
 #include <stddef.h>
 #include <stdlib.h>  /*prevent types getting duplicated*/
 #include <stdint.h>
 #include "res_config.h"

/*Types:*/
 #ifndef ushort
  #define ushort unsigned short
 #endif
 #ifndef ulong
  #define ulong  unsigned long
 #endif
 #ifndef uchar
  #define uchar  unsigned char
 #endif

 #ifndef size_t
  #ifdef BITS_32
   #define size_t uint32_t
  #endif
  #ifdef BITS_64
   #define size_t uint64_t
  #endif
 #endif

 #define RES_ID       ushort
 #define RES_TYPE     ushort
 #define RES_SORT_KEY short

/*Type-related constants:*/
 #ifndef NULL /*IS type related - ie to a pointer type*/
  #ifdef BITS_32
    #define NULL 0x00
  #endif
  #ifdef BITS_64
    #define NULL UINT64_C(0x00)
  #endif
 #endif

 #define RES_SORT_KEY_MAX   0x0FFD
 #define RES_SORT_KEY_MIN   -0x0FFE
#endif
