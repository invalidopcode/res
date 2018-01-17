/* list_test.c - unit tests for lists code 
 *
 * REQUIRES: list_2
 * TESTS: list_2.0
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include "list.h"

  int main(void);
  void create_destroy(void);
  void add_remove_seek(void);
  void get_change(void);
  void search(void);

int main()
{
  srand((unsigned int)time(NULL));
  create_destroy();
  add_remove_seek();
  get_change();
  search();
  printf("ALL TESTS COMPLETED - win!\n");
  return(EXIT_SUCCESS);
}

void create_destroy()
{
  res_list_t *list1;
    printf("testing list creation and destruction:\n");
   /*create and destroy list*/
    printf("\tcreating and destroying a list... ");
    list1 = res_list_create();
    assert(NULL != list1);
    assert( 0 == res_list_destroy(list1) );
    printf("Good!\n");

   /*destroy list with entries, with NULL handle (MUST fail)*/
    printf("\ttesting list_destroy error reporting... ");
    list1 = res_list_create();
    assert(NULL != list1);
    list1->num_entries = 1;
    assert( 2 == res_list_destroy(list1) );
    list1->num_entries = 0;
    assert( 0 == res_list_destroy(list1) );
    printf("Good!\n");
}

void add_remove_seek()
{
  res_list_t *list1;
  size_t i;
  size_t entries;
    printf("testing add/remove/seek/next/prev:\n");
   /*create test list*/
    list1 = res_list_create();
    assert(NULL != list1);

   /*add 1000 entries*/
    printf("\tadding 1000 entries... ");
    for(i=0; i<1000; i++)
      assert( 0 == res_list_add(list1, (void*)(uintptr_t)((unsigned)rand() % 0x10000), (unsigned short)rand()%1000, (unsigned short)rand()%1000, (short)(500 - rand()%1000)) );
    assert( 1000 == res_list_get_num_entries(list1) );
    printf("Good!\n");

   /*remove 20 entries (manually set current entry for now)*/
    printf("\tremoving 20 entries... ");
    for(i=0; i<20; i++)
    {
      assert( 0 == res_list_seek(list1, 10 + i) );
      assert( 0 == res_list_remove(list1) );
    }
    assert( 980 == res_list_get_num_entries(list1) );
    printf("Good!\n");

   /*add another 1000 entries*/
    printf("\tadding (another) 1000 entries... ");
    for(i=0; i<1000; i++)
      assert( 0 == res_list_add(list1, (void*)(uintptr_t)((unsigned)rand() % 0x10000), (unsigned short)rand()%1000, (unsigned short)rand()%1000, (short)(500 - rand()%1000)) );
    assert( 1980 == res_list_get_num_entries(list1) );
    printf("Good!\n");

   /*test seek behaviour*/
    printf("\ttesting seek, next, prev... ");
    list1->current_node = NULL;  /*make sure no current node, so we can test errors*/
    assert( 3 == res_list_prev(list1) );
    assert( 3 == res_list_next(list1) );
    assert( 2 == res_list_seek(list1, 2000000) );
    assert( 0 == res_list_seek(list1, 0) );
    assert( 2 == res_list_prev(list1) );
    assert( 0 == res_list_next(list1) );
    assert( 0 == res_list_prev(list1) );
    assert( list1->current_node == list1->first_node);  /*should be at the beginning again*/
    entries = res_list_get_num_entries(list1);
    assert(RES_LIST_ERR != entries);
    assert( 2 == res_list_seek(list1, entries) );
    assert( 0 == res_list_seek(list1, entries-1) );  /*last entry*/
    assert( 2 == res_list_next(list1) );
    assert( 0 == res_list_prev(list1) );
    assert( 0 == res_list_next(list1) );
    printf("Good!\n");

   /*verify order of list*/
    printf("\tverifying the order of the list... ");
    entries = res_list_get_num_entries(list1);
    assert(RES_LIST_ERR != entries);
    assert( list1->first_node->next->sort_key <= list1->first_node->sort_key );
    assert( 0 == res_list_seek(list1, entries-1) );
    assert( list1->current_node->prev->sort_key >= list1->current_node->sort_key);
    for(i=1; i<entries-1; i++)
    {
      assert( 0 == res_list_seek(list1, i) );
      assert( list1->current_node->sort_key <= list1->current_node->prev->sort_key );
      assert( list1->current_node->sort_key >= list1->current_node->next->sort_key );
    }
    printf("Good!\n");

   /*verify structure of list*/
    printf("\tverifying the structure of the list... ");
    entries = res_list_get_num_entries(list1);
    assert(RES_LIST_ERR != entries);
    assert( NULL == list1->first_node->prev );
    assert( list1->first_node->next->prev == list1->first_node);
    assert( 0 == res_list_seek(list1, entries-1) );
    assert( NULL == list1->current_node->next );
    assert( list1->current_node->prev->next == list1->current_node );
    for(i=1; i<entries-1; i++)
    {
      assert( 0 == res_list_seek(list1, i) );
      assert( list1->current_node->prev->next == list1->current_node );
      assert( list1->current_node->next->prev == list1->current_node );
    }
    printf("Good!\n");

   /*remove entries*/
    printf("\tremoving all remaining entries... ");
    entries = res_list_get_num_entries(list1);
    assert(RES_LIST_ERR != entries);
    for(i=0; i<entries; i++)
    {
      assert( 0 == res_list_seek(list1, 0) );
      assert( 0 == res_list_remove(list1) );
    }
    printf("Good!\n");

   /*destroy test list*/
    assert( 0 == res_list_destroy(list1) );
}

void get_change()
{
  res_list_t* list1;
  size_t i;
  size_t entries;
  RES_SORT_KEY tmp1;
  RES_SORT_KEY tmp2;
    printf("testing get and ch:\n");
   /*create list of 1000 entries*/
    printf("\tcreating list... ");
    list1 = res_list_create();
    assert(NULL != list1);
    for(i=0; i<1000; i++)
      assert( 0 == res_list_add(list1, (void*) i, (unsigned short) i, (unsigned short)i, (short)-i) );
    printf("Good!\n");

   /*test error reporting from get and ch*/
    printf("\ttesting error reporting... ");
    errno = 0;
    assert( NULL == res_list_get_resource(list1) );
    assert(RES_ERR_NO_CURRENT_NODE == errno);
    errno = 0;
    assert( RES_TYPE_ERR == res_list_get_type(list1) );
    assert(RES_ERR_NO_CURRENT_NODE == errno);
    errno = 0;
    assert( RES_ID_ERR == res_list_get_id(list1) );
    assert(RES_ERR_NO_CURRENT_NODE == errno);
    errno = 0;
    assert( RES_SORT_KEY_ERR == res_list_get_sort_key(list1) );
    assert(RES_ERR_NO_CURRENT_NODE == errno);
    assert( 2 == res_list_ch_resource(list1, NULL) );
    assert( 2 == res_list_ch_type(list1, 0) );
    assert( 2 == res_list_ch_id(list1, 0) );
    assert( 2 == res_list_ch_sort_key(list1, 0) );
    printf("Good!\n");

   /*change and get properties of a few entries (but NO change of sort key)*/
    printf("\tget'ing and ch'ing resource, id, type... ");
    assert( 0 == res_list_seek(list1, 0) );
    assert( (void*)0 == res_list_get_resource(list1) );
    assert( 0 == res_list_get_id(list1) );
    assert( 0 == res_list_get_type(list1) );
    assert( 0 == res_list_ch_resource(list1, (void*) 0x31337) );
    assert( 0 == res_list_ch_id(list1, 1337) );
    assert( 0 == res_list_ch_type(list1, 1337) );
    assert( (void*) 0x31337 == res_list_get_resource(list1) );
    assert( 1337 == res_list_get_id(list1) );
    assert( 1337 == res_list_get_type(list1) );
    assert( 0 == res_list_seek(list1, 999) );
    assert( (void*)999 == res_list_get_resource(list1) );
    assert( 999 == res_list_get_id(list1) );
    assert( 999 == res_list_get_type(list1) );
    assert( 0 == res_list_ch_resource(list1, (void*) 0x31337) );
    assert( 0 == res_list_ch_id(list1, 1337) );
    assert( 0 == res_list_ch_type(list1, 1337) );
    assert( (void*) 0x31337 == res_list_get_resource(list1) );
    assert( 1337 == res_list_get_id(list1) );
    assert( 1337 == res_list_get_type(list1) );
    printf("Good!\n");

   /*change a few sort keys, then verify the order of the list*/
    printf("\ttesting get and ch of sort key... ");
    assert( 0 == res_list_seek(list1, 0) );
    assert( 0 == res_list_ch_sort_key(list1, -12) );
    assert( 0 == res_list_seek(list1, 999) );
    assert( 0 == res_list_ch_sort_key(list1, -89) );

    for(i=0; i<2000; i++)
    {
      assert( 0 == res_list_seek(list1, (unsigned)rand()%999) );
      assert( 0 == res_list_ch_sort_key(list1, (short)(1000-(rand()%2000))) );
    }

    entries = res_list_get_num_entries(list1);
    assert(RES_LIST_ERR != entries);
    assert( 0 == res_list_seek(list1, 0) );
    tmp1 = res_list_get_sort_key(list1);
    assert(RES_SORT_KEY_ERR != tmp1);
    assert( 0 == res_list_next(list1) );
    tmp2 = res_list_get_sort_key(list1);
    assert( RES_SORT_KEY_ERR != tmp2 );
    assert(tmp1 >= tmp2);

    assert( 0 == res_list_seek(list1, entries-1) );
    tmp1 = res_list_get_sort_key(list1);
    assert(RES_SORT_KEY_ERR != tmp1);
    assert( 0 == res_list_prev(list1) );
    tmp2 = res_list_get_sort_key(list1);
    assert( RES_SORT_KEY_ERR != tmp2 );
    assert(tmp1 <= tmp2);

    for(i=1; i<entries-1; i++)
    {
      assert( 0 == res_list_seek(list1, i) );
      tmp1 = res_list_get_sort_key(list1);
      assert( RES_SORT_KEY_ERR != tmp1);
      assert( 0 == res_list_prev(list1) );
      tmp2 = res_list_get_sort_key(list1);
      assert( RES_SORT_KEY_ERR != tmp2 );
      assert( tmp2 >= tmp1);
      assert( 0 == res_list_next(list1) );
      assert( 0 == res_list_next(list1) );
      tmp2 = res_list_get_sort_key(list1);
      assert( RES_SORT_KEY_ERR != tmp2 );
      assert( tmp2 <= tmp1);
    }

    printf("Good!\n");

   /*destroy list*/
    while(0 != res_list_get_num_entries(list1))
    {
      assert(0 == res_list_seek(list1, 0));
      assert(0 == res_list_remove(list1));
    }
    assert( 0 == res_list_destroy(list1) );
}

void search()
{
  res_list_t* list1;
  size_t i;
    printf("testing search:\n");
   /*create list*/
    list1 = res_list_create();
    assert(NULL != list1);

   /*empty list*/
    printf("\tempty list errors... ");
    assert(2 == res_list_search_by_resource(list1, 1, (void*)0xabc));
    assert(2 == res_list_search_by_id(list1, 1, 1));
    assert(2 == res_list_search_by_type(list1, 1, 1));
    assert(2 == res_list_search_by_sort_key(list1, 1, 1));
    printf("Good!\n");

   /*add test data*/
    for(i=1; i<=1000; i++)
      res_list_add(list1, (void*)(uintptr_t)i, (unsigned short)i, (unsigned short)i, (short)-i);

   /*items at end and beginning and middle of list*/
    printf("\ttesting search for item at start of list... ");
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_resource(list1, 0, (void*)1));
    assert((void*)1 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_id(list1, 0, 1));
    assert(1 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_type(list1, 0, 1));
    assert(1 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_sort_key(list1, 0, -1));
    assert(-1 == res_list_get_sort_key(list1));
   
    printf("backwards... ");
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_resource(list1, -1000, (void*)1));
    assert((void*)1 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_id(list1, -1000, 1));
    assert(1 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_type(list1, -1000, 1));
    assert(1 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_sort_key(list1, -1000, -1));
    assert(-1 == res_list_get_sort_key(list1));
    
    printf("Good!\n");

    printf("\ttesting search for item at end of list... ");
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_resource(list1, 0, (void*)1000));
    assert((void*)1000 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_id(list1, 0, 1000));
    assert(1000 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_type(list1, 0, 1000));
    assert(1000 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_sort_key(list1, 0, -1000));
    assert(-1000 == res_list_get_sort_key(list1));
    printf("backwards... ");
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_resource(list1, -1000, (void*)1000));
    assert((void*)1000 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_id(list1, -1000, 1000));
    assert(1000 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_type(list1, -1000, 1000));
    assert(1000 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_sort_key(list1, -1000, -1000));
    assert(-1000 == res_list_get_sort_key(list1));
    
    printf("Good!\n");

    printf("\ttesting search for item in middle of list... ");
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_resource(list1, 0, (void*)500));
    assert((void*)500 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_id(list1, 0, 500));
    assert(500 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_type(list1, 0, 500));
    assert(500 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 0));
    assert(0 == res_list_search_by_sort_key(list1, 0, -500));
    assert(-500 == res_list_get_sort_key(list1));
    
    printf("backwards... ");
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_resource(list1, -1000, (void*)500));
    assert((void*)500 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_id(list1, -1000, 500));
    assert(500 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_type(list1, -1000, 500));
    assert(500 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 999));
    assert(0 == res_list_search_by_sort_key(list1, -1000, -500));
    assert(-500 == res_list_get_sort_key(list1));
    
    printf("Good!\n");

   /*same, but within a range*/
    printf("\ttesting bounded search...\n");
    
    printf("\t\t1 iteration (fw & bw)... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_resource(list1, 1, (void*)10));
    assert((void*)10 == res_list_get_resource(list1));
    assert(0 == res_list_search_by_resource(list1, -1, (void*)10));
    assert((void*)10 == res_list_get_resource(list1));
    assert(0 == res_list_search_by_id(list1, 1, 10));
    assert(10 == res_list_get_id(list1));
    assert(0 == res_list_search_by_id(list1, -1, 10));
    assert(10 == res_list_get_id(list1));
    assert(0 == res_list_search_by_type(list1, 1, 10));
    assert(10 == res_list_get_type(list1));
    assert(0 == res_list_search_by_type(list1, -1, 10));
    assert(10 == res_list_get_type(list1));
    assert(0 == res_list_search_by_sort_key(list1, 1, -10));
    assert(-10 == res_list_get_sort_key(list1));
    assert(0 == res_list_search_by_sort_key(list1, -1, -10));
    assert(-10 == res_list_get_sort_key(list1));

    printf("Good!\n");

    printf("\t\t100 iterations - item at start... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_resource(list1, 100, (void*)10));
    assert((void*)10 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_id(list1, 100, 10));
    assert(10 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_type(list1, 100, 10));
    assert(10 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_sort_key(list1, 100, -10));
    assert(-10 == res_list_get_sort_key(list1));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_resource(list1, -100, (void*)10));
    assert((void*)10 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_id(list1, -100, 10));
    assert(10 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_type(list1, -100, 10));
    assert(10 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_sort_key(list1, -100, -10));
    assert(-10 == res_list_get_sort_key(list1));

    printf("Good!\n");

    printf("\t\t100 iterations - item at end... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_resource(list1, 100, (void*)109));
    assert((void*)109 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_id(list1, 100, 109));
    assert(109 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_type(list1, 100, 109));
    assert(109 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_sort_key(list1, 100, -109));
    assert(-109 == res_list_get_sort_key(list1));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_resource(list1, -100, (void*)109));
    assert((void*)109 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_id(list1, -100, 109));
    assert(109 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_type(list1, -100, 109));
    assert(109 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_sort_key(list1, -100, -109));
    assert(-109 == res_list_get_sort_key(list1));

    printf("Good!\n");

    printf("\t\t100 iterations - item in middle... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_resource(list1, 100, (void*)50));
    assert((void*)50 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_id(list1, 100, 50));
    assert(50 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_type(list1, 100, 50));
    assert(50 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 9));
    assert(0 == res_list_search_by_sort_key(list1, 100, -50));
    assert(-50 == res_list_get_sort_key(list1));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_resource(list1, -100, (void*)50));
    assert((void*)50 == res_list_get_resource(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_id(list1, -100, 50));
    assert(50 == res_list_get_id(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_type(list1, -100, 50));
    assert(50 == res_list_get_type(list1));
    assert(0 == res_list_seek(list1, 108));
    assert(0 == res_list_search_by_sort_key(list1, -100, -50));
    assert(-50 == res_list_get_sort_key(list1));

    printf("Good!\n");

   /*error on items not within range*/
    printf("\ttesting error conditions... \n");

    printf("\t\tnon-existing item... ");
    assert(0 == res_list_seek(list1, 0));
    assert(0 != res_list_search_by_resource(list1, 0, (void*)0xbeef));
    assert(0 == res_list_seek(list1, 0));
    assert(0 != res_list_search_by_id(list1, 0, 0xbeef));
    assert(0 == res_list_seek(list1, 0));
    assert(0 != res_list_search_by_type(list1, 0, 0xbeef));
    assert(0 == res_list_seek(list1, 0));
    assert(0 != res_list_search_by_sort_key(list1, 0, -0xbee));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 999));
    assert(0 != res_list_search_by_resource(list1, -1000, (void*)0xbeef));
    assert(0 == res_list_seek(list1, 999));
    assert(0 != res_list_search_by_id(list1, -1000, 0xbeef));
    assert(0 == res_list_seek(list1, 999));
    assert(0 != res_list_search_by_type(list1, -1000, 0xbeef));
    assert(0 == res_list_seek(list1, 999));
    assert(0 != res_list_search_by_sort_key(list1, -1000, -0xbee));

    printf("Good!\n");

    printf("\t\tout of range low... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_resource(list1, 100, (void*)5));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_id(list1, 100, 5));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_type(list1, 100, 5));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_sort_key(list1, 100, -5));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_resource(list1, -100, (void*)5));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_id(list1, -100, 5));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_type(list1, -100, 5));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_sort_key(list1, -100, -5));

    printf("Good!\n");

    printf("\t\tout of range high... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_resource(list1, 100, (void*)115));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_id(list1, 100, 115));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_type(list1, 100, 115));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_sort_key(list1, 100, -115));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_resource(list1, -100, (void*)115));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_id(list1, -100, 115));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_type(list1, -100, 115));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_sort_key(list1, -100, -115));

    printf("Good!\n");

    printf("\t\toff by one low... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_resource(list1, 100, (void*)9));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_id(list1, 100, 9));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_type(list1, 100, 9));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_sort_key(list1, 100, -9));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_resource(list1, -100, (void*)9));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_id(list1, -100, 9));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_type(list1, -100, 9));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_sort_key(list1, -100, -9));

    printf("Good!\n");

    printf("\t\toff by one high... ");
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_resource(list1, 100, (void*)110));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_id(list1, 100, 110));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_type(list1, 100, 110));
    assert(0 == res_list_seek(list1, 9));
    assert(0 != res_list_search_by_sort_key(list1, 100, -110));

    printf("bw... ");
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_resource(list1, -100, (void*)110));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_id(list1, -100, 110));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_type(list1, -100, 110));
    assert(0 == res_list_seek(list1, 108));
    assert(0 != res_list_search_by_sort_key(list1, -100, -110));

    printf("Good!\n");

   /*destroy list*/
    while(0 != res_list_get_num_entries(list1))
    {
      assert(0 == res_list_seek(list1, 0));
      assert(0 == res_list_remove(list1));
    }
    assert( 0 == res_list_destroy(list1) );
}
