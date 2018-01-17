Reusable Resource Handling Code
===============================

Contents
--------
01. Intro
02. Usage & configuration
03. Lists
04. Bitmaps
05. Stacks
06. Thread safety
07. Error handling
08. Compilers
09. Unit tests
10. Internal functions

Intro
-----
This is NOT a library - rather it's a set of code designed to be copied into
projects. It's made up of a set of functionality I found myself re-using in my
own projects, but often causing bugs. Essentially it's a set of known-good
functions with extensive automated testing. There are definitely opportunities
for optimisation that I hope to implement in future. The main reason this is
not a library is that the kind of functions here (eg linked lists) will almost
certainly need minor adaptations for use in real projects.

ALL function names MUST BE preceded by res_[modulename]_ - for example,
res_bitmap_create(). Generally, modules work with resources by means of handles
and pointers. The purpose of resource modules is to create extensible peices of
code that can be included in projects simply.

Each module comes with a header (eg bitmap.h), and one or more .c files, each
of which start with the module name (eg bitmap.c, bitmap_create.c). The
functions exported by each module are defined in an API. APIs have a minor
version number (for additions), and a major version number (for incompatible
changes). Different implementations may be created to run a different
algorithm, etc. Each implementation should have a unique name and release
number. Header files for each module MUST include API version numbers,
implementation name, and release number, clearly.

Finally, each module MAY include "res_types.h", "res_config.h", and "res_err.h"

See the api documentation for functions, etc

Usage
-----
To use code (eg stack.c, bitmap.c, etc) you simply need to include the relevant
header (eg stack.h for stack functions). This header will include in anything
else necessary (eg res_config.h)

res_config tries to determine if the system is 32 or 64 bit itself (used for
optimisation). However, if it fails then BITS_32 or BITS_64 must be defined for
it. 16-bit and 8-bit systems are not supported. There is no other configuration
necessary.

Lists
-----
A list is a linked list, sorted using a "sortkey", with elements optionally
containing type and ID values of the resources they point to. A list is made up
of 'entries', referred to by the order in which they occur (eg the first entry
is entry 0). This has implications for thread safety, in that the order may
change (new entries added, or removed), such that the list entry that an entry
number refers to may change during execution. This means that it is very
important that only one thread accesses a given list at any one time.

A list is ordered according to the 'sort key' contained in each entry - a
higher value means the entry is place closer to the start of the list. Each
list entry may also contain other fields, which do not effect how the entry is
stored in the list. These are: the resource pointer, the resource ID, and the
resource type. The resource pointer is intended to be a pointer to the resource
that the entry is 'about'. The resource ID is intended to be a unique
identifier for the resource, so that the programmer has a persistent way to
refer to each entry. However, the uniqueness of each resource ID is not
verified, and the field may be utilised in whatever way the programmer wishes.
The resource type is intended to be a non-unique identifier, so that the
programmer may include information in each entry about what kind of resource
the pointer points to. None of these fields HAS to be utilised

Bitmaps
-------
A bitmap is a set of bits in memory. Normally this will be used as a 'map' for
keeping track of a resource - for example, each bit might represent whether or
not one particular area of memory had been allocated. Four operations are
provided that can be performed on a block of bits in a bitmap. The first,
'alloc', is used to find and mark a continuous set of a given number of
un-marked bits. 'free' is used to un-mark a set of bits, and 'take' does the
opposite - ensuring all bits in a given set are marked. Finally, 'check' tests
a set of bits in the bitmap, and returns a different value depending on whether
they are all marked, all un-marked, or all different.

Part of the motivation for including bitmaps is that there are a number of
different possible algorithms for allocating bits. Having a simple interface to
interact with bitmaps, independent of the algorithm used, means that the
algorithm can be modified without making any changes to the main source code of
an application that uses it. However, the algorithm implemented at present is
intended only as a placeholder, as it is very inefficient.

Stacks
------
A stack is a set of pointers, stored in the order in which they are saved, and
retrieved in reverse order (ie first on, last off). Loading pointers from
elsewhere in a stack is slower, whereas using them in the intended way is very
fast. Stacks may be resized at run-time.

Thread Safety
-------------
Resources SHOULD only be written to when no other threads are reading or writing
to them. This is especially important in the case of lists, as the location of
items in the list (and hence the identifier used to refer to items and read
their properties), may change when other items are added or removed.

Error reporting via errno will only be thread-safe if the std c library is
thread-safe (ie is C11 compliant)

Error Handling
--------------
Some functions (see api_functions) use stdc "errno" to pass information. SOME
common error numbers are defined in res_err.h. The c function
res_err_string(err) (documented in api_functions) is implemented in
res_err_string.c and defined in res_err.h, and MAY return a string explaining
the error. If this function does not recognise the error number then it will
return the result of the stdc strerr function, since some res functions pass
on errno from failed stdc functions (eg from a failed malloc call).

Error strings are in English only at present.

Compilers
---------
Requires at least a C99 standard C compiler to compile (assumes, for instance,
rounding towards 0)

Tests
-----
Unit tests MAY be written in files [modulename]_test.c
Interactive tests MAY be written in files [modulename]_interactive_test.c

Internal Functions
------------------
_res_..._size and _res_..._set functions are implemented for SOME 'external'
functions that allocate memory. These MAY be standard c functions OR they
MAY be implemented as macros or inlines in the module header file. (This will
be useful in freestanding environments where basic memory management functions
are unavailable)

