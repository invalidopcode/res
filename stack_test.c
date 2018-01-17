/* stack_test_c - unit tests for stack.c
 *
 * REQUIRES: stack_1
 * TESTS: stack_1.0
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
#include "stack.h"
#include "res_err.h"

  int main(void);
  int create_destroy(void);  /*get_size, etc used and tested throughout*/
  int push_pop_get_change(void);
  int pushx_popx(void);
  int resize(void);

int main()
{
   /*run tests*/
    printf("01 - create & destroy\n");
    if (0 != create_destroy())
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

    printf("02 - push, pop, get & change\n");
    if (0 != push_pop_get_change())
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

    printf("03 - pushx, popx\n");
    if (0 != pushx_popx())
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

    printf("04 - resize\n");
    if (0 != resize())
    {
      printf("TEST_FAIL!\n");
      return(EXIT_FAILURE);
    }

  printf("ALL TESTS PASSED!\n");
  return(EXIT_SUCCESS);
}

int create_destroy(void)
{
  res_stack_t* stack1;
  res_stack_t* stack2;
   /*make a very small stack*/
    printf("\tcreating stack of size 1... ");
    errno = 0;
    stack1 = res_stack_create(0);
    if (NULL == stack1)
    {
      printf("FAILED - error %s\n", res_err_string(errno));
      return(1);
    }
    printf("Good!\n");

   /*make a massive stack*/
    printf("\tcreating a stack of size 1,000,000... ");
    errno = 0;
    stack2 = res_stack_create(999999);
    if (NULL == stack2)
    {
      printf("FAILED - error %s\n", res_err_string(errno));
      return(1);
    }
    printf("Good!\n");

   /*destroy the stacks*/
    printf("\tdestroying stacks... ");
    printf("1... ");
    assert(0 == res_stack_destroy(stack1));
    printf("2... ");
    assert(0 == res_stack_destroy(stack2));
    printf("Good!\n");
  return(0);
}

int push_pop_get_change(void)
{
  res_stack_t* stack1;
  size_t i;
   /*create stack for tests*/
    stack1 = res_stack_create(24);
    assert(NULL != stack1);

   /*test error conditions & edge conditions*/
    printf("\ttesting edges... ");
    assert(2 == res_stack_change(stack1, 0, (void*) 0xdeadb33f));
    errno = 0;
    assert(NULL == res_stack_pop(stack1));
    assert(RES_ERR_STACK_EMPTY == errno);
    errno = 0;
    assert(NULL == res_stack_get(stack1, 0));
    assert(RES_ERR_NOT_FOUND == errno);
    assert(0 == res_stack_push(stack1, (void*) 0xdeadbeef));
    assert((void*)0xdeadbeef == res_stack_get(stack1, 0));
    assert(1 == res_stack_get_entries(stack1));
    assert(0 == res_stack_change(stack1, 0, (void*) 0xdeadb33f));
    assert(1 == res_stack_get_entries(stack1));
    assert((void*)0xdeadb33f == res_stack_pop(stack1));
    assert(0 == res_stack_get_entries(stack1));

    for (i=0; i<=24; i++)  /*fill up stack*/
      assert(0 == res_stack_push(stack1, NULL));  /*if this errors, probably an off-by one error stack full check*/
    assert(25 == res_stack_get_entries(stack1));
    assert(2 == res_stack_push(stack1, (void*) 0xdeadbeef));
    errno = 1;
    assert(NULL == res_stack_get(stack1, 24));
    assert(0 == errno);
    assert(0 == res_stack_change(stack1, 24, (void*) 1));
    assert(2 == res_stack_change(stack1, 25, (void*) 1));
    assert((void*)1 == res_stack_get(stack1, 24));
    errno = 0;
    assert(NULL == res_stack_get(stack1, 25));
    assert(RES_ERR_NOT_FOUND == errno);
    assert(0 == res_stack_change(stack1, 24, NULL));
    errno = 1;
    assert(NULL == res_stack_pop(stack1));
    assert(0 == errno);
    assert(24 == res_stack_get_entries(stack1));

    for (i=0; i<=23; i++) /*pop all the stack, 1 already popped just now*/
    {
      errno = 1;
      assert(NULL == res_stack_pop(stack1));
      assert(0 == errno);
    }
    assert(0 == res_stack_get_entries(stack1));

    printf("Good!\n");
  
   /*push 10 items, get them, change one, pop all*/
    printf("\ttesting ordering... ");
    for (i=0; i<=9; i++)
      assert(0 == res_stack_push(stack1, (void*) i));
    assert(10 == res_stack_get_entries(stack1));

    errno = 0;
    for (i=0; i<=9; i++)
    {
      assert((void*)i == res_stack_get(stack1, i));
      assert(0 == errno);
    }

    assert(0 == res_stack_change(stack1, 4, (void*)101));
    assert((void*)101 == res_stack_get(stack1, 4));

    assert(2 == res_stack_change(stack1, 10, (void*)101));
    errno = 0;
    assert(NULL == res_stack_get(stack1, 10));
    assert(RES_ERR_NOT_FOUND == errno);

    for (i=9; i>4; i--)
      assert((void*)i == res_stack_pop(stack1));
    assert((void*)101 == res_stack_pop(stack1));
    for (i=3; i>0; i--)
      assert((void*)i == res_stack_pop(stack1));
    errno = 0;  /*better to do 0 separately, for loop problem in that 0-- loops around instead of going to -1!*/
    assert((void*)0 == res_stack_pop(stack1));
    assert(0 == errno);
    assert(0 == res_stack_get_entries(stack1));

    printf("Good!\n");

   /*destroy stack*/
    assert(0 == res_stack_destroy(stack1));
  return(0);
}

int pushx_popx(void)
{
  res_stack_t* stack1;
  size_t i;
   /*create stack for testing*/
    stack1 = res_stack_create(24);
    assert(NULL != stack1);
    assert(0 == res_stack_push(stack1, (void*)0x1badbabe));
    assert(0 == res_stack_push(stack1, NULL));
    assert(0 == res_stack_push(stack1, (void*)33));
    assert(3 == res_stack_get_entries(stack1));

  /*error conditions and edge conditions*/
    printf("\ttesting non-existent entries... ");
    assert(3 == res_stack_xpush(stack1, 4, (void*)1));
    assert(3 == res_stack_xpush(stack1, 10, (void*)1));
    assert(3 == res_stack_xpush(stack1, 100, (void*)1));
    errno = 0;
    assert(NULL == res_stack_xpop(stack1, 3));
    assert(RES_ERR_NOT_FOUND == errno);
    errno = 0;
    assert(NULL == res_stack_xpop(stack1, 10));
    assert(RES_ERR_NOT_FOUND == errno);
    errno = 0;
    assert(NULL == res_stack_xpop(stack1, 100));
    assert(RES_ERR_NOT_FOUND == errno);
    printf("Good!\n");

    printf("\ttesting NULL entries... ");
    errno = 1;
    assert(NULL == res_stack_xpop(stack1, 1));
    assert(0 == errno);
    assert((void*)0x1badbabe == res_stack_get(stack1, 0));  /*check stack looks right*/
    assert((void*)33 == res_stack_get(stack1, 1));
    assert(0 == res_stack_xpush(stack1, 1, NULL));
    assert((void*)0x1badbabe == res_stack_get(stack1, 0));  /*check stack looks right*/
    errno = 1;
    assert(NULL == res_stack_get(stack1, 1));
    assert(0 == errno);
    assert((void*)33 == res_stack_get(stack1, 2));
    while(res_stack_get_entries(stack1) > 0)  /*flush stack*/
      res_stack_pop(stack1);
    printf("Good!\n");

   /*stack ordering after bigger changes and changes at edges*/
    printf("\ttesting edge cases & ordering... ");
    assert(0 == res_stack_get_entries(stack1));
    res_stack_push(stack1, (void*)7);  /*start with checking works at lower end of stack*/
    assert(0 == res_stack_xpush(stack1, 0, (void*) 8));
    assert(2 == res_stack_get_entries(stack1));
    assert((void*)8 == res_stack_get(stack1, 0));
    assert((void*)7 == res_stack_get(stack1, 1));
    assert((void*)8 == res_stack_xpop(stack1, 0));
    assert((void*)7 == res_stack_get(stack1, 0));
    assert((void*)7 == res_stack_xpop(stack1, 0));
    assert(0 == res_stack_get_entries(stack1));
    errno = 0;
    assert(NULL == res_stack_xpop(stack1, 0));
    assert(RES_ERR_NOT_FOUND == errno);

    for (i=0; i<=24; i++)
      res_stack_push(stack1, (void*)i);
    assert(25 == res_stack_get_entries(stack1));
    assert(2 == res_stack_xpush(stack1, 9, (void*) 9));
    assert((void*)9 == res_stack_xpop(stack1, 9));
    assert(24 == res_stack_get_entries(stack1));
    errno = 1;
    for (i=0; i<=8; i++)
    {
      assert((void*)i == res_stack_get(stack1, i));
      assert(0 == errno);
    }
    for (i=10; i<=23; i++)
      assert((void*)i == res_stack_get(stack1, i-1));

    assert(0 == res_stack_xpush(stack1, 24, (void*)0xacab));
    assert(25 == res_stack_get_entries(stack1));
    errno=1;
    for (i=0; i<=8; i++)
    {
      assert((void*)i == res_stack_get(stack1, i));
      assert(0 == errno);
    }
    for (i=10; i<=23; i++)
      assert((void*)i == res_stack_get(stack1, i-1));
    assert((void*)0xacab == res_stack_get(stack1, 24));
    assert((void*)0xacab == res_stack_xpop(stack1, 24));
    assert(24 == res_stack_get_entries(stack1));
    assert(0 == res_stack_xpush(stack1, 9, (void*)9));
    assert(25 == res_stack_get_entries(stack1));
    errno=1;
    for(i=0; i<=24; i++)
    {
      assert((void*)i == res_stack_get(stack1, i));
      assert(0 == errno);
    }
    printf("Good!\n");

  assert(0 == res_stack_destroy(stack1));
  return(0);
}

int resize(void)
{
  res_stack_t *stack1;
  size_t i;
   /*make test stack*/
    printf("\tcreating stack for test... ");
    stack1 = res_stack_create(127);
    assert(NULL != stack1);
    assert(127 == res_stack_get_size(stack1));
    assert(0 == res_stack_get_entries(stack1));
    printf("Good!\n");

   /*error conditions*/
    printf("\terror conditions... ");
    assert(0 == res_stack_push(stack1, (void*)1));
    assert(0 == res_stack_push(stack1, (void*)2));
    assert(2 == res_stack_get_entries(stack1));
    assert(2 == res_stack_resize(stack1, 0));
    assert(NULL != res_stack_pop(stack1));
    assert(NULL != res_stack_pop(stack1));
    /*no way to certainly cause memory error, can't test that*/
    printf("Good!\n");

   /*fill up with test data except top entry*/
    printf("\tadding test data to stack... ");
    assert(0 == res_stack_get_entries(stack1));
    for (i=0; i<127; i++)
      assert(0 == res_stack_push(stack1, (void*) i));
    printf("\tverifying... ");
    for (i=0; i<127; i++)
      assert((void*) i == res_stack_get(stack1, i));
    printf("Good!\n");

   /*check info preserved, esp at edges*/
    printf("\tresizing down by one... ");
    assert(0 == res_stack_resize(stack1, 126));
    assert(126 == res_stack_get_size(stack1));
    printf("verifying data... ");
    errno = 1;
    for (i=0; i<127; i++)
    {
      assert((void*) i == res_stack_get(stack1, i));
      assert(0 == errno);
    }
    printf("Good!\n");

    printf("\tresizing up by two... ");
    assert(0 == res_stack_resize(stack1, 128));
    assert(128 == res_stack_get_size(stack1));
    printf("verifying data... ");
    for (i=0; i<127; i++)
      assert((void*) i == res_stack_get(stack1, i));
    printf("adding new test data... ");
    assert(0 == res_stack_push(stack1, (void*) 127));
    assert(0 == res_stack_push(stack1, (void*) 128));
    for (i=0; i<=128; i++)
      assert((void*) i == res_stack_get(stack1, i));
    printf("Good!\n");

    printf("\tresizing up to 5000... ");
    assert(0 == res_stack_resize(stack1, 4999));
    assert(4999 == res_stack_get_size(stack1));
    printf("verifying data... ");
    for (i=0; i<=128; i++)
      assert((void*) i == res_stack_get(stack1, i));
    printf("adding more test data... ");
    for (i=129; i<4000; i++)
      assert(0 == res_stack_push(stack1, (void*) i));
    printf("verifying... ");
    for (i=3999; i>0; i--)
      assert((void*) i == res_stack_pop(stack1));
    errno = 1;
    assert((void*) 0 == res_stack_pop(stack1));
    assert(0 == errno);
    printf("Good!\n");

  assert(0 == res_stack_destroy(stack1));
  return(0);
}

