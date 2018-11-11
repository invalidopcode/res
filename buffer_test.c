/* buffer_test.c - unit tests for buffer.c
 *
 * REQUIRES: buffer_1
 * TESTS: buffer_1.0
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "buffer.h"
#include "res_err.h"

/*why use these macros, instead of assert? Allows us to give more information, using res_err_string, and also by exiting gracefully*/
# define ERRNO_FAIL_ON(condition, on_error)  \
          do {\
            if ( (condition) || (0 != errno) ) \
            { \
              printf("\nFAILED - error %s, line %u\n", res_err_string(errno), __LINE__); \
              on_error ; \
            } \
          } while(0)
# define FAIL_ON(condition, on_error)  \
          do { \
            if ( (condition) ) \
            { \
              printf("\nFAILED, line %u on %s\n", __LINE__, #condition ); \
              on_error ; \
            } \
          } while(0)

 int main(void);
 int create_init_destroy(void);
 int get_next_prev(void);  /*also tests get_n*/
 int appendf(void);

int main()
{
   /*run tests*/
    printf("01 - create, init, & destroy\n");
    if (! create_init_destroy() )
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

    printf("02 - get, next, prev, get_n, reset\n");
    if (! get_next_prev() )
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

    printf("03 - appendf\n");
    if (! appendf() )
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

  printf("ALL TESTS PASSED!\n");
  return(EXIT_SUCCESS);
}

int create_init_destroy()
{
  res_buffer_t* small_buffer;
  res_buffer_t* large_buffer;
   /*reset errno*/
    errno = 0;

   /*make a very small buffer*/
    printf("\tcreating buffer of size 1... ");
    small_buffer = res_buffer_create(0);
    ERRNO_FAIL_ON( (NULL == small_buffer), return(false) );
    printf("Good!");

   /*make a huge one*/
    printf("\tsize 1,000,000... ");
    large_buffer = res_buffer_create(999999);
    ERRNO_FAIL_ON( NULL == large_buffer, return(false) );
    printf("Good!\n");

   /*destroy all 4*/
    printf("\tdestroying... ");
    FAIL_ON( 0 != res_buffer_destroy( small_buffer ), return(false) );
    FAIL_ON( 0 != res_buffer_destroy( large_buffer ), return(false) );
    printf("Good!\n");
  return(true);
}

int get_next_prev()
{
  size_t i;
  res_buffer_t* buffer_h;
  uint8_t* buffer;
   /*reset errno*/
    errno = 0;

   /*make buffer & fill with pre-planned data*/
    printf("\tcreating test buffer & contents... ");
    buffer_h = res_buffer_create(255);
    ERRNO_FAIL_ON( NULL == buffer_h, return (false) );
    buffer = (uint8_t*)res_buffer_get(buffer_h);
    ERRNO_FAIL_ON( NULL == buffer, return (false) );

    for (i=0; i<256; i++)
      buffer[i] = (uint8_t)i;
    printf("Good!\n");

   /*use next to scroll through verifying each one w/get*/
    printf("\tverifying with get(), next()... ");
    for (i=0; i<255; i++)
    {
      buffer = res_buffer_get(buffer_h);
      ERRNO_FAIL_ON( NULL == buffer , return (false) );
      FAIL_ON( *buffer != i , return (false));
      FAIL_ON( res_buffer_get_n(buffer_h) != 256-i , return (false));
      FAIL_ON( 0 != res_buffer_next(buffer_h, 1) , return (false) );
    }

   /*last char verified separately as next next() will fail...*/
    buffer = res_buffer_get(buffer_h);
    ERRNO_FAIL_ON( NULL == buffer , return (false) );
    FAIL_ON( *buffer != 255 , return (false));
    ERRNO_FAIL_ON( res_buffer_get_n(buffer_h) != 1 , return (false));
    printf("Good!");

   /*error condition: next at end of buffer*/
    printf("\t error conditions... ");
    FAIL_ON( 0 == res_buffer_next(buffer_h, 1) , return (false));
    printf("Good!\n");

   /*use prev to scroll back and verify w/get*/
    printf("\tverifying with get(), prev()... ");
    for (i=256; i>1; i--)
    {
      buffer = res_buffer_get(buffer_h);
      ERRNO_FAIL_ON( NULL == buffer , return (false));
      FAIL_ON( *buffer != i-1 , return (false));
      FAIL_ON( res_buffer_get_n(buffer_h) != (257 - i) , return(false) );
      FAIL_ON( 0 != res_buffer_prev(buffer_h, 1) , return (false));
    }

   /*last char verified separately as next prev() will fail...*/
    buffer = res_buffer_get(buffer_h);
    ERRNO_FAIL_ON( NULL == buffer , return (false) );
    FAIL_ON( *buffer != 0 , return (false));
    ERRNO_FAIL_ON( res_buffer_get_n(buffer_h) != 256 , return (false));
    printf("Good!");

   /*error condition: prev at start of buffer*/
    printf("\t error conditions... ");
    FAIL_ON( 0 == res_buffer_prev(buffer_h, 1) , return (false));
    printf("Good!\n");

   /*error condition: prev goes back 2000 when 1 left*/
    printf("\ttest prev & next limit conditions, further error conditions... ");
    printf("1... ");
    res_buffer_next(buffer_h, 1);  /*move up 1 so prev() must fail for the right reasons*/
    FAIL_ON( 0 == res_buffer_prev(buffer_h, 2000) , return (false) );
    /*why do this first? So we can distinguish totally wrong error handling, from subtle off-by-one condition tested next...*/

   /*error condition: prev goes back 2 when 1 left*/
    printf("2... ");
    FAIL_ON( 0 == res_buffer_prev(buffer_h, 2) , return (false) );

   /*prev goes back 1*/
    printf("3... ");
    FAIL_ON( 0 != res_buffer_prev(buffer_h, 1) , return (false) );

   /*next advances 255*/
    printf("4... ");
    FAIL_ON( 0 != res_buffer_next(buffer_h, 255) , return (false) );

   /*prev goes back 254, then 1*/
    printf("5... ");
    FAIL_ON( 0 != res_buffer_prev(buffer_h, 254) , return (false) );
    FAIL_ON( 0 != res_buffer_prev(buffer_h, 1) , return (false) );

   /*next advances 254*/
    printf("6... ");
    FAIL_ON( 0 != res_buffer_next(buffer_h, 254) , return (false) );

   /*error condition: next advances 2000 when 1 left*/
    printf("7... ");
    FAIL_ON( 0 == res_buffer_next(buffer_h, 2000) , return (false) );
    /*why do this first? So we can distinguish totally wrong error handling, from subtle off-by-one condition tested next...*/

   /*error condition: next advances 2 when 1 left*/
    printf("8... ");
    FAIL_ON( 0 == res_buffer_next(buffer_h, 2) , return (false) );

   /*next advances 1*/
    printf("9... ");
    FAIL_ON( 0 != res_buffer_next(buffer_h, 1) , return (false) );

   /*prev goes back 255*/
    printf("10... ");
    FAIL_ON( 0 != res_buffer_prev(buffer_h, 255) , return (false) );
    printf("Good!\n");
  
   /*test reset on beginning, middle, and end*/
    printf("\treset from start... ");
    FAIL_ON( 0 != res_buffer_reset(buffer_h) , return (false));
    ERRNO_FAIL_ON( res_buffer_get_n(buffer_h) != 256 , return (false));

    printf("\treset from middle... ");
    res_buffer_next(buffer_h, 128);
    FAIL_ON( 0 != res_buffer_reset(buffer_h) , return (false));
    ERRNO_FAIL_ON( res_buffer_get_n(buffer_h) != 256 , return (false));

    printf("\treset from end... ");
    res_buffer_next(buffer_h, 256);
    FAIL_ON( 0 != res_buffer_reset(buffer_h) , return (false));
    ERRNO_FAIL_ON( res_buffer_get_n(buffer_h) != 256 , return (false));
    printf("Good!\n");
  return(true);
}

int appendf()
{
  res_buffer_t* buffer;
   /*initialise basic buffer*/
    printf("\tcreating buffer... ");
    buffer = res_buffer_create(127);
    ERRNO_FAIL_ON( NULL == buffer , return (false));
    FAIL_ON( 128 != res_buffer_get_n(buffer), return (false));  /*if limit not set exactly right, some tests will fail*/
    printf("Good!\n");

   /*try two simple strings*/
    printf("\tbasic string appends... ");
    FAIL_ON( 0 !=
      res_buffer_appendf(buffer, "%s%s%d", "zomg", "hax", 1337),
      return (false) );
    FAIL_ON( 0 !=
      res_buffer_appendf(buffer, "w00!\nyay!"),
      return (false) );
    printf("Good! ");

   /*reset*/
    FAIL_ON( 0 != res_buffer_reset(buffer) , return (false));

   /*verify result*/
    printf("\tcomparing... ");
    FAIL_ON( 0 !=
      strcmp( res_buffer_get(buffer),
              "zomghax1337w00!\nyay!" ),
      return (false));
    printf("Good!\n");

   /*try writing one just 1 character over limit*/
    printf("\tappend just above limit... ");
    FAIL_ON( 0 !=
      res_buffer_appendf(buffer, "%*d", 128, 1),
      return (false) );

   /*reset*/
    FAIL_ON( 0 != res_buffer_reset(buffer) , return (false));

   /*test that limit changed*/
    FAIL_ON( 128 == res_buffer_get_n(buffer), return (false));
    printf("Good!\n");

   /*try writing 1000 characters over the limit*/
    printf("\tappend far above limit... ");
    FAIL_ON( 0 !=
      res_buffer_appendf(buffer, "%*d", 1000, 1),
      return (false) );
    FAIL_ON( 0 != res_buffer_reset(buffer) , return (false));  /*go back to start for count test that comes next*/
    FAIL_ON( 1000 > res_buffer_get_n(buffer) , return (false));  /*can't have really worked if size is still lower than 1000*/
    printf("Good!\n");

   /*NOTE - we cannot test most error conditions, as trying to trigger a realloc fail might crash the system, as might an invalid format specifier*/

   /*destroy buffer*/
    printf("\tdestroying buffer... ");
    FAIL_ON( 0 != res_buffer_destroy(buffer) , return (false));
    printf("Good!\n");

   /*make a tiny buffer - to test realloc mechanism doesn't have any weird (eg divide by zero) flaws*/
    printf("\tcreating tiny buffer... ");
    buffer = res_buffer_create(0);
    ERRNO_FAIL_ON( NULL == buffer , return (false));
    FAIL_ON( 1 != res_buffer_get_n(buffer), return (false));  /*ensure it really is tiny*/
    printf("Good!\n");

   /*add a tiny string*/
    printf("\ttrying slightly bigger appendf... ");
    FAIL_ON( 0 !=
      res_buffer_appendf(buffer, "."),
      return (false) );

   /*check limit expanded*/
    printf("\tverifying buffer has grown... ");
    FAIL_ON( 1 == res_buffer_get_n(buffer), return (false));  /*ensure it has grown to fit string*/
    printf("Good!\n");

   /*destroy tiny buffer*/
    printf("\tdestroying tiny buffer... ");
    FAIL_ON( 0 != res_buffer_destroy(buffer) , return (false));
    printf("Good!\n");

  return(true);
}

