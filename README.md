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

 Module name      | Generator
------------------|------------------------------------------------------------
 alfib            | LFib(+,2^{64},607,203)
 chacha_avx       | ChaCha12 CSPRNG: AVX2 implementation
 chacha           | ChaCha12 CSPRNG: Cross-platform implementation 
 coveyou64        |
 isaac64          | ISAAC64 CSPRNG
 kiss93           | KISS93 (doesn't pass Crush and BigCrush)
 kiss99           | KISS99
 kiss64           | 64-bit version of KISS
 lcg64            | LCG(2^{64},6906969069,1) that returns upper 32 bits
 lcg128           | LCG(2^{128},18000690696906969069,1), returns upper 32 bits
 lcg69069         | LCG(2^{32},69069,1), returns whole 32 bits
 lfib_ranmar      | RANMAR: subtractive lagged Fibonacci + "Weyl sequence"
 minstd           |
 mlfib17_5        | LFib(x,2^{64},17,5)
 mt19937          | Mersenne twister from C++ standard library.
 mwc32x           | Similar to MWC64X, but x and c are 16-bit
 mwc64x           | MWC64X: 32-bit Multiply-With-Carry with XORing x and c
 mwc128x          | MWC128X: similar to MWC64X but x and c are 64-bit
 philox           | Philox4x64x10 (weakened and altered ThreeFish)
 randu            | LCG(2^{32},65539,1), returns whole 32 bits
 ranluxpp         | RANLUX++, RANLUX reformulated as LCG
 rc4              | RC4 obsolete CSPRNG (doesn't pass PractRand)
 sqxor            | sqxor
 sqxor32          | sqxor32
 threefry         | Threefry4x64x20 (ThreeFish with reduced number of rounds) 
 wyrand           | wyrand: output function of wyhash + Weyl sequence
 xoroshiro128stst | xoroshiro128**
 xorwow           | xorwow


The supplied generators can be divided into several groups:

1. Cryptographically secure pseudorandom numbers generators (CSPRNG):
   ChaCha12, ISAAC64
2. Simplified generators based on CSPRNG: Philox, Threefry.
3. High-quality PRNGs that pass BigCrush: KISS99, KISS64, LCG128,
   multiplicative lagged Fibonacci, MWC64X, MWC128X, RANLUX++,
   sqxor, wyrand, xoroshiro128**.
4. RC4: passes BigCrush but fails PractRand (obsolete and slow CSPRNG).
5. RANMAR: passes BigCrush after some modification, fails PractRand.
6. PRNGs that pass SmallCrush but fail more complex tests: Coveyou64, KISS93,
   LCG64, Mersenne Twister, MWC32X, sqxor32, xorwow.
7. Low-quality PRNGs: lcg69069, minstd
8. RANDU
   


 Module name      | Type   | SmallCrush | Crush | BigCrush | PractRand
------------------|--------|------------|-------|----------|-----------
 alfib            | u32    | +          |       |          |
 chacha_avx       | u32    | +          |       |          |
 chacha           | u32    | +          |       |          |
 coveyou64        | u32    | +          | -     | -        | 256 KiB
 isaac64          | u64    | +          |       |          |
 kiss93           | u32    | +          |       |          | 1 MiB
 kiss99           | u32    | +          |       |          |
 kiss64           | u64    | +          |       |          |
 lcg64            | u32    | +          | -     | -        | 16 MiB
 lcg128           | u32/64 | +          |       |          |
 lcg69069         | u32    | -          |       |          | 2 KiB
 lfib_ranmar      | double | +          | +     | +        | < 1KiB
 minstd           | u32    | -          | -     | -        | 1 KiB
 mlfib17_5        | u32    | +          |       |          |
 mt19937          | u32    | +          | -     | -        |
 mwc32x           | u32    | +          | -     | -        | 256MiB
 mwc64x           | u32    | +          | +     |          | 
 mwc128x          | u64    | +          |       |          |
 philox           | u64    | +          | +     |          |
 randu            | u32    | -          | -     | -        | 1 KiB
 ranluxpp         | u64    | +          |       |          | 
 rc4              | u32    | +          |       |          |
 sqxor            | u64    | +          |       |          |
 sqxor32          | u32    | +          |       |          | 16 GiB
 threefry         | u64    | +          |       |          |
 wyrand           | u64    | +          |       |          |
 xoroshiro128stst | u64    | +          |       |          |
 xorwow           | u32    | +          | -     | -        | 128 KiB



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

