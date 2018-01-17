/* stack.h - header for stack.c
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
#ifndef H_RES_STACK
#define H_RES_STACK
 #include "res_config.h"
 #include "res_types.h"
 #include "res_err.h"

/*Structures:*/
 typedef struct
 {
   void **stack;  /*pointer to an array of pointers*/
   size_t limit;  /*number of pointers possible in stack. Stack with 1 entry, limit = 0*/
   size_t top; /*that is, this entry is where new data will be pushed to. pop from top - 1*/
  } res_stack_t;

/*External Functions:*/
 res_stack_t* res_stack_create(size_t max_entries);  /*creates a stack of size big enough to fit max_entries, starts at 0 (i.e. res_add_stack(0) means stack with 1 entry). Returns NULL on failure to allocate memory, errno preserved from calloc call*/
 ushort res_stack_destroy(res_stack_t* stack_handle);  /*returns 0 on success, non-zero on fail*/

 ushort res_stack_push(res_stack_t* stack_handle, void* resource);  /*pushes a resource pointer to the top of the stack, returns 0 on success, 2 on stack full*/
 void* res_stack_pop(res_stack_t* stack_handle);  /*returns the resource pointer at the top of the stack and removes that entry, returns NULL on failure with errno set to res_err.h error code. If resource was a null pointer to begin with, errno=0*/
 ushort res_stack_xpush(res_stack_t* stack_handle, size_t n, void* resource);  /*inserts a resource pointer into the stack at point n (0 = first stack element. Cannot insert an entry more than one element after the top one), returns 0 on success, 2 on stack full, 3 on n too high*/
 void* res_stack_xpop(res_stack_t* stack_handle, size_t n);  /*removes and returns the resource pointer at n, returns NULL on failure with errno set to a res_err.h error code. If the resource was a null pointer to begin with, errno=0*/

 ushort res_stack_change(res_stack_t* stack_handle, size_t n, void* resource);  /*changes the resource pointer of stack entry n. Returns: 0 on success, 2 on stack entry non-existent*/
 void* res_stack_get(res_stack_t* stack_handle, size_t n);  /*returns the resource pointed to by stack entry n, returns NULL on failure with errno set to a res_err.h error code. If the resource was a null pointer to begin with, errno=0*/

 ushort res_stack_resize(res_stack_t* stack_handle, size_t max_entries);  /*resizes the stack to new size max_entries. Returns: 0 on success, 2 on stack too big, 3 on memory error; errno preserved on realloc fail*/

 size_t res_stack_get_entries(res_stack_t* stack_handle);  /*returns the number of entries that have been pushed onto stack, 0= no stack entries, returns RES_STACK_ERR on failure*/
 size_t res_stack_get_size(res_stack_t* stack_handle);  /*returns the maximum number of entries that the stack can hold, 0= max one entry, returns RES_STACK_ERR on failure*/

/*Internal Functions:*/
 void _res_stack_handle_set(res_stack_t* stack_handle, void** base, size_t max_entries);
 size_t _res_stack_size(size_t max_entries);
#endif
