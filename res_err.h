/* res_err.h - defines some common error codes
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */
#ifndef _H_RES_ERR
#define _H_RES_ERR
  #include <errno.h>
  #include <stdint.h>

 /*return codes*/
  #define RES_BITMAP_ERR     SIZE_MAX-1
  #define RES_LIST_ERR       SIZE_MAX-1
  #define RES_STACK_ERR      SIZE_MAX-1
  #define RES_ID_ERR         0xFFFE
  #define RES_TYPE_ERR       0xFFFE
  #define RES_SORT_KEY_ERR   0x0FFE


 /*errno codes and strings - if we could depend on c11 this might not be needed...*/
  #define RES_ERR_BAD_PARAMETER   303
  #define RES_ERR_INCOMPATIBLE_RESOURCE  304
  #define RES_ERR_NO_MATCH        305
  #define RES_ERR_NO_CURRENT_NODE 306
  #define RES_ERR_STACK_EMPTY     307
  #define RES_ERR_NOT_FOUND       308
  #define RES_ERR_UNKNOWN         309

 /*functions*/
  const char* res_err_string(int err);  /*returns a pointer to a string explaining the error, like strerror. If error code not known, returns strerror(errno)*/
#endif
