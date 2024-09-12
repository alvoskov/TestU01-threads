Introduction
============

TestU01-threads is an extension of TestU01 library that allows to run
SmallCrush, Crush, BigCrush and pseudoDIEHARD test batteries in
a multi-threaded mode. TestU01-threads doesn't modify the original
library and just calls tests from it using its own multi-threaded
dispatcher. It also allows to send PRNG output to stdout that is
required for testing with PractRand. The key features are:

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

  Executable        | Function
--------------------|------------------------------------------------------
 `testu01th_run`    | Runs PRNGs tests for arbitrary PRNG from C module
 `testu01th_demo`   | Demonstration of calling batteries from C++ program
 `splitmix_exec`    | Demonstration of calling batteries from C program


Supplied PRNGs external modules
===============================

 Module name      | Generator
------------------|---------------------------------------------------------------------------
 alfib            | \f$ LFib(+,2^{64},607,203) \f$
 alfib_mod        | \f$ LFib(+,2^{64},607,203) \f$ XORed by "Weyl sequence"
 chacha_avx       | ChaCha12 CSPRNG: AVX2 implementation
 chacha           | ChaCha12 CSPRNG: Cross-platform implementation 
 coveyou64        |
 cmwc4096         | CMWC4096 "Mother-of-all" PRNG by G.Marsaglia
 isaac64          | ISAAC64 CSPRNG
 kiss93           | KISS93 (doesn't pass Crush and BigCrush)
 kiss99           | KISS99
 kiss64           | 64-bit version of KISS
 lcg64            | \f$ LCG(2^{64},6906969069,1) \f$ that returns upper 32 bits
 lcg128           | \f$ LCG(2^{128},18000690696906969069,1) \f$, returns upper 32/64 bits
 lcg69069         | \f$ LCG(2^{32},69069,1)\f$, returns whole 32 bits
 lfib_ranmar      | RANMAR: subtractive lagged Fibonacci + "Weyl sequence"
 minstd           | \f$ LCG(2^{31} - 1, 16807, 0)\f$ "minimial standard" obsolete generator.
 mlfib17_5        | \f$ LFib(x,2^{64},17,5) \f$
 mt19937          | Mersenne twister from C++ standard library.
 msws             | Middle-Squares Weyl Sequence PRNG by B.Widynski
 mwc32x           | Similar to MWC64X, but x and c are 16-bit
 mwc64x           | MWC64X: 32-bit Multiply-With-Carry with XORing x and c
 mwc128x          | MWC128X: similar to MWC64X but x and c are 64-bit
 pcg32            | Permuted Congruental Generator (32-bit version, 64-bit state)
 pcg64            | Permuted Congruental Generator (32-bit version, 64-bit state)
 philox           | Philox4x64x10 (weakened and altered ThreeFish)
 philox32         | Philox4x32x10 (weakened and altered ThreeFish)
 randu            | \f$ LCG(2^{32},65539,1) \f$, returns whole 32 bits
 ranluxpp         | RANLUX++, RANLUX reformulated as LCG
 rc4              | RC4 obsolete CSPRNG (doesn't pass PractRand)
 rrmxmx           | Modified SplitMix PRNG with improved output function
 seigzin63        | \f$ LCG(2^{63}-25,a,0) \f$
 sfc64            | "Small Fast Chaotic 64-bit" PRNG by 
 squares64        | Counter-based 64-bit PRNG by B.Widynski
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
3. High-quality PRNGs that pass BigCrush: CMWC, KISS99, KISS64, LCG128,
   multiplicative lagged Fibonacci, MWC64X, MWC128X, RANLUX++,
   sqxor, wyrand, xoroshiro128**.
4. RC4: passes BigCrush but fails PractRand (obsolete and slow CSPRNG).
5. RANMAR: passes BigCrush after some modification, fails PractRand.
6. PRNGs that pass SmallCrush but fail more complex tests: Coveyou64, KISS93,
   LCG64, Mersenne Twister, MWC32X, sqxor32, xorwow.
7. Low-quality PRNGs: lcg69069, minstd
8. RANDU: low-quality LCG that fails almost all statistical tests.
   


 Module name      | Type   | SmallCrush | Crush | BigCrush | PractRand    | cpb
------------------|--------|------------|-------|----------|--------------|-----------
 alfib            | u32    | +          | -     | -        | 128 GiB      | 0.34
 alfib_mod        | u32    | +          | +     | +        | 1 TiB        | 0.40
 chacha_avx       | u32    | +          | +     |          |              | 0.91
 chacha           | u32    | +          | +     | +        |              | 2.04
 coveyou64        | u32    | +          | -     | -        | 256 KiB      | 0.46
 cmwc4096         | u32    | +          | +     | +        | >= 1 TiB     | 0.36
 isaac64          | u64    | +          | +     | +        |              | 0.85
 kiss93           | u32    | +          | -     | -        | 1 MiB        | 0.85
 kiss99           | u32    | +          | +     | +        | >=1 TiB      | 1.08
 kiss64           | u64    | +          | +     | +        |              | 0.41
 lcg64            | u32    | +          | -     | -        | 16 MiB       | 0.41
 lcg128           | u32/64 | +          | +     | +        | >=2TiB/64GiB | 0.53/0.29
 lcg69069         | u32    | -          | -     | -        | 2 KiB        | 0.40
 lfib_ranmar      | double | +          | +     | +        | < 1KiB       | 6.07
 minstd           | u32    | -          | -     | -        | 1 KiB        | 2.71
 mlfib17_5        | u32    | +          | +     | +        | >=1TiB       | 0.38
 mt19937          | u32    | +          | -     | -        | 128 GiB      | 1.38
 msws             | u32    | +          | +     | +        |              | 0.53
 mwc32x           | u32    | +          | -     | -        | 256MiB       | 1.45
 mwc64x           | u32    | +          | +     | +        | >=8TiB       | 0.57
 mwc128x          | u64    | +          | +     | +        | >=1TiB       | 0.21
 pcg32            | u32    | +          | +     | +        |              | 0.47
 pcg64            | u64    | +          | +     | +        |              | 0.30
 philox           | u64    | +          | +     | +        |              | 0.95
 philox32         | u64    | +          | +     | +        |              | 1.91
 randu            | u32    | -          | -     | -        | 1 KiB        | 0.37
 ranluxpp         | u64    | +          | +     | +        |              | 3.82
 rc4              | u32    | +          | +     | +        | 512 GiB      | 7.41
 rrmxmx           | u64    | +          | +     | +        |              | 0.18
 seigzin63        | u32    | +          | +     | -+       | >= 2TiB      | 3.50
 sfc64            | u32    | +          | +     | +        | >= 1TiB      | 0.12
 splitmix         | u64    | +          | +     |          |              | 0.19
 shr3             | u32    | -          | -     | -        | 32 KiB       | 0.82
 sqxor            | u64    | +          | +     | +        |              | 0.14
 sqxor32          | u32    | +          | -     | -        | 16 GiB       | 0.24
 squares64        | u64    | +          | +     | +        | >=1 TiB      | 0.42
 threefry         | u64    | +          | +     | +        |              | 1.14
 wyrand           | u64    | +          | +     | +        |              | ~0.1
 xoroshiro128stst | u64    | +          | +     |          |              | 0.28
 xorwow           | u32    | +          | -     | -        | 128 KiB      | 0.73
 xsh              | u64    | -          | -     | -        | 32 KiB       | 0.42



C module interface
==================

A module with PRNG implementation that supports C API should export the next
three functions:

- `int gen_initlib(CallerAPI *intf)` - initializes the library and gets
   a pointer to the structure with pointers to some functions of
   TestU01-threads library, e.g. for obtaining seeds, controlling
   dynamic memory.
- `int gen_closelib(void)` - called before closing the library.
- `int gen_getinfo(GenInfoC *gi)` - should fill the `GenInfoC` structure
   with information about generator (mainly with pointer to its callback
   functions)

C modules should be compiled as freestanding, i.e. don't use any functions from
standard library and other libraries. However, CallerAPI structure contains
pointer to some functions useful for PRNG construction:

- `get_seed64` - get random 64-bit seed using hardware random numbers generator.
- `malloc` - pointer to malloc function from C standard library.
- `free` - pointer to free function from C standard library.
- `printf` - pointer to printf function.
- `strcmp` - pointer to strcmp function from C standard library.

Function prototype for `get_seed64`:

- `uint64_t get_seed64(void);`

The next fields in GenInfoC structure should be filled by `gen_getinfo`:

- `name` - generator name, will be used in reports.
- `init_state` - initializes the generator state.
- `delete_state` - deletes the generator state.
- `get_u01` - returns the double pseudorandom number.
- `get_bits32` - returns the uint32_t pseudorandom number.

Functions prototypes:

- `void *init_state(void);`
- `void delete_state(void *param, void *state);`
- `double get_u01(void *param, void *state);`
- `unsigned long get_bits32(void *param, void *state);`

Remember that `unsigned long` may be either 32-bit or 64-bit, it should be 
taken into account in PRNG implementations. This is made for compatibility
with TestU01 subroutines and reducing function calls overhead. The first
argument `param` is not used by TestU01-threads and left mainly for
compatibility with TestU01 and for future use.

The next fields are optional but may be filled:

- `get_bits64` - returns the uint64_t pseudorandom number.
- `get_array32` - fills the uint32_t array buffer with pseudorandom numbers.
- `get_array64` - fills the uint64_t array buffer with pseudorandom numbers.
- `get_sum32` - returns the sum of uint32_t pseudorandom number.
- `get_sum64` - returns the sum of uint64_t pseudorandom number.
- `run_self_test` - runs the internal self-test.

Functions prototypes:

- `uint64_t get_bits64(void *param, void *state);`
- `void get_array32(void *param, void *state, uint32_t *out, size_t len);`
- `void get_array64(void *param, void *state, uint64_t *out, size_t len);`
- `uint32_t get_sum32(void *param, void *state, size_t len);`
- `uint64_t get_sum64(void *param, void *state, size_t len);`
- `int run_self_test(void);`

The next fields are filled in GenInfoC before `gen_getinfo` is called and used
to transfer information to the PRNG initialization

- `options` - string with generator options


Predefined macroses and inline functions for C modules
=================================

C interface is rather simple but may require to write a lot of boilerplate
code. Some macroses are predefined in `testu01th/cinterface.h` to reduce
its amount in the most common situations.


- `UINT128_ENABLED` - defined only if 128-bit integers such as `__uint128_t`
  are available.
- `PRNG_CMODULE_PROLOG` - implements the default code for entry point and
  default versions of `gen_initlib` and `gen_closelib` functions. It also
  defines the `static CallerAPI intf;` variable with pointers to API functions
  for caller such as printf, malloc etc.
- `MAKE_UINT32_PRNG(prng_name, selftest_func)` - implements default versions
  of all exported functions except `init_state` for 32-bit PRNG. The PRNG code
  should be inside the
  `static inline uint32_t get_bits32_raw(void *param, void *state)` function.
- `MAKE_UINT64_UPTO32_PRNG(prng_name, selftest_func)` - implements default
  versions of all exported functions except `init_state` for 64-bit PRNG.
  32-bit integers are formed from the upper 32-bits, one 64-bit integer
  is converted to one double. The PRNG code should be inside the next function:
  `static inline uint64_t get_bits64_raw(void *param, void *state)`
- `MAKE_UINT64_INTERLEAVED32_PRNG(prng_name, Type, selftest_func)` - 
  implements default versions of all exported functions except `init_state`
  for 64-bit PRNG. For 32-bit integers interleaved output of higher and lower
  halves of 64-bit integers are implemented. But one 64-bit integer
  is converted to one double. The PRNG code should be inside the next function:
  `static inline uint64_t get_bits64_raw(void *param, void *state)`

If interleaved 32-bit output is desired then the structure with PRNG state
must include the i32buf field that has `Interleaved32Buffer` data type.
This field must me initialized by the next inline function:

- `void Interleaved32Buffer_init(Interleaved32Buffer *obj)`

The next inline functions are predefined in `testu01th/cinterface`:

- `double uint64_to_udouble(uint64_t val)` - converts unsigned 64-bit number
  to double precision float in the 0 <= u < 1 interval. The 52 bits are written
  into mantissa. Then 1.0 is subtracted from the float in the 0 <= u < 1
  interval.
- `double uint32_to_udouble(uint32_t val)` - converts unsigned 32-bit integer
  to double precision float in the 0 <= u < 1 interval by multiplying
  by constant.
- `uint64_t pcg_bits64(uint64_t *state)` - pcg_rxs_m_xs64 generator for
  initialization for other PRNGs such as lagged Fibonacci.
- `uint64_t unsigned_mul128(uint64_t a, uint64_t b, uint64_t *high)` -
  cross-compiler implementation of 128-bit multiplication, useful for such
  generators as Philox. Allows to work with MSVC.


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

