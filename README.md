Reusable Resource Handling Code
===============================

Contents
--------
01. Intro
02. Usage & configuration
03. Lists
04. Bitmaps
05. Stacks
06. Buffers
07. Thread safety
08. Error handling
09. Compilers
10. Unit tests
11. Internal functions

Intro
-----
This is NOT a library - rather it's a set of code designed to be copied into
projects. It's made up of a set of functionality I found myself re-using in my
own projects, but often causing bugs. Essentially it's a set of known-good
functions with extensive automated testing. There are definitely opportunities
for optimisation that I hope to implement in future. The main reason this is
not a library is that the kind of functions here (eg linked lists) will almost
certainly need minor adaptations for use in real projects.

ALL function names MUST BE preceded by res\_\[modulename\]\_ - for example,
res\_bitmap\_create(). Generally, modules work with resources by means of handles
and pointers. The purpose of resource modules is to create extensible pieces of
code that can be included in projects simply.

Each module comes with a header (eg bitmap.h), and one or more .c files, each
of which start with the module name (eg bitmap.c, bitmap\_create.c). The
functions exported by each module are defined in an API. APIs have a minor
version number (for additions), and a major version number (for incompatible
changes). Different implementations may be created to run a different
algorithm, etc. Each implementation should have a unique name and release
number. Header files for each module MUST include API version numbers,
implementation name, and release number, clearly.

Each module MAY include "res\_types.h", "res\_config.h", and "res\_err.h"

See the api documentation for functions, etc

Note that all documentation follows the definitions in RFC 2119.

Usage
-----
To use code (eg stack.c, bitmap.c, etc) you simply need to include the relevant
header (eg stack.h for stack functions). This header will include in anything
else necessary (eg res\_config.h)

res\_config tries to determine if the system is 32 or 64 bit itself (used for
optimisation). However, if it fails then BITS\_32 or BITS\_64 must be defined for
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

Buffers
-------
Are a safe way of using c buffers - providing bounds checking and automatic
growth (using realloc). The main expected use-case is for preparing a server
response one part at a time, calling res\_buffer\_appendf() multiple times
to append format strings safely to the buffer. If the buffer is too small
then it will automatically have memory allocated to grow to fit.

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
Some functions (see api\_functions) use stdc "errno" to pass information. SOME
common error numbers are defined in res\_err.h. The c function
res\_err\_string(err) (documented in api\_functions) is implemented in
res\_err\_string.c and defined in res\_err.h, and MAY return a string explaining
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
Unit tests MAY be written in files [modulename]\_test.c

Interactive tests MAY be written in files [modulename]\_interactive\_test.c

Internal Functions
------------------
\_res\_...\_size and \_res\_...\_set functions are implemented for SOME 'external'
functions that allocate memory. These MAY be standard c functions OR they
MAY be implemented as macros or inlines in the module header file. (This will
be useful in freestanding environments where basic memory management functions
are unavailable)

