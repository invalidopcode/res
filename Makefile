.PHONY: all clean check distclean
CC := gcc
LD := gcc
RM := rm
WARNINGS := -pedantic -Wall -Wshadow -Wextra -Wpointer-arith -Wcast-align -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Werror -Wwrite-strings -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wconversion
CFLAGS := -g -std=c11 $(WARNINGS)
LDFLAGS := $(CFLAGS)
RES_DEPENDS := res_config.h res_err.h res_types.h res_err_string.o
BITMAP_DEPENDS := $(RES_DEPENDS) bitmap.o bitmap.h
LIST_DEPENDS := $(RES_DEPENDS) list.o list.h
STACK_DEPENDS := $(RES_DEPENDS) stack.o stack.h

all: bitmap_test bitmap_interactive_test list_test stack_test

check: bitmap_test list_test stack_test
	./bitmap_test
	./list_test
	./stack_test

clean:
	-$(RM) *.o

distclean: clean
	-$(RM) bitmap_test
	-$(RM) list_test
	-$(RM) stack_test
	-$(RM) bitmap_interactive_test

bitmap_test: bitmap_test.c $(BITMAP_DEPENDS)
	$(CC) -c $(CFLAGS) bitmap_test.c -o bitmap_test.o
	$(LD) $(LDFLAGS) bitmap_test.o bitmap.o res_err_string.o -o bitmap_test

bitmap_interactive_test: bitmap_interactive_test.c $(BITMAP_DEPENDS)
	$(CC) -c $(CFLAGS) bitmap_interactive_test.c -o bitmap_interactive_test.o
	$(LD) $(LDFLAGS) bitmap_interactive_test.o bitmap.o res_err_string.o -o bitmap_interactive_test

list_test: list_test.c $(LIST_DEPENDS)
	$(CC) -c $(CFLAGS) list_test.c -o list_test.o
	$(LD) $(LDFLAGS) list_test.o list.o res_err_string.o -o list_test

stack_test: stack_test.c $(STACK_DEPENDS)
	$(CC) -c $(CFLAGS) stack_test.c -o stack_test.o
	$(LD) $(LDFLAGS) stack_test.o stack.o res_err_string.o -o stack_test

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
	
