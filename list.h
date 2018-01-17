/* list.h - header for list.c
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

#ifndef H_RES_LIST
#define H_RES_LIST
 #include "res_config.h"
 #include "res_types.h"
 #include "res_err.h"

/*Structures*/
 typedef struct s_res_list_node res_list_node_t;

 struct s_res_list_node
 {
   void *resource;
   RES_ID id;
   RES_TYPE type;
   RES_SORT_KEY sort_key;  /*lower = further from the beginning*/
   res_list_node_t *next;
   res_list_node_t *prev;
 };

 typedef struct
 {
   res_list_node_t *first_node;
   res_list_node_t *current_node;
   size_t num_entries;
 } res_list_t;

/*External functions*/
 res_list_t* res_list_create(void);  /*allocates memory for a list handle and sets initial values. Returns: pointer to new list on success, NULL on failure; errno preserved from malloc fail*/
 ushort res_list_destroy(res_list_t* list); /*frees list handle. List MUST be empty to be destroyed. Returns: 0 on success, 2 on list not empty*/ 

 ushort res_list_add(res_list_t* list, void* resource, RES_ID id, RES_TYPE type, RES_SORT_KEY sort_key);  /*inserts a new entry into a list. The greater the sort_key (signed), the closer the entry will be inserted to the beginning of the list. Sort keys that are equal have undefined order (usually newest first in THIS implementation). Returns 0 on success, 2 on memory error; errno preserved from malloc on memory error*/
 ushort res_list_remove(res_list_t* list);  /*removes current node from a list & frees from memory. Returns: 0 on success, 2 on no current node*/

 ushort res_list_seek(res_list_t* list, size_t index);  /*set current node to list entry given by index. Returns 0 on success, 2 on index out of range*/
 ushort res_list_next(res_list_t* list);  /*jumps to the next entry on the list from the current one. Returns 0 on success, 2 on end of list, 3 on no current node set*/
 ushort res_list_prev(res_list_t* list);  /*jumps to the previous entry on the list from the current one. Returns 0 on success, 2 when already at start of list, 3 on no current node set*/

 void* res_list_get_resource(res_list_t* list);  
/*gets the resource pointer of the current node of a list. Current node is set using seek, next, prev. Returns: pointer on success, NULL on failure; errno is set to a res_err.h error code, in the event that the resource pointer really is NULL, errno is set to 0*/
 RES_ID res_list_get_id(res_list_t* list);
/*gets the id of the current node of a list. Current node is set using seek, next, prev. Returns: pointer on success, RES_ID_ERR on failure; errno is set to a res_err.h error code*/
 RES_TYPE res_list_get_type(res_list_t* list);
/*gets the type value of the current node of a list. Current node is set using seek, next, prev. Returns: pointer on success, RES_TYPE_ERR on failure; errno is set to a res_err.h error code*/
 RES_SORT_KEY res_list_get_sort_key(res_list_t* list);
/*gets the sort key value of the current node of a list. Current node is set using seek, next, prev. Returns: pointer on success, RES_SORT_KEY_ERR on failure; errno is set to a res_err.h error code*/

 ushort res_list_ch_resource(res_list_t* list, void* resource);  /*changes the resource pointer of the current node of a list. Current node is set using seek, next, prev. Returns: 2 on no current node*/
 ushort res_list_ch_id(res_list_t* list, RES_ID id);  /*changes the id of the current node of a list. Current node is set using seek, next, prev. Returns: 2 on no current node*/
 ushort res_list_ch_type(res_list_t* list, RES_TYPE type);  /*changes the type value of the current node of a list. Current node is set using seek, next, prev. Returns: 2 on no current node*/
 ushort res_list_ch_sort_key(res_list_t* list, RES_SORT_KEY sort_key);  /*changes the sort key of the current node of a list, and changes the location of the list entry based on the new sort key. Current node is set using seek, next, prev. Returns: 2 on no current node*/

 ushort res_list_search_by_resource(res_list_t* list, intmax_t iterations, void* resource);  /*searches list for first occurrence of resource, sets current node to that. Starts on and includes current node, repeats until end of list or when number of entries tested == iterations. Iterations of 0 means search to end of list. Iterations of less than 0 means search in reverse. If current node is not set, starts search at the beginning/end of list. Returns: 0 on success, 2 on no list entries, 3 on no matching list entry found, 4 on end of list reached (ie more iterations left, but no more entries to test). Current node set to last node checked on errors 3 or 4*/
 ushort res_list_search_by_id(res_list_t* list, intmax_t iterations, RES_ID id);  /*searches list for first occurrence of id, sets current node to that. Starts on and includes current node, repeats until end of list or when number of entries tested == iterations. Iterations of 0 means search to end of list. Iterations of less than 0 means search in reverse. If current node is not set, starts search at the beginning/end of list. Returns: 0 on success, 2 on no list entries, 3 on no matching list entry found, 4 on end of list reached (ie more iterations left, but no more entries to test). Current node set to last node checked on errors 3 or 4*/
 ushort res_list_search_by_type(res_list_t* list, intmax_t iterations, RES_TYPE type);  /*searches list for first occurrence of type, sets current node to that. Starts on and includes current node, repeats until end of list or when number of entries tested == iterations. Iterations of 0 means search to end of list. Iterations of less than 0 means search in reverse. If current node is not set, starts search at the beginning/end of list. Returns: 0 on success, 2 on no list entries, 3 on no matching list entry found, 4 on end of list reached (ie more iterations left, but no more entries to test). Current node set to last node checked on errors 3 or 4*/
 ushort res_list_search_by_sort_key(res_list_t* list, intmax_t iterations, RES_SORT_KEY sort_key);  /*searches list for first occurrence of sort_key, sets current node to that. Starts on and includes current node, repeats until end of list or when number of entries tested == iterations. Iterations of 0 means search to end of list. Iterations of less than 0 means search in reverse. If current node is not set, starts search at the beginning/end of list. Returns: 0 on success, 2 on no list entries, 3 on no matching list entry found, 4 on end of list reached (ie more iterations left, but no more entries to test). Current node set to last node checked on errors 3 or 4*/

 size_t res_list_get_num_entries(res_list_t* list);  /*Returns: number of list entries on success, RES_LIST_ERR on failure; errno is set to a res_err.h error code*/

/*Internal functions*/
 void _res_list_node_set(res_list_node_t* node, void* resource, RES_ID id, RES_TYPE type, RES_SORT_KEY sort_key);
 void _res_list_node_link(res_list_t* list, res_list_node_t* node);  /*inserts a pre-filled node structure into the list, location determined by sort_key. NOTE: does not check if node is already linked*/
 void _res_list_node_unlink(res_list_t* list, res_list_node_t* node);  /*NOTE: does not reset node's next & prev*/
 void _res_list_handle_set(res_list_t* list);
#endif
