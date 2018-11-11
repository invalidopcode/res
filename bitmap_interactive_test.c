/* bitmap_interactive_test.c - tool for checking bitmap.c works v0.0.1
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
#define _GNU_SOURCE
#include <stdio.h>
#include "bitmap.h"

 int main(void);
 void print_help(void);
 void print_bitmap(res_bitmap_t* bitmap);
 void print_bitmap_2(res_bitmap_t* bitmap);
 void print_bitmap_info(res_bitmap_t* bitmap);

int main()
{
  ssize_t line_length;
  size_t dummy_len;  /*always set to 0 for getline*/
  char *line;
  res_bitmap_t* test_bitmap;
  size_t n;
  size_t base;
  ushort ret_code;
   /*make bitmap for testing*/
    test_bitmap = res_bitmap_create(304); 

   /*input loop*/
    while(1)
    {
      printf("? ");

      dummy_len = 0;
      line = NULL;
      line_length = getline(&line, &dummy_len, stdin);

      if(line_length < 0)
      {
        free(line);  /*specs say always free line pointer*/
        res_bitmap_destroy(test_bitmap);
        return(EXIT_FAILURE);
      }

      if(0 == line_length)
        print_help();

      if(line_length > 0) /*interpret command*/
      {
        switch(*line)
        {
          case '?' :
            print_help();
            break;
          case 'q' :
            free(line);
            res_bitmap_destroy(test_bitmap);
            return(EXIT_SUCCESS);
          case 'p' :
            print_bitmap(test_bitmap);
            break;
          case 'o' :
            print_bitmap_2(test_bitmap);
            break;
          case 'i' :
            print_bitmap_info(test_bitmap);
            break;
          case 'a' :
            if(sscanf(line, "a %i", (int*)&n) <= 0)
              printf("invalid use of allocate command, type ? for help\n");
            else
            {
              base = res_bitmap_alloc(test_bitmap, n);
              if(RES_BITMAP_ERR == base)
                printf("ERR %u - %s\n", errno, res_err_string(errno));
              else
                printf("allocated starting at base 0x%lx\n", base);
            }            
            break;
          case 't' :
            if(sscanf(line, "t %li %li", (long int*)&base, (long int*)&n) <=0)
              printf("invalid use of take command, type ? for help\n");
            else
            {
              ret_code = res_bitmap_take(test_bitmap, base, n);
              if(0 == ret_code)
                printf("done\n");
              else
                printf("ERR %u. base=%lu n=%lu\n", ret_code, base, n);
            }
            break;
          case 'f' :
            if(sscanf(line, "f %li %li", (long int*)&base, (long int*)&n) <=0)
              printf("invalid use of free command, type ? for help\n");
            else
            {
              ret_code = res_bitmap_free(test_bitmap, base, n);
              if(0 == ret_code)
                printf("done\n");
              else
                printf("ERR %u. base=%lu n=%lu\n", ret_code, base, n);
            }
            break;
          case 'c' :
            if(sscanf(line, "c %li %li", (long int*)&base, (long int*)&n) <=0)
              printf("invalid use of check command, type ? for help\n");
            else
            {
              ret_code = res_bitmap_check(test_bitmap, base, n);
              if(ret_code < 3)
                printf("result: %u\n", ret_code);
              else
                printf("ERR %u. base=%lu n=%lu\n", ret_code, base, n);
            }
            break;
          case 'r' :
            if(sscanf(line, "r %li", (long int*)&n) <=0)
              printf("invalid use of resize command, type ? for help\n");
            else
            {
              ret_code = res_bitmap_resize(test_bitmap, n);
              if(0 == ret_code)
                printf("done\n");
              else
                printf("ERR %u. n=%lu\n", ret_code, n);
            }
            break;
          case 'n' :
            if(sscanf(line, "n %li %li", (long int*)&base, (long int*)&n) <=0)
              printf("invalid use of count command, type ? for help\n");
            else
            {
              n = res_bitmap_count(test_bitmap, base, n);
              if(RES_BITMAP_ERR == n)
                printf("ERR %u - %s\n", errno, res_err_string(errno));
              else
                printf("result: %lu\n", n);
            }
            break;
          case 's':
            n = res_bitmap_get_size(test_bitmap);
            if(RES_BITMAP_ERR == n)
              printf("ERR %u - %s\n", errno, res_err_string(errno));
            else
              printf("result: %lu\n", n);
            break;
          default :
            printf("invalid command, type ? for help\n");
        }
      }

      free(line);      
    }

  return(EXIT_SUCCESS);
}

void print_help()
{
   printf("commands:\n");
   printf("  q - exit test tool\n");
   printf("  p - print bitmap (using res_bitmap functions)\n");
   printf("  o - print bitmap memory (bypassing res_bitmap functions)\n");
   printf("  i - print bitmap info\n");
   printf("  a <n> - allocate <n> in bit map, returns base or error\n");
   printf("  t <base> <n> - take n bits, starting from base\n");
   printf("  f <base> <n> - free n bits, starting from base\n");
   printf("  c <base> <n> - check n bits, starting from base\n");
   printf("  r <n> - resize bitmap to n bits\n");
   printf("  n <base> <n> - count bits taken between base and base+n\n");
   printf("  s - get size of bitmap in bits\n");
   printf("n can be any non-signed decimal, octal, or hexadecimal integer\n");
   return;
}

void print_bitmap(res_bitmap_t* bitmap)
{
  size_t i;
  size_t j;
  ushort tmp;
    for(i=0; i<=res_bitmap_get_size(bitmap); i+=8)
    {
      if((i%64 == 0) && (i!=0))
        printf("\n");
      for(j=0; j<8; j++)
      {
        if(i+j > res_bitmap_get_size(bitmap))
          break;
        tmp = res_bitmap_check(bitmap, i+j, 0);
        printf("%hu", tmp);
      }
      printf(" ");
    }
    printf("\n");
}

void print_bitmap_2(res_bitmap_t* bitmap)
{
  size_t i;
  size_t j;
  uint8_t *bit = (uint8_t*) bitmap->base;
    for(i=0; i<=res_bitmap_get_size(bitmap); i+=8)
    {
      if((i%64 == 0) && (i!=0))
        printf("\n");
      for(j=0; j<8; j++)
      {
        if(i+j > res_bitmap_get_size(bitmap))
          break;
        if(0 != (bit[i/8] & (1<<j)))
        {
          printf("1");
        } else {
          printf("0"); 
        }
      }
      printf(" ");
    }
    printf("\n");
}

void print_bitmap_info(res_bitmap_t* bitmap)
{
  printf("handle=%p base=%p limit=0x%lx num_bits=%lu\n", (void*)bitmap, (void*)bitmap->base, bitmap->limit, bitmap->num_bits);
}

/* no getline?? Needs a posix 2008 compatible clib. Try the code here to fix:
 *   http://cvsweb.netbsd.org/bsdweb.cgi/pkgsrc/pkgtools/libnbcompat/files/getdelim.c?only_with_tag=MAIN
 *   http://cvsweb.netbsd.org/bsdweb.cgi/pkgsrc/pkgtools/libnbcompat/files/getline.c?only_with_tag=MAIN
 * you'll need both functions for it to work.
 */
