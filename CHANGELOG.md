# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)

## [0.11]

### Bugfix

- Some potential data races fixed (see the `init_TestU01_internals` function
  in `src\testu01_callbacls.cpp` for details). The bug was reported by
  [skeeto](https://www.reddit.com/r/RNG/comments/1vwnr1l/comment/p5invw8).
- Binary stdin/stdout mode now correctly works in `src/testu01th_demo.cpp` and
  `src/testu01th_pipes.cpp`. It allows to send PRNG output to TestU01 through
  stdin/stdout (just as in PractRand) but only in one-threaded mode.
- `testu01th_pipes`: obsolete command line arguments documentation fixed.

### Changed

- CMakeLists.txt refactoring.

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
