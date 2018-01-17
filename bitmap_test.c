/* bitmap_test.c - unit tests for bitmap.c
 *
 * REQUIRES: bitmap_1
 * TESTS: bitmap_1.0
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include "bitmap.h"
#include "res_err.h"

  int main(void);
  int create_destroy(void);
  int free_take_check_count(void);
  int test_alloc(void);  /*named test_... because alloc is already a function name, don't want to cause problems if linked against stdlib*/
  int resize(void);
  void print_bitmap(res_bitmap_t* bitmap); /*print_ functions used for debugging, not in tests*/
  void print_bitmap_2(res_bitmap_t* bitmap);
  
int main()
{
  srand((unsigned int)time(NULL));
   /*create, destroy*/
    printf("01 - create & destroy bitmaps\n");
    if( 0 != create_destroy() )
    {
      printf("TEST FAIL!\n");
      return(EXIT_FAILURE);
    }

   /*free, take, check*/
    printf("02 - free, take, check, & count operations\n");
    if( 0 != free_take_check_count() )
    {
      printf("TEST FAIL!\n");
      return(EXIT_FAILURE);
    }
    
   /*alloc*/
    printf("03 - alloc\n");
    if( 0 != test_alloc() )
    {
      printf("TEST FAIL!\n");
      return(EXIT_FAILURE);
    }

   /*resize*/
    printf("04 - resize\n");
    if( 0 != resize() )
    {
      printf("TEST FAIL!\n");
      return(EXIT_FAILURE);
    }

  printf("ALL TESTS PASSED!\n");
  return(EXIT_SUCCESS);
}

int create_destroy()
{
  res_bitmap_t* bitmap1;
  res_bitmap_t* bitmap2;
    printf("\tcreating bitmap of size 1... ");
    errno = 0;
    bitmap1 = res_bitmap_create(0);
    if(NULL == bitmap1)
    {
      printf("FAILED - error %s", res_err_string(errno));
      return(1);
    }
    printf("Good!\n");

    printf("\tcreating bitmap of size 1,000,000... ");
    errno = 0;
    bitmap2 = res_bitmap_create(999999);
    if(NULL == bitmap2)
    {
      printf("FAILED - error %s", res_err_string(errno));
      return(1);
    }
    printf("Good!\n");

    printf("\tdestroying first bitmap... ");
    assert(0 == res_bitmap_destroy(bitmap1));
    printf("Good!\n");

    printf("\tdestroying second bitmap... ");
    assert(0 == res_bitmap_destroy(bitmap2));
    printf("Good!\n");
  return(0);
}

int free_take_check_count()
{
  res_bitmap_t* bitmap1;
  uint16_t taken_array[500];
  size_t i;
  size_t j;
   /*create bitmap - size 2001*/
    printf("\tcreating new bitmap, size 2001... ");
    bitmap1 = res_bitmap_create(2000);
    assert(NULL != bitmap1);
    printf("Good!\n");

   /*test error conditions for free, take, check, count*/
    printf("\ttesting error conditions... ");

    assert( 2 == res_bitmap_free(bitmap1, 2001, 0) );
    assert( 3 == res_bitmap_free(bitmap1, 0, 2001) );
    assert( 3 == res_bitmap_free(bitmap1, 100, 1990) );
    assert( 3 == res_bitmap_free(bitmap1, 1900, SIZE_MAX - 100) );

    assert( 2 == res_bitmap_take(bitmap1, 2001, 0) );
    assert( 3 == res_bitmap_take(bitmap1, 0, 2001) );
    assert( 3 == res_bitmap_take(bitmap1, 100, 1990) );
    assert( 3 == res_bitmap_take(bitmap1, 1900, SIZE_MAX - 100) );

    assert( 4 == res_bitmap_check(bitmap1, 2001, 0) );
    assert( 5 == res_bitmap_check(bitmap1, 0, 2001) );
    assert( 5 == res_bitmap_check(bitmap1, 100, 1990) );
    assert( 5 == res_bitmap_check(bitmap1, 1900, SIZE_MAX - 100) );

    assert( RES_BITMAP_ERR == res_bitmap_count(bitmap1, 2001, 0) );
    assert( RES_ERR_BAD_PARAMETER == errno );
    assert( RES_BITMAP_ERR == res_bitmap_count(bitmap1, 0, 2001) );
    assert( RES_ERR_BAD_PARAMETER == errno );
    assert( RES_BITMAP_ERR == res_bitmap_count(bitmap1, 100, 1990) );
    assert( RES_ERR_BAD_PARAMETER == errno );
    assert( RES_BITMAP_ERR == res_bitmap_count(bitmap1, 1900, SIZE_MAX - 100) );
    assert( RES_ERR_BAD_PARAMETER == errno );

    printf("Good!\n");

   /*allocate and free all bits, one by one*/
    printf("\tbasic take, check, free, check test... ");
    for(i=0; i<2001; i++)
    {
      assert( 0 == res_bitmap_check(bitmap1, i, 0) );  /*ensure initial state of bit is 0*/
      assert( 0 == res_bitmap_take(bitmap1, i, 0) );
      assert( 1 == res_bitmap_check(bitmap1, i, 0) );
      assert( 0 == res_bitmap_free(bitmap1, i, 0) );
      assert( 0 == res_bitmap_check(bitmap1, i, 0) );
    }
    printf("Good!\n");

   /*take 500 random bits, check them as added, put locations in array*/
    printf("\ttaking 500 bits at random points in the bitmap... ");
    for(i=0; i<500; i++)
    {
      j = (unsigned)rand() % 2000;
      while(0 != res_bitmap_check(bitmap1, j, 0))
        j = (unsigned)rand() % 2000;
      assert( 0 == res_bitmap_take(bitmap1, j, 0) );
      assert( 1 == res_bitmap_check(bitmap1, j, 0) );
      taken_array[i] = (uint16_t)j;
    }
    printf("Good!\n");

   /*test the whole map, 1 bit at a time - NO more or less than 500 1's!*/
    printf("\ttesting the whole map... ");
    j = 0;
    for(i=0; i<2001; i++)
    {
      assert(res_bitmap_check(bitmap1, i, 0) < 2);
      if(1 == res_bitmap_check(bitmap1, i, 0))
        j++;
    }
    assert(500 == j);
    assert(500 == res_bitmap_count(bitmap1, 0, 2000));
    printf("Good!\n");

   /*free 300 of the bits taken, checking as free'd*/
    printf("\tfreeing first 300 bits taken... ");
    for(i=0; i<300; i++)
    {
      j = taken_array[i];
      assert( 0 == res_bitmap_free(bitmap1, j, 0) );
      assert( 0 == res_bitmap_check(bitmap1, j, 0) );
    }
    printf("Good!\n");

   /*test the whole map, 1 bit at a time - NO more or less than 200 1's!*/
    printf("\ttesting the whole map... ");
    j = 0;
    for(i=0; i<2001; i++)
    {
      assert(res_bitmap_check(bitmap1, i, 0) < 2);
      if(1 == res_bitmap_check(bitmap1, i, 0))
        j++;
    }
    assert(200 == j);
    assert(200 == res_bitmap_count(bitmap1, 0, 2000));
    printf("Good!\n");

   /*destroy the bitmap, create a new one*/
    assert( 0 == res_bitmap_destroy(bitmap1) );
    errno = 0;
    bitmap1 = res_bitmap_create(2000);
    assert(NULL != bitmap1);

   /*do a 1-0 pattern over 70 bits, then check*/
    printf("\tallocating 70 bits in a 1-0 pattern... ");
    for(i=0; i<70; i+=2)
    {
      assert(0 == res_bitmap_take(bitmap1, i, 0));
      assert(0 == res_bitmap_free(bitmap1, i+1, 0));
    }
    printf("testing pattern... ");
    for(i=0; i<70; i+=2)
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
      assert(0 == res_bitmap_check(bitmap1, i+1, 0));
    }
    assert(4 == res_bitmap_count(bitmap1, 1, 7));
    assert(0 == res_bitmap_free(bitmap1, 0, 69));
    printf("Good!\n");

   /*multi-bit tests need: <8 bits <32bits <64bits >64bits*/
    printf("\ttesting multi-bit take...\n");
    printf("\t\t7 bits, allocating... ");
    assert(0 == res_bitmap_take(bitmap1, 0, 6));
    for(i=0; i<7; i++)  /*check done 1 bit at a time*/
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
    }
    assert(0 == res_bitmap_check(bitmap1, 7, 0));  /*check last bit NOT allocated*/
    assert(7 == res_bitmap_count(bitmap1, 0, 7));
    printf("clearing... ");
    assert(0 == res_bitmap_free(bitmap1, 0, 6));  /*clear bitmap*/
    for(i=0; i<7; i++)  /*check done 1 bit at a time*/
    {
      assert(0 == res_bitmap_check(bitmap1, i, 0));
    }
    assert(0 == res_bitmap_count(bitmap1, 0, 7));
    printf("Good!\n");

    printf("\t\t29 bits, allocating... ");
    assert(0 == res_bitmap_take(bitmap1, 0, 28));
    for(i=0; i<29; i++)  /*check done 1 bit at a time*/
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
    }
    assert(0 == res_bitmap_check(bitmap1, 29, 0));  /*check last bit NOT allocated*/
    printf("clearing... ");
    assert(0 == res_bitmap_free(bitmap1, 0, 28));  /*clear bitmap*/
    for(i=0; i<29; i++)  /*check done 1 bit at a time*/
    {
      assert(0 == res_bitmap_check(bitmap1, i, 0));
    }
    printf("Good!\n");

    printf("\t\t62 bits, allocating... ");
    assert(0 == res_bitmap_take(bitmap1, 0, 61));
    for(i=0; i<62; i++)  /*check done 1 bit at a time*/
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
    }
    assert(0 == res_bitmap_check(bitmap1, 62, 0));  /*check last bit NOT allocated*/
    printf("clearing... ");
    assert(0 == res_bitmap_free(bitmap1, 0, 61));  /*clear bitmap*/
    for(i=0; i<62; i++)  /*check done 1 bit at a time*/
    {
      assert(0 == res_bitmap_check(bitmap1, i, 0));
    }
    printf("Good!\n");

    printf("\t\t66 bits, allocating... ");
    assert(0 == res_bitmap_take(bitmap1, 0, 65));
    for(i=0; i<66; i++)  /*check done 1 bit at a time*/
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
    }
    assert(0 == res_bitmap_check(bitmap1, 66, 0));  /*check last bit NOT allocated*/
    printf("clearing... ");
    assert(0 == res_bitmap_free(bitmap1, 0, 65));  /*clear bitmap*/
    for(i=0; i<66; i++)  /*check done 1 bit at a time*/
    {
      assert(0 == res_bitmap_check(bitmap1, i, 0));
    }
    printf("Good!\n");

   /*do a 111-000 pattern over 70 bits, and a 0x65 then 1x65, then check one at a time*/
    printf("\tcreating multi-bit patterns...\n");
    res_bitmap_take(bitmap1, 0, 199);
    for(i=0; i<70; i+=6)
    {
      assert(0 == res_bitmap_take(bitmap1, i, 2));
      assert(0 == res_bitmap_free(bitmap1, i+3, 2));
    }
    assert(0 == res_bitmap_free(bitmap1, 70, 64));
    assert(0 == res_bitmap_take(bitmap1, 135, 64));

    printf("\t\ttesting bit-by-bit... ");
    for(i=0; i<70; i+=6)
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
      assert(1 == res_bitmap_check(bitmap1, i+1, 0));
      assert(1 == res_bitmap_check(bitmap1, i+2, 0));
      assert(0 == res_bitmap_check(bitmap1, i+3, 0));
      if(i+3>=70) break;  /*needed so that the loop doesn't run over 70*/
      assert(0 == res_bitmap_check(bitmap1, i+4, 0));
      assert(0 == res_bitmap_check(bitmap1, i+5, 0));
    }
    for(i=70; i<135; i++)
    {
      assert(0 == res_bitmap_check(bitmap1, i, 0));
    }
    for(i=135; i<200; i++)
    {
      assert(1 == res_bitmap_check(bitmap1, i, 0));
    }
    assert(0 == res_bitmap_check(bitmap1, 200, 0)); /*check last bit not marked*/
    printf("Good!\n");

   /*multi-bit check obstacle course - using above layout*/
    printf("\t\ttesting above with multi-bit check... ");
    /*1st 3 = 1*/
    assert(1 == res_bitmap_check(bitmap1, 0, 2));
    /*1st 4 = 2*/
    assert(2 == res_bitmap_check(bitmap1, 0, 3));
    /*3-6 = 0*/
    assert(0 == res_bitmap_check(bitmap1, 3, 2));
    /*30-33 = 2*/
    assert(2 == res_bitmap_check(bitmap1, 30, 4));
    /*70-134 = 0*/
    assert(0 == res_bitmap_check(bitmap1, 70, 64));
    /*68-134 = 2 - has to be 68 because last sequence ends on a 0 anyway*/
    assert(2 == res_bitmap_check(bitmap1, 68, 66));
    /*70-135 = 2*/
    assert(2 == res_bitmap_check(bitmap1, 70, 65));
    /*135-199 = 1*/
    assert(1 == res_bitmap_check(bitmap1, 135, 64));
    /*135-200 = 2*/
    assert(2 == res_bitmap_check(bitmap1, 135, 65));
    /*100-150 = 2*/
    assert(2 == res_bitmap_check(bitmap1, 100, 50));
    printf("Good!\n");

   /*destroy the bitmap*/
    printf("\tdestroying bitmap... ");
    assert(0 == res_bitmap_destroy(bitmap1));
    printf("Good!\n");

  return(0);
}

int test_alloc()
{
  res_bitmap_t* bitmap1;
  size_t i;
  size_t j;
  size_t alloc_array[6];
   /*create bitmap - size 2001*/
    printf("\tcreating new bitmap, size 2001... ");
    bitmap1 = res_bitmap_create(2000);
    assert(NULL != bitmap1);
    printf("Good!\n");

  /*basic error conditions (block too big)*/
    printf("\ttesting simple error conditions... ");
    errno = 0;  /*reset errno for proper testing*/
    assert(RES_BITMAP_ERR == res_bitmap_alloc(bitmap1, 2001));
    assert(RES_ERR_BAD_PARAMETER == errno);
    printf("Good!\n");

  /*basic test*/
    printf("\tchecking basic alloc... ");
    alloc_array[0] = res_bitmap_alloc(bitmap1, 0);
    alloc_array[1] = res_bitmap_alloc(bitmap1, 0);
    alloc_array[2] = res_bitmap_alloc(bitmap1, 0);
    assert(RES_BITMAP_ERR != alloc_array[0]);  /*did it work?*/
    assert(RES_BITMAP_ERR != alloc_array[1]);
    assert(RES_BITMAP_ERR != alloc_array[2]);
    assert(alloc_array[0] != alloc_array[1]);  /*are they all different?*/
    assert(alloc_array[0] != alloc_array[2]);
    assert(alloc_array[1] != alloc_array[2]);
    j = 0;  /*is the expected number of blocks marked in the whole map?*/
    for(i=0; i<2001; i++)
    {
      assert(res_bitmap_check(bitmap1, i, 0) < 2);
      if(1 == res_bitmap_check(bitmap1, i, 0))
        j++;
    }
    assert(3 == j);
    printf("Good!\n");

    printf("\tchecking multi-bit alloc... ");
    alloc_array[3] = res_bitmap_alloc(bitmap1, 1);
    alloc_array[4] = res_bitmap_alloc(bitmap1, 62);
    alloc_array[5] = res_bitmap_alloc(bitmap1, 101);
    assert(RES_BITMAP_ERR != alloc_array[3]);  /*did it work?*/
    assert(RES_BITMAP_ERR != alloc_array[4]);
    assert(RES_BITMAP_ERR != alloc_array[5]);
    j = 0;  /*is the expected number of blocks marked in the whole map? This*/
    for(i=0; i<2001; i++)  /*should also ensure that take was done right too*/
    {
      assert(res_bitmap_check(bitmap1, i, 0) < 2);
      if(1 == res_bitmap_check(bitmap1, i, 0))
        j++;
    }
    assert(170 == j);
    printf("Good!\n");

  /*failure test - what if there's no space at all? No holes big enough?*/
    printf("\ttesting bitmap full error conditions... ");
    errno = 0;  /*reset errno for proper testing*/
    assert(0 == res_bitmap_take(bitmap1, 0, 2000)); /*test single alloc with no holes*/
    assert(RES_BITMAP_ERR == res_bitmap_alloc(bitmap1, 0));
    assert(RES_ERR_NO_MATCH == errno);

    assert(0 == res_bitmap_free(bitmap1, 0, 66)); /*next test multi-bit alloc with 1 less space than needed available*/
    assert(0 == res_bitmap_free(bitmap1, 100, 66));
    assert(0 == res_bitmap_free(bitmap1, 703, 66));

    errno = 0;  /*reset errno for proper testing*/
    assert(RES_BITMAP_ERR == res_bitmap_alloc(bitmap1, 67));
    assert(RES_ERR_NO_MATCH == errno);
    printf("Good!\n");

   /*destroy the bitmap*/
    printf("\tdestroying bitmap... ");
    assert(0 == res_bitmap_destroy(bitmap1));
    printf("Good!\n");
    
  /*NOTE that we do NOT test if the allocator can find space in hard conditions
   * (eg only one match). This is because allocators may be designed to be fast
   * but imperfect, and this is ok. They just need to only return space that
   * isn't already taken*/
  return(0);
}

int resize(void)
{
  res_bitmap_t *bitmap1;
   /*create bitmap*/
    printf("\tcreating new bitmap, size 60... ");
    bitmap1 = res_bitmap_create(59);
    assert(NULL != bitmap1);
    assert(59 == res_bitmap_get_size(bitmap1));
    printf("Good!\n");

   /*write initial test data - 3 bits at top and bottom*/
    assert( 0 == res_bitmap_take(bitmap1, 57, 2) );
    assert( 0 == res_bitmap_take(bitmap1, 0, 2) );
    
   /*try a few new sizes - check size right, information preserved at edges, new bits zero'ed*/
    printf("\tresizing to 64 bits... ");
    assert( 0 == res_bitmap_resize(bitmap1, 63) );
    assert( 63 == res_bitmap_get_size(bitmap1) );
    assert( 6 == res_bitmap_count(bitmap1, 0, 63) );
    assert( 0 == res_bitmap_check(bitmap1, 60, 3) );
    assert( 0 == res_bitmap_check(bitmap1, 3, 53) );
    assert( 1 == res_bitmap_check(bitmap1, 0, 2) );
    assert( 1 == res_bitmap_check(bitmap1, 57, 2) );
    printf("Good!\n");

    printf("\tresizing to 67 bits... ");
    assert( 0 == res_bitmap_resize(bitmap1, 66) );
    assert( 66 == res_bitmap_get_size(bitmap1) );
    assert( 6 == res_bitmap_count(bitmap1, 0, 63) );
    assert( 0 == res_bitmap_check(bitmap1, 60, 6) );
    assert( 0 == res_bitmap_check(bitmap1, 3, 53) );
    assert( 1 == res_bitmap_check(bitmap1, 0, 2) );
    assert( 1 == res_bitmap_check(bitmap1, 57, 2) );
    printf("Good!\n");

    printf("\tresizing to 58 bits... ");
    assert( 0 == res_bitmap_resize(bitmap1, 57) );
    assert( 57 == res_bitmap_get_size(bitmap1) );
    assert( 4 == res_bitmap_count(bitmap1, 0, 57) );
    assert( 0 == res_bitmap_check(bitmap1, 3, 53) );
    assert( 1 == res_bitmap_check(bitmap1, 0, 2) );
    assert( 1 == res_bitmap_check(bitmap1, 57, 0) );
    printf("Good!\n");

   /*destroy the bitmap*/
    printf("\tdestroying bitmap... ");
    assert(0 == res_bitmap_destroy(bitmap1));
    printf("Good!\n");
  return(0);
}

