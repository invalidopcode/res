/* res_config.h - configuration options for reusable resource code
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */
#ifndef _H_RES_CONFIG
#define _H_RES_CONFIG

#include <stdint.h>
#include <assert.h>

/*try to determine BITS*/
#ifndef BITS_64
  #ifndef BITS_32
    #ifdef __x86_64__
      #define BITS_64
      static_assert(8 == sizeof(uintptr_t), "BITS could not be determined, please define either BITS_64 or BITS_32");
    #else
      #define BITS_32
      static_assert(4 == sizeof(uintptr_t), "BITS could not be determined, please define either BITS_64 or BITS_32");
    #endif
  #endif
#endif

 #define BITS_IN_A_BYTE     8   /*obviously shouldn't change, but avoids use of magic numbers*/
 #ifdef BITS_32
   #define BYTES_IN_A_POINTER 4
   #define BITS               32
 #endif
 #ifdef BITS_64
   #define BYTES_IN_A_POINTER 8
   #define BITS               64
 #endif
#endif
