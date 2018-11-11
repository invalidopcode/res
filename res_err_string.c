/* res_err_string.c - provides res_err_string(errno), function to give
 *                   explanation for an error code
 *
 * This file is released into the public domain, and permission is granted
 * to use, modify, and / or redistribute at will. This software is provided
 * 'as-is', without any express or implied  warranty. In no event will the
 * authors be held liable for any damages arising from the use of this
 * software.
 */
#include <string.h>
#include "res_err.h"

  const char str_err_handle[] = "Error in handle";
  const char str_err_bad_parameter[] = "Bad parameter";
  const char str_err_incompatible_resource[] = "Incompatible resource type";
  const char str_err_no_match[] = "Search found no matches";
  const char str_err_no_current_node[] = "No current node for list - use res_list_seek to select";
  const char str_err_stack_empty[] = "Stack empty";
  const char str_err_not_found[] = "The resource requested was not found";
  const char str_err_unknown[] = "Unknown error";
/*^^ global because these need to be read by calling function*/

const char* res_err_string(int err)
{
  switch(err)  /*no break statements needed, return breaks out of switch*/
  {
    case RES_ERR_BAD_PARAMETER :
      return(str_err_bad_parameter);
    case RES_ERR_INCOMPATIBLE_RESOURCE :
      return(str_err_incompatible_resource);
    case RES_ERR_NO_MATCH :
      return(str_err_no_match);
    case RES_ERR_NO_CURRENT_NODE :
      return(str_err_no_current_node);
    case RES_ERR_STACK_EMPTY :
      return(str_err_stack_empty);
    case RES_ERR_NOT_FOUND :
      return(str_err_not_found);
    case RES_ERR_UNKNOWN :
      return(str_err_unknown);
    default :
      return(strerror(errno));
  }
}
