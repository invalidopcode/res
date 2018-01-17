/* array_test.c - unit tests for array.c
 *
 * REQUIRES: array_2
 * TESTS: array_2.0
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "array.h"
#include "res_err.h"

 int main(void);
 int destroy_create(void);
 int add_get_change(void);
 int copy_resize(void);
 int search(void);

int main()
{
   /*Create, destroy test*/
    printf("TEST 01 - create & destroy\n");
    if(0 != destroy_create())
      return(EXIT_FAILURE);

   /*add, get, change test*/
    printf("TEST 02 - add, get, change\n");
    if(0 != add_get_change())
      return(EXIT_FAILURE);

   /*copy, resize test*/
    printf("TEST 03 - copy & resize\n");
    if(0 != copy_resize())
      return(EXIT_FAILURE);

   /*search test*/
    printf("TEST 04 - search\n");
    if(0 != search())
      return(EXIT_FAILURE);

  printf("ALL TESTS PASSED!\n");
  return(EXIT_SUCCESS);
}

int destroy_create()
{
  ushort retval;
  res_array_t *array1;
  res_array_t *array2;

    printf("\tcreating a typed array with 1 entry... ");
    errno = 0;
    array1 = res_array_create(0, RES_ARRAY_TYPED);
    if(NULL == array1)
    {
       printf("FAILED - error %s", res_err_string(errno));
       return(1);
    }
    printf("Good!\n");

    printf("\tcreating a simple array with 20,000,000 entries... ");
    errno = 0;
    array2 = res_array_create(20000000, RES_ARRAY_SIMPLE);
    if(NULL == array2)
    {
       printf("FAILED - error %s", res_err_string(errno));
       return(1);
    }
    printf("Good!\n");

    printf("\tdestroying first array... ");
    retval = res_array_destroy(array1);
    if(0 != retval)
    {
      printf("FAILED - error code %u", retval);
      return(1);
    }
    printf("Good!\n");

    printf("\tdestroying second array... ");
    retval = res_array_destroy(array2);
    if(0 != retval)
    {
      printf("FAILED - error code %u", retval);
      return(1);
    }
    printf("Good!\n");

  return(0);
}

int add_get_change()
{
  size_t i;
  ushort retval;
  void* p_tmp;
  RES_TYPE tmp_type;
   /*create a typed array of 100 entries, simple array of 20*/
    printf("\tcreating arrays for test... ");
    res_array_t *array1 = res_array_create(99, RES_ARRAY_TYPED);
    assert(NULL != array1);

    res_array_t *array2 = res_array_create(19, RES_ARRAY_SIMPLE);
    assert(NULL != array2);
    printf("Good!\n");

   /*check correct handling of NULL resource pointer*/
    printf("\tchecking correct handling of NULL resource pointer... ");
    assert(0 == res_array_add_entry(array1, 0, NULL, 0));
    errno = 1;
    assert(NULL == res_array_get_resource(array1, 0));
    assert(0 == errno);
    assert(0 == res_array_remove_entry(array1, 0));
    printf("Good!\n");

   /*add 100 entries to array1, 20 to array 2*/
    printf("\tadding entries... ");
    for(i=0; i<100; i++)
    {
      retval = res_array_add_entry(array1, i, (void*)0x100, 0x10);
      assert(0 == retval);
    }

    for(i=0; i<20; i++)
    {
      retval = res_array_add_entry(array2, i, (void*)0x200, 0x10);
      assert(0 == retval);
    }
    printf("Good!\n");

   /*verify both using get_resource, & get_type for array1*/
    printf("\tverifying arrays using get_type, get_resource... ");
    for(i=0; i<100; i++)
    {
      p_tmp = res_array_get_resource(array1, i);
      assert((void *)0x100 == p_tmp);
      tmp_type = res_array_get_type(array1, i);
      assert(0x10 == tmp_type);
    }

    for(i=0; i<20; i++)
    {
      p_tmp = res_array_get_resource(array2, i);
      assert((void *)0x200 == p_tmp);
    }
    printf("Good!\n");

   /*try to get type for array 2*/
    printf("\ttrying to get_type on simple array (MUST fail)... ");
    errno = 0;
    tmp_type = res_array_get_type(array2, 10);
    assert(RES_TYPE_ERR == tmp_type);
    assert(errno == RES_ERR_INCOMPATIBLE_RESOURCE);
    printf("Error string - '%s' Good!\n", res_err_string(errno));

   /*destroy array 2*/
    printf("\tdestroying the simple array... ");
    assert(0 == res_array_destroy(array2));
    printf("Good!\n");

   /*try to add to entry 101*/
    printf("\ttrying to add out-of-bounds entry (MUST fail)... ");
    retval = res_array_add_entry(array1, 100, (void *)0x100, 0x10);
    assert(2 == retval);
    printf("Good!\n");

   /*try to remove entry 101*/
    printf("\ttrying to remove out-of-bounds entry (MUST fail)... ");
    retval = res_array_remove_entry(array1, 100);
    assert(2 == retval);
    printf("Good!\n");

   /*remove entry 10, then get resource*/
    printf("\tremoving entry 10 from array... ");
    retval = res_array_remove_entry(array1, 9);
    assert(0 == retval);
    assert(NULL == res_array_get_resource(array1, 9));
    assert(0 == res_array_get_type(array1, 9));
    printf("Good!\n");

   /*change type of entry 11, then get*/
    printf("\tchanging the type of an entry... ");
    retval = res_array_ch_type(array1, 10, 0x30);
    assert(0 == retval);
    assert( 0x30 == res_array_get_type(array1, 10) );
    printf("Good!\n");

   /*change resource of entry 0, then get*/
    printf("\changing the resource of an entry... ");
    retval = res_array_ch_resource(array1, 0, (void *)0x5000);
    assert(0 == retval);
    assert( (void *)0x5000 == res_array_get_resource(array1, 0) );
    printf("Good!\n");

   /*try to get entry 200*/
    printf("\ttrying to get non-existent entry (MUST fail)... ");
    printf("1... ");
    assert( RES_TYPE_ERR == res_array_get_type(array1, 199) );
    printf("2... ");
    assert( NULL == res_array_get_resource(array1, 199) );
    printf("Good!\n");

   /*try to change entry 101*/
    printf("\ttrying to change non-existent entry (MUST fail)... ");
    printf("1... ");
    assert( 2 == res_array_ch_type(array1, 100, 0x15) );
    printf("2... ");
    assert( 2 == res_array_ch_resource(array1, 100, (void *)0x20000) );  
    printf("Good!\n");

   /*destroy array*/
    printf("\tdestroying array... ");
    assert( 0 == res_array_destroy(array1) );
    printf("Good!\n");

  return(0);
}

int copy_resize()
{
  size_t i;
  res_array_t* array1;
  res_array_t* array2;
  res_array_t* array3;
  ushort retval;
   /*create typed array with 300 entries, add entries*/
    printf("\tcreating full array & empty array... ");
    array1 = res_array_create(300, RES_ARRAY_TYPED);
    assert(NULL != array1);
    for(i=0; i<300; i++)
      assert( 0 == res_array_add_entry(array1, i, (uint8_t *)0x100 + i, 0x10) );
    array2 = res_array_create(300, RES_ARRAY_TYPED);
    assert(NULL != array2);
    printf("Good!\n");

   /*copy array1 into array2, verify*/
    printf("\tcopying same-sized arrays... ");
    assert( 0 == res_array_copy(array2, array1) );
    for(i=0; i<300; i++)
      assert( ((uint8_t *)0x100 + i) == res_array_get_resource(array2, i) );
    assert( 0 == res_array_destroy(array2) );
    printf("Good!\n");    

   /*create simple array of 300 entries, try to copy in array1*/
    printf("\tattempting to copy typed array into simple array (MUST fail)... ");
    array3 = res_array_create(300, RES_ARRAY_SIMPLE);
    assert(NULL != array3);
    assert( 0 != res_array_copy(array3, array1) );
    assert( 0 == res_array_destroy(array3) );
    printf("Good!\n");

   /*copy array 1 into 400 entry and 200 entry arrays, verify*/
    printf("\tattempting to copy into bigger and smaller arrays... ");
    array2 = res_array_create(400, RES_ARRAY_TYPED);
    assert(NULL != array2);
    array3 = res_array_create(200, RES_ARRAY_TYPED);
    assert(NULL != array3);
    assert( 0 == res_array_copy(array2, array1) );
    for(i=0; i<300; i++)
      assert( ((uint8_t *)0x100 + i) == res_array_get_resource(array2, i) );
    for(i=300; i<400; i++)
      assert( NULL == res_array_get_resource(array2, i) );
    assert( 0 == res_array_copy(array3, array1) );
    for(i=0; i<200; i++)
      assert( ((uint8_t *)0x100 + i) == res_array_get_resource(array2, i) );
    printf("Good!\n");

   /*make array1 bigger, verify*/
    printf("\tresizing a 300 entry array to 400 entries, then verifying... ");
    retval = res_array_resize(array1, 400);
    if(0 != retval)
    {
      printf("ERROR - %u\n", retval);
      if(2 == retval)
        perror("");
      if(3 == retval)
        printf("copy error %u\n", errno);
    }
    assert( 0 == retval );
    for(i=0; i<300; i++)
      assert( ((uint8_t *)0x100 + i) == res_array_get_resource(array1, i) );
    for(i=300; i<400; i++)
      assert( NULL == res_array_get_resource(array1, i) );
    printf("Good!\n");
    
   /*make 200 entry array into 100 entry array, verify*/
    printf("\tresizing a 400 entry array to 200 entries, then verifying... ");
    assert( 0 == res_array_resize(array1, 200) );
    for(i=0; i<200; i++)
      assert( ((uint8_t *)0x100 + i) == res_array_get_resource(array1, i) );
    printf("Good!\n");

   /*destroy arrays*/
    printf("\tdestroying remaining arrays... ");
    assert( 0 == res_array_destroy(array1) );
    assert( 0 == res_array_destroy(array2) );
    assert( 0 == res_array_destroy(array3) );
    printf("Good!\n");
  return(0);
}

int search()
{
  res_array_t* array1;
  res_array_t* array2;
  size_t i;
   /*create 100 item array, containing resources 0x1000 + i*2, type 0x10 + i*2*/
    printf("\tcreating array... ");
    array1 = res_array_create(99, RES_ARRAY_TYPED);
    assert(NULL != array1);
    array2 = res_array_create(99, RES_ARRAY_SIMPLE);
    assert(NULL != array2);
    for(i=0; i<100; i++)
    {
      assert( 0 == res_array_add_entry(array1, i, (void *)(0x1000 + i*2), 0x10 + i*2) );
      assert( 0 == res_array_add_entry(array2, i, (void *)(0x1000 + i*2), 0) );
    }
    printf("Good!\n");

   /*full search for beginning and end items*/
    printf("\tsearching for known start & finish entries... ");
    assert( 0 == res_array_search_by_resource(array1, 0, 0, (void*)0x1000) );
    assert( 0 == res_array_search_by_type(array1, 0, 0, 0x10) );
    assert( 0 == res_array_search_by_resource(array2, 0, 0, (void*)0x1000) );
    assert( RES_ARRAY_ERR == res_array_search_by_type(array2, 0, 0, 0x10) );
    i = 99 * 2;
    if(99 != res_array_search_by_resource(array1, 0, 0, (void*)(0x1000 + i)))
      printf("%s", res_err_string(errno));
    assert( 99 == res_array_search_by_resource(array1, 0, 0, (void*)(0x1000 + i)) );
    assert( 99 == res_array_search_by_type(array1, 0, 0, 0x10 + i) );
    assert( 99 == res_array_search_by_resource(array2, 0, 0, (void*)(0x1000 + i)) );
    assert( RES_ARRAY_ERR == res_array_search_by_type(array2, 0, 0, 0x10 + i) );
    printf("Good!\n");

   /*bounded search for start and end then for known items*/
    printf("\tbounded searches for known entries... ");
    assert( 0 == res_array_search_by_resource(array1, 0, 99, (void*)0x1000) );
    assert( 0 == res_array_search_by_type(array1, 0, 99, 0x10) );
    assert( 0 == res_array_search_by_resource(array2, 0, 99, (void*)0x1000) );
    i = 99 * 2;
    assert( 99 == res_array_search_by_resource(array1, 0, 99, (void*)(0x1000 + i)) );
    assert( 99 == res_array_search_by_type(array1, 0, 99, 0x10 + i) );
    assert( 99 == res_array_search_by_resource(array2, 0, 99, (void*)(0x1000 + i)) );
    i = 20 * 2;
    assert( 20 == res_array_search_by_resource(array1, 0, 20, (void*)(0x1000 + i)) );
    assert( 20 == res_array_search_by_type(array1, 0, 20, 0x10 + i) );
    assert( 20 == res_array_search_by_resource(array2, 0, 20, (void*)(0x1000 + i)) );
    i = 11 * 2;
    /*NOTE - need to have multiple resources that match in range in order to test properly*/
    assert( 0 == res_array_ch_resource(array1, 14, (void*)(0x1000 + i)) );
    assert( 0 == res_array_ch_type(array1, 14, 0x10 + i) );
    assert( 0 == res_array_ch_resource(array2, 14, (void*)(0x1000 + i)) );
    assert( 11 == res_array_search_by_resource(array1, 0, 20, (void*)(0x1000 + i)) );
    assert( 11 == res_array_search_by_type(array1, 0, 20, 0x10 + i) );
    assert( 11 == res_array_search_by_resource(array2, 0, 20, (void*)(0x1000 + i)) );
    i = 48 * 2;
    /*NOTE - need to have other resources that match, but out of range, in order to test properly...*/
    assert( 0 == res_array_ch_resource(array1, 30, (void*)(0x1000 + i)) );
    assert( 0 == res_array_ch_type(array1, 30, 0x10 + i) );
    assert( 0 == res_array_ch_resource(array2, 30, (void*)(0x1000 + i)) );
    assert( 0 == res_array_ch_resource(array1, 78, (void*)(0x1000 + i)) );
    assert( 0 == res_array_ch_type(array1, 78, 0x10 + i) );
    assert( 0 == res_array_ch_resource(array2, 78, (void*)(0x1000 + i)) );
    assert( 48 == res_array_search_by_resource(array1, 31, 60, (void*)(0x1000 + i)) );
    assert( 48 == res_array_search_by_type(array1, 31, 60, 0x10 + i) );
    assert( 48 == res_array_search_by_resource(array2, 31, 60, (void*)(0x1000 + i)) );
    printf("Good!\n");

   /*backwards of above*/
    printf("\treverse of above bound searches... ");
    assert( 0 == res_array_search_by_resource(array1, 99, 0, (void*)0x1000) );
    assert( 0 == res_array_search_by_type(array1, 99, 0, 0x10) );
    i = 99 * 2;
    assert( 99 == res_array_search_by_resource(array1, 99, 0, (void*)(0x1000 + i)) );
    assert( 99 == res_array_search_by_type(array1, 99, 0, 0x10 + i) );
    assert( 99 == res_array_search_by_resource(array2, 99, 0, (void*)(0x1000 + i)) );
    i = 20 * 2;
    assert( 20 == res_array_search_by_resource(array1, 20, 0, (void*)(0x1000 + i)) );
    assert( 20 == res_array_search_by_type(array1, 20, 0, 0x10 + i) );
    assert( 20 == res_array_search_by_resource(array2, 20, 0, (void*)(0x1000 + i)) );
    i = 11 * 2;
    assert( 11 == res_array_search_by_resource(array1, 20, 0, (void*)(0x1000 + i)) );
    assert( 11 == res_array_search_by_type(array1, 20, 0, 0x10 + i) );
    assert( 11 == res_array_search_by_resource(array2, 20, 0, (void*)(0x1000 + i)) );
    i = 48 * 2;
    assert( 48 == res_array_search_by_resource(array1, 60, 31, (void*)(0x1000 + i)) );
    assert( 48 == res_array_search_by_type(array1, 60, 31, 0x10 + i) );
    assert( 48 == res_array_search_by_resource(array2, 60, 31, (void*)(0x1000 + i)) );
    printf("Good!\n");

   /*error reporting - stop & start out of range, */
    printf("\terror reporting - bad parameters... ");
    errno = 0;
    assert( RES_ARRAY_ERR == res_array_search_by_resource(array1, 0, 200, NULL) );
    assert(RES_ERR_BAD_PARAMETER == errno);
    assert( RES_ARRAY_ERR == res_array_search_by_type(array1, 0, 200, 0x00) );
    assert(RES_ERR_BAD_PARAMETER == errno);
    assert( RES_ARRAY_ERR == res_array_search_by_resource(array2, 100, 0, NULL) );
    assert(RES_ERR_BAD_PARAMETER == errno);
    assert( RES_ARRAY_ERR == res_array_search_by_type(array2, 100, 0, 0x00) );
    assert(RES_ERR_BAD_PARAMETER == errno);
    printf("Good!\n");

   /*error reporting - no match, no match in range*/
    printf("\terror reporting - no match in array, no match in range... ");
    errno = 0;
    assert( RES_ARRAY_ERR == res_array_search_by_resource(array1, 0, 0, (void *)0x909) );
    assert(RES_ERR_NO_MATCH == errno);
    assert( RES_ARRAY_ERR == res_array_search_by_type(array1, 0, 0, 0x7) );
    assert(RES_ERR_NO_MATCH == errno);
    i = 87 * 2;
    assert( RES_ARRAY_ERR  == res_array_search_by_resource(array2, 60, 31, (void*)(0x1000 + i)) );
    assert(RES_ERR_NO_MATCH == errno);
    assert( RES_ARRAY_ERR == res_array_search_by_type(array1, 60, 31, 0x10 + i) );
    assert(RES_ERR_NO_MATCH == errno);
    i = 30 * 2;
    assert( RES_ARRAY_ERR  == res_array_search_by_resource(array1, 60, 31, (void*)(0x1000 + i)) );
    assert(RES_ERR_NO_MATCH == errno);
    assert( RES_ARRAY_ERR == res_array_search_by_type(array1, 60, 31, 0x10 + i) );
    assert(RES_ERR_NO_MATCH == errno);
    printf("Good!\n");

   /*error reporting - search by type on simple array*/
    printf("\terror reporting - search by type on simple array... ");
    errno = 0;
    assert( RES_ARRAY_ERR == res_array_search_by_type(array2, 0, 0, 0x10) );
    assert( RES_ERR_INCOMPATIBLE_RESOURCE == errno );
    printf("Good!\n");
   
  return(0);
}
