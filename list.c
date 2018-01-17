/* list.c - linked-list handling code
 *
 * API: list 2.0
 * IMPLEMENTATION: reff-2
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */

#include "list.h"
#include <stdlib.h>

res_list_t* res_list_create()
{
  res_list_t* list = malloc( sizeof(res_list_t) );
    if(NULL == list)
      return(NULL);  /*malloc will have set errno*/
    _res_list_handle_set(list);
  return(list);
}

void _res_list_handle_set(res_list_t* list)
{
  list->first_node = NULL;
  list->current_node = NULL;
  list->num_entries = 0;
}

ushort res_list_destroy(res_list_t* list)
{
    if(0 != list->num_entries)
      return(2);
    free(list);
  return(0);
}

ushort res_list_add(res_list_t* list,
                    void* resource,
                    RES_ID id,
                    RES_TYPE type,
                    RES_SORT_KEY sort_key)
{
  res_list_node_t* new_node;
   /*create new node*/
    new_node = malloc( sizeof(res_list_node_t) );
    if(NULL == new_node)
      return(2);
    _res_list_node_set(new_node, resource, id, type, sort_key);

   /*insert into list*/
    _res_list_node_link(list, new_node);
  return(0);
}

ushort res_list_remove(res_list_t* list)
{
  res_list_node_t* current_node;
   /*get current node*/
    current_node = list->current_node;
    if(NULL == current_node)
     return(2);

   /*unlink from list*/
    _res_list_node_unlink(list, current_node);
  
   /*free node from memory*/
    free(current_node);
  return(0);
}

void _res_list_node_set(res_list_node_t* node,
                        void* resource,
                        RES_ID id,
                        RES_TYPE type,
                        RES_SORT_KEY sort_key)
{
  node->resource = resource;
  node->id = id;
  node->type = type;
  node->sort_key = sort_key;
  node->next = NULL;
  node->prev = NULL;
}

void _res_list_node_link(res_list_t* list, res_list_node_t* node)
{
  res_list_node_t* tmp;

  list->num_entries++;
 
  if((NULL == list->first_node) ||
     (list->first_node->sort_key <= node->sort_key))  /*if inserting at start*/
  {
    if(NULL == list->first_node)  /*only item in list*/
    {
      node->next = NULL;
    } else {  /*not only item - need to update prev*/
      node->next = list->first_node;
      node->next->prev = node;
    }
    list->first_node = node;
    node->prev = NULL;
  } else {
    tmp = list->first_node;
    while((NULL != tmp->next) && (tmp->next->sort_key > node->sort_key))
      tmp = tmp->next;
    if(NULL != tmp->next)  /*insert in middle of list*/
    {
      node->next = tmp->next;
      tmp->next->prev = node;
    } else {  /*insert at end of list*/
      node->next = NULL;
    }
    node->prev = tmp;
    tmp->next = node;
  }
}

void _res_list_node_unlink(res_list_t* list, res_list_node_t* node)
{
  list->num_entries--;

  if(NULL != node->prev)
    node->prev->next = node->next;
  else  /*if this was the first node...*/
    list->first_node = node->next;
  if(NULL != node->next)
    node->next->prev = node->prev;
  if(node == list->current_node)  /*not STRICTLY needed, but will stop bugs*/
    list->current_node = NULL;
}

ushort res_list_seek(res_list_t* list, size_t index)
{
  size_t i;
    if(index >= list->num_entries)
      return(2);
    list->current_node = list->first_node;
    for(i=0; i<index; i++)
      list->current_node = list->current_node->next;
  return(0);  
}

ushort res_list_next(res_list_t* list)
{
  if(NULL == list->current_node)
    return(3);
  if(NULL == list->current_node->next)
    return(2);
  else
    list->current_node = list->current_node->next;
  return(0);
}

ushort res_list_prev(res_list_t* list)
{
  if(NULL == list->current_node)
    return(3);
  if(NULL == list->current_node->prev)
    return(2);
  else
    list->current_node = list->current_node->prev;
  return(0);
}

void* res_list_get_resource(res_list_t* list)
{
 /*check parameters*/
  if(NULL == list->current_node)
  {
    errno = RES_ERR_NO_CURRENT_NODE;
    return(NULL);
  }

 /*report resource pointer*/
  if(NULL == list->current_node->resource)  /*NULL is our error code, so we*/
    errno = 0;                              /*must signal there's no problem*/
  return(list->current_node->resource);
}

RES_ID res_list_get_id(res_list_t* list)
{
 /*check parameters*/
  if(NULL == list->current_node)
  {
    errno = RES_ERR_NO_CURRENT_NODE;
    return(RES_ID_ERR);
  }

 /*report resource id*/
  return(list->current_node->id);
}

RES_TYPE res_list_get_type(res_list_t* list)
{
 /*check parameters*/
  if(NULL == list->current_node)
  {
    errno = RES_ERR_NO_CURRENT_NODE;
    return(RES_TYPE_ERR);
  }

 /*report resource type*/
  return(list->current_node->type);
}

RES_SORT_KEY res_list_get_sort_key(res_list_t* list)
{
 /*check parameters*/
  if(NULL == list->current_node)
  {
    errno = RES_ERR_NO_CURRENT_NODE;
    return(RES_SORT_KEY_ERR);
  }

 /*report resource sort key*/
  return(list->current_node->sort_key);
}


ushort res_list_ch_resource(res_list_t* list, void* resource)
{
   /*check parameters*/
    if(NULL == list->current_node)
      return(2);

   /*do the change*/
    list->current_node->resource = resource;
  return(0);
}

ushort res_list_ch_id(res_list_t* list, RES_ID id)
{
   /*check parameters*/
    if(NULL == list->current_node)
      return(2);

   /*do the change*/
    list->current_node->id = id;
  return(0);
}

ushort res_list_ch_type(res_list_t* list, RES_TYPE type)
{
   /*check parameters*/
    if(NULL == list->current_node)
      return(2);

   /*do the change*/
    list->current_node->type = type;
  return(0);
}

ushort res_list_ch_sort_key(res_list_t* list, RES_SORT_KEY sort_key)
{
  res_list_node_t* current_node;
   /*check parameters*/
    if(NULL == list->current_node)
      return(2);

   /*do the change*/
    current_node = list->current_node;  /*removing from list, so must store*/
    _res_list_node_unlink(list, current_node);
    current_node->sort_key = sort_key;
    _res_list_node_link(list, current_node);
  return(0);
}

ushort res_list_search_by_resource(res_list_t* list,
                                     intmax_t iterations,
                                     void* resource)
{
  intmax_t i;
   /*check parameters*/
    if(NULL == list->current_node)
    {
      if(0 == list->num_entries)
        return(2);
      else
      {
        if(iterations >= 0)  /*are we going forwards?*/
          list->current_node = list->first_node;  /*start at the start*/
        else
          res_list_seek(list, (list->num_entries) - 1); /*start at the end*/
      }
    }

    if(0 == iterations)
      iterations = (intmax_t)list->num_entries;

   /*loop*/
    if(iterations > 0)
    {
      for(i=0; i<iterations; i++)
      {
        if(resource == list->current_node->resource) return(0);
        if(NULL == list->current_node->next) return(4);
        list->current_node = list->current_node->next;
      }
    } else {
      for(i=0; i>iterations; i--)
      {
        if(resource == list->current_node->resource) return(0);
        if(NULL == list->current_node->prev) return(4);
        list->current_node = list->current_node->prev;
      }
    }
  return(3);
}

ushort res_list_search_by_id(res_list_t* list, intmax_t iterations, RES_ID id)
{
  intmax_t i;
   /*check parameters*/
    if(NULL == list->current_node)
    {
      if(0 == list->num_entries)
        return(2);
      else
        if(iterations >= 0)  /*are we going forwards?*/
          list->current_node = list->first_node;  /*start at the start*/
        else
          res_list_seek(list, (list->num_entries) - 1); /*start at the end*/
    }

    if(0 == iterations)
      iterations = (intmax_t)list->num_entries;

   /*loop*/
    if(iterations > 0)
    {
      for(i=0; i<iterations; i++)
      {
        if(id == list->current_node->id) return(0);
        if(NULL == list->current_node->next) return(4);
        list->current_node = list->current_node->next;
      }
    } else {
      for(i=0; i>iterations; i--)
      {
        if(id == list->current_node->id) return(0);
        if(NULL == list->current_node->prev) return(4);
        list->current_node = list->current_node->prev;
      }
    }
  return(3);
}

ushort res_list_search_by_type(res_list_t* list, intmax_t iterations, RES_TYPE type)
{
  intmax_t i;
   /*check parameters*/
    if(NULL == list->current_node)
    {
      if(0 == list->num_entries)
        return(2);
      else
        if(iterations >= 0)  /*are we going forwards?*/
          list->current_node = list->first_node;  /*start at the start*/
        else
          res_list_seek(list, (list->num_entries) - 1); /*start at the end*/
    }

    if(0 == iterations)
      iterations = (intmax_t)list->num_entries;

   /*loop*/
    if(iterations > 0)
    {
      for(i=0; i<iterations; i++)
      {
        if(type == list->current_node->type) return(0);
        if(NULL == list->current_node->next) return(4);
        list->current_node = list->current_node->next;
      }
    } else {
      for(i=0; i>iterations; i--)
      {
        if(type == list->current_node->type) return(0);
        if(NULL == list->current_node->prev) return(4);
        list->current_node = list->current_node->prev;
      }
    }
  return(3);
}

ushort res_list_search_by_sort_key(res_list_t* list, intmax_t iterations, RES_SORT_KEY sort_key)
{
  intmax_t i;
   /*check parameters*/
    if(NULL == list->current_node)
    {
      if(0 == list->num_entries)
        return(2);
      else
      {
        if(iterations >= 0)  /*are we going forwards?*/
          list->current_node = list->first_node;  /*start at the start*/
        else
          res_list_seek(list, (list->num_entries) - 1); /*start at the end*/
      }
    }

    if(0 == iterations)
      iterations = (intmax_t)list->num_entries;

   /*loop*/
    if(iterations > 0)
    {
      for(i=0; i<iterations; i++)
      {
        if(sort_key == list->current_node->sort_key) return(0);
        if(NULL == list->current_node->next) return(4);
        list->current_node = list->current_node->next;
      }
    } else {
      for(i=0; i>iterations; i--)
      {
        if(sort_key == list->current_node->sort_key) return(0);
        if(NULL == list->current_node->prev) return(4);
        list->current_node = list->current_node->prev;
      }
    }
  return(3);
}

size_t res_list_get_num_entries(res_list_t* list)
{
 /*return number of list entries*/
  return(list->num_entries);
}
