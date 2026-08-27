# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)

## [0.11] 2026-08-27

### Added

- An experimental multithreaded mode for `testu01th_pipes` (`--threads` optional
  extra key). Not as efficient as direct usage C++ interface but still faster
  than the single-threaded original version TestU01.
- xorwow generator (two variants, the classical one by G.Marsaglia and its
  improved modification by A.L.Voskov)
- Assertions in SmallCrush, Crush and BigCrush batteries that control the total
  number of tests.

### Bugfix

- Some potential data races fixed (see the `init_TestU01_internals` function
  in `src\testu01_callbacks.cpp` for details). The bug was reported by
  [skeeto](https://www.reddit.com/r/RNG/comments/1vwnr1l/comment/p5invw8).
- `include/testu01_callbacks.h`: some workarounds for `STDC_HEADERS`
  redefinition inside TestU01 files.
- Binary stdin/stdout mode now correctly works in `src/testu01th_demo.cpp` and
  `src/testu01th_pipes.cpp`. It allows to send PRNG output to TestU01 through
  stdin/stdout (just as in PractRand) but only in one-threaded mode.
- `testu01th_pipes`: obsolete command line arguments documentation fixed.
- uint32_t to double conversion fixed in a 31-bit LCG, also new
  `uint32_to_udouble` helper function for such conversion.

### Changed

- CMakeLists.txt refactoring (still not complete).
- Auto-replacement of `:` to `_` in PRNG names loaded from SmokeRand plugins.
  The `:` symbol is used as a separator by TestU01 report generators.

## [0.10] 2026-08-24

An initial pre-release.

### Added

- Multithreaded runs of TestU01 batteries are now fully reproducible.
- Some refactoring (more `const`, especially for references etc.)
- TestU01 internals are now completely wrapped, i.e. no TestU01 header
  files are directly used by a TestU01-threads user. That includes wrappers
  for the original single-threaded versions of SmallCrush, Crush, BigCrush
  and pseudoDIEHARD.
- `src/example.cpp`: a very simple "Hello, World" example that shows how
  to use TestU01-threads for custom PRNGs testing.

### Bugfix

- Compiler warnings in GNU/Linux eliminated.
- `sstring_HammingWeight2_cb` signature bugfix (had no influence on
  computation results).
- Position independent code for `libtestu01threads.a` to enable compilation
  of the `libtestu01th_sr_ext.so` plugin for SmokeRand for GNU/Linux.
