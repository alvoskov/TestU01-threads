TestU01-threads is an extension of TestU01 library that allows to run
SmallCrush, Crush, BigCrush and pseudoDIEHARD test batteries in
a multi-threaded mode. TestU01-threads doesn't modify the original
library and just calls tests from it using its own multi-threaded
dispatcher. The key features are:

- Multi-threaded test batteries from TestU01 without modification
  of TestU01 itself. It allows to use the precompiled library
  in some GNU/Linux distributions (e.g. Ubuntu)
- Allows to repeat the specified test from the specified battery
  without rerunning other tests just by command line arguments.
- C and C++ interfaces that is designed especially for enveloping
  TestU01 objects/structures. C interface is designed especially
  for making external modules (SO/DLL).
- Can send PRNG output to PractRand by means of file streams.
  Both 32-bit and 64-bit PRNGs are supported.
- Some examples of PRNG including CSPRNG ChaCha12.


The information about the original TestU01 library can be found at:

- Pierre L'Ecuyer, Richard Simard. TestU01: A C library for empirical
  testing of random number generators // ACM Trans. Math. Software. 2007.
  Vol. 33. Issue 4. Article No.: 22. pp 1-40.
  https://doi.org/10.1145/1268776.1268777
- https://github.com/umontreal-simul/TestU01-2009
- http://simul.iro.umontreal.ca/testu01/tu01.html


Previous attemps to make multithreaded TestU01 were based on rewriting some
tests using parallel programming, e.g. OpenMP. However, they were not completed
and don't include full SmallCrush, Crush, BigCrush and pseudoDIEHARD batteries.

- A. Suciu, R. A. Toma and K. Marton, "Parallel implementation of the TestU01
  statistical test suite," 2012 IEEE 8th International Conference on Intelligent
  Computer Communication and Processing, Cluj-Napoca, Romania, 2012, pp. 317-322
  doi: 10.1109/ICCP.2012.6356206.
- A. Suciu, R. A. Toma and K. Marton, "Parallel object-oriented implementation
  of the TestU01 statistical test suites," 2014 IEEE 10th International Conference
  on Intelligent Computer Communication and Processing (ICCP), Cluj-Napoca, Cluj,
  Romania, 2014, pp. 311-315, doi: 10.1109/ICCP.2014.6937014.
- https://github.com/adamsolomou/TestU01

Executables
===========

  Executable        | Function
--------------------|----------------------
 `chacha_avx_test`  |
 `chacha_test`      |
 `testu01th_lib`    |
 `testu01th_demo`   |
 `splitmix_exec`    |


Supplied PRNGs external modules
===============================


- alfib
- chacha_avx
- chacha
- isaac
- kiss93
- lfib_float
- lfib_ranmar_float
- lfib_vfloat
- mlfib17_5
- philox
- rc4
- sqxor
- sqxor32
- threefry
- wyrand
- xoroshiro128stst
- xorwow



C module interface
==================

A module with PRNG implementation that supports C API should export the next
functions:

- `int gen_initlib(CallerAPI *intf)` - initializes the library and gets
   a pointer to the structure with pointers to some functions of TestU01-threads
   library (e.g. for obtaining seeds, controlling dynamic memory.
- `int gen_closelib(void)` - called before closing the library.
- `int gen_getinfo(GenInfoC *gi)` - should fill the `GenInfoC` structure
   with information about generator (mainly with pointer to its callback
   functions)



Compilation
===========

The program can be compiled for Linux or MS Windows using GCC compiler
and CMake building system. For some Linux distributions it is possible
to install TestU01-1.2.3 from repositories. E.g. for Ubuntu the next
packages should be installed:

- libtestu01-0:amd64 (contains the compiled TestU01 library)
- libtestu01-0-dev-common (contains the TestU01 header files)

For MS Windows the TestU01 library should be compiled from its source code.
The original TestU01-2009 uses GNU Autoconf for building, and it may be
simpler to use modifications with CMake support, e.g.:

- https://github.com/JamesHirschorn/TestU01-CMake

