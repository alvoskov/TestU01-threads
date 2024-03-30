TestU01-threads is an extension of TestU01 library that allows to run
SmallCrush, Crush, BigCrush and pseudoDIEHARD test batteries in
a multi-threaded mode. TestU01-threads doesn't modify the original
library and just calls tests from it using its own multi-threaded
dispatcher.


The information about the original TestU01 library can be found at:

- Pierre L'Ecuyer, Richard Simard. TestU01: A C library for empirical
  testing of random number generators // ACM Trans. Math. Software. 2007.
  Vol. 33. Issue 4. Article No.: 22. pp 1-40.
  https://doi.org/10.1145/1268776.1268777
- https://github.com/umontreal-simul/TestU01-2009
- http://simul.iro.umontreal.ca/testu01/tu01.html

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

