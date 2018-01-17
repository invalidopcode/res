/* stack.c - stack handling code
 *
 * API: stack 1.0
 * IMPLEMENTATION: reff-1
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */

#include <stdlib.h>
#include "res_err.h"
#include "stack.h"

res_stack_t* res_stack_create(size_t max_entries)
{
  res_stack_t *handle;
  void **base;
   /*allocate memory*/
    base = calloc( max_entries + 1, sizeof(void*) );  /*calloc zeros memory for us, not that it's strictly needed*/
    if(NULL == base)  /*memory error, errno set by calloc*/
      return(NULL);

    handle = malloc( sizeof(res_stack_t) );
    if(NULL == handle)
    {
      free(base);
      return(NULL);  /*errno set by calloc*/
    }

   /*create descriptor*/
    _res_stack_handle_set(handle, base, max_entries);

  return(handle);
}

ushort res_stack_destroy(res_stack_t* stack_handle)
{
  free( (stack_handle->stack) );
  free(stack_handle);
  return(0);
}

ushort res_stack_push(res_stack_t* stack_handle, void* resource)
{
  void **stack;
   /*Is it full?*/
    if(stack_handle->top > stack_handle->limit)
      return(2);

   /*Get stack*/
    stack = stack_handle->stack;

   /*Push resource in at the top*/
    stack[stack_handle->top] = resource;
    stack_handle -> top++;
  return(0);
}

void* res_stack_pop(res_stack_t* stack_handle)
{
  void **stack;  /*double pointer. int **stack -> **stack[3] = the integer, *stack[3] = the pointer to the integer, stack = pointer to the array of pointers*/
   /*Check if there are any entries at all*/
    if(0 == stack_handle->top)
    {
      errno = RES_ERR_STACK_EMPTY;
      return(NULL);
    }

   /*Get stack*/
    stack = stack_handle->stack;

   /*Remove top entry*/
    stack_handle -> top--;

  if (NULL == stack[stack_handle -> top])  /*NULL is also used to signal error, so set errno to 0 to make it clear this is the answer, not an error*/
    errno = 0;
  return(stack[stack_handle -> top]);
}

ushort res_stack_xpush(res_stack_t* stack_handle, size_t n, void* resource)
{
  void **stack;  /*double pointer. int **stack -> **stack[3] = the integer, *stack[3] = the pointer to the integer, stack = pointer to the array of pointers*/
  size_t i;
   /*is it full?*/
    if (stack_handle->top > stack_handle->limit)
      return(2);

   /*is n in range*/
    if (n > stack_handle->top)
      return(3);

   /*get stack*/
    stack = stack_handle->stack;

   /*start at the most recent entry, shift each one up*/
    for(i = (stack_handle->top); i>n; i--)
      stack[i] = stack[i-1];

   /*insert new entry*/
    stack[n] = resource;
    stack_handle->top++;
  return(0);
}

void* res_stack_xpop(res_stack_t* stack_handle, size_t n)
{
  void **stack;  /*double pointer. int **stack -> **stack[3] = the integer, *stack[3] = the pointer to the integer, stack = pointer to the array of pointers*/
  void* retval;
  size_t i;
   /*check stack has enough entries for xpop*/
    if(stack_handle->top <= n)
    {
      errno = RES_ERR_NOT_FOUND;
      return(NULL);
    }

   /*Get stack*/
    stack = stack_handle->stack;

   /*extract value*/
    retval = stack[n];

   /*shift stack down*/
    for(i=n; i < (stack_handle->top - 1); i++)  /*has to be top-1 since if stack is full top won't exist*/
      stack[i] = stack[i+1];
    stack_handle -> top--;

  if (NULL == retval)  /*NULL is also used to signal error, so set errno to 0 to make it clear this is the answer, not an error*/
    errno = 0;
  return(retval);
}

ushort res_stack_change(res_stack_t* stack_handle, size_t n, void* resource)
{
  void **stack;  /*double pointer. int **stack -> **stack[3] = the integer, *stack[3] = the pointer to the integer, stack = pointer to the array of pointers*/
   /*is n in range*/
    if (n >= stack_handle->top)
      return(2);

   /*get stack*/
    stack = stack_handle->stack;

   /*change stack entry*/
    stack[n] = resource;
  return(0);
}

void* res_stack_get(res_stack_t* stack_handle, size_t n)
{
  void **stack;  /*double pointer. int **stack -> **stack[3] = the integer, *stack[3] = the pointer to the integer, stack = pointer to the array of pointers*/
   /*is n in range*/
    if (n >= stack_handle->top)
    {
      errno = RES_ERR_NOT_FOUND;
      return(NULL);
    }

   /*get stack*/
    stack = stack_handle->stack;

 /*return stack entry*/
  if (NULL == stack[n])  /*NULL is also used to signal error, so set errno to 0 to make it clear this is the answer, not an error*/
    errno = 0;
  return(stack[n]);
}

ushort res_stack_resize(res_stack_t* stack_handle, size_t max_entries)
{
  void** base;
   /*check stack can fit in new size*/
    if ((max_entries + 2) <= stack_handle->top)
      return(2);

   /*re-alloc memory*/
    base = realloc(stack_handle->stack, _res_stack_size(max_entries));
    if (NULL == base)
      return(3);
   
   /*update handle*/
    stack_handle->stack = (void**) base;
    stack_handle->limit = max_entries;
    
  return(0);
}

size_t res_stack_get_entries(res_stack_t* stack_handle)
{
  return(stack_handle->top);
}

size_t res_stack_get_size(res_stack_t* stack_handle)
{
  return(stack_handle->limit);
}

/*-------------- Internals ----------------*/

void _res_stack_handle_set(res_stack_t* stack_handle, void** base, size_t max_entries)
{
   /*fill out descriptor*/
    stack_handle->stack = base;
    stack_handle->limit = max_entries;
    stack_handle->top = 0;
}

size_t _res_stack_size(size_t max_entries)
{
  return((max_entries + 1) * sizeof(void*));
}

