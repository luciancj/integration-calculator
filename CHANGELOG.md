# Changelog

All notable changes to this project will be documented in this file.

## [v1.0.0] - 2026-05-28

### Added
- CLI with interactive mode (single function) and parameter mode (batch CSV)
- wxWidgets GUI with integration plot, history, and user authentication
- Five adaptive integration methods: Right Riemann, Left Riemann, Midpoint Riemann, Trapezoidal, Simpson's
- Recursive-descent expression parser supporting `+` `-` `*` `/` `^`, functions (`sin`, `cos`, `tan`, `exp`, `ln`, `sqrt`, `abs`, `pow`), and constants (`pi`, `e`)
- `back` navigation at every input prompt to correct previous values without restarting
- Batch CSV processing with per-row error markers in output
- Integration statistics: area, min, max, mean, RMS, delta, segment count
- CTest suite of 24 tests covering parser, all integration methods, stats, and CSV processor
- Out-of-source CMake build; GUI and tests are opt-in (`-DBUILD_GUI=ON`, `-DBUILD_TESTS=ON`)
