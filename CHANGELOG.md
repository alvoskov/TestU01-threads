# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)

## [0.10] 2026-08-23

An initial pre-release.

### Added

- Multithreaded runs of TestU01 batteries are now fully reproducible.
- Some refactoring (more `const`, especially for references etc.)

### Bugfix

- Compiler warnings in GNU/Linux eliminated.
- `sstring_HammingWeight2_cb` signature bugfix (had no influence on
  computation results).
- Position independent code for `libtestu01threads.a` to enable compilation
  of the `libtestu01th_sr_ext.so` plugin for SmokeRand for GNU/Linux.
