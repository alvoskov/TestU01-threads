Introduction
============

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
  TestU01 objects/structures.
- Some examples of PRNGs.
- Possible compilation as an external module for SmokeRand PRNG test
  suite that allows to load generators from shared libraries (SO/DLL)
  with C interface. It also contains filters like reverse order of
  bits, upper half/lower half/interleaved modes for 64-bit PRNGs.
  https://github.com/alvoskov/SmokeRand

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
  statistical test suite," 2012 IEEE 8th International Conference on
  Intelligent Computer Communication and Processing, Cluj-Napoca, Romania,
  2012, pp. 317-322  doi: 10.1109/ICCP.2012.6356206.
- A. Suciu, R. A. Toma and K. Marton, "Parallel object-oriented implementation
  of the TestU01 statistical test suites," 2014 IEEE 10th International
  Conference on Intelligent Computer Communication and Processing (ICCP),
  Cluj-Napoca, Cluj, Romania, 2014, pp. 311-315, doi: 10.1109/ICCP.2014.6937014.
- https://github.com/adamsolomou/TestU01

TestU01-threads uses an entirely different approach: it doesn't modify the
original TestU01 library but just replaces single-threaded batteries
implementation from `bbattery.c` (sequental call of statistical tests) to its
own multithreaded version (parallel call of statistical tests from different
threads). These statistical tests are reentrant. Such modification also
requires new API for pseudorandom number generators: threads should be able to
create its own examples of generators.

There is another program with similar approach: TestU01-parallel. However, it
was forked from an older non-free version of TestU01. It also almost not
documented and uses Python scripts for running different processes with
slightly modified TestU01.

- https://github.com/rski/testu01-parallel

Executables
===========

  Executable           | Function
-----------------------|---------------------------------------------------
 `testu01th_run`       | Runs PRNGs tests for arbitrary PRNG from C module
 `testu01th_pipes`     | Reads PRNG output from stdin and sends to TestU01
 `libtestu01th_sr_ext` | Plugin for SmokeRand


Supplied PRNGs external modules
===============================

The next pseudorandom number generators are supplied with testu01-threads:

- MT19937 plugin based on C++11 standard library
- Linear congruential generators: minstd and lcg59
- KISS93

Earlier pre-release versions of TestU01-threads included a large collection
of plugins with generators but all of them were transferred to SmokeRand test
suite (https://github.com/alvoskov/SmokeRand).

Compilation
===========

The program can be compiled for GNU/Linux or MS Windows using GCC or MSVC
compilers and CMake building system. For some Linux distributions it is
possible to install TestU01-1.2.3 from repositories. E.g. for Ubuntu the next
packages should be installed:

- `libtestu01-0:amd64` (contains the compiled TestU01 library)
- `libtestu01-0-dev-common` (contains the TestU01 header files)

For MS Windows the TestU01 library should be compiled from its source code.
The original TestU01-2009 uses GNU Autoconf for building, and it may be
simpler to use modifications with CMake support, e.g.:

- https://github.com/JamesHirschorn/TestU01-CMake

