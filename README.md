# Integration Calculator - Final Project Documentation

![License](https://img.shields.io/badge/license-GPLv2-blue.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey.svg)
![Language](https://img.shields.io/badge/language-C%2B%2B23-orange.svg)
![Build](https://img.shields.io/badge/build-CMake-064F8C.svg?logo=cmake)
![Tests](https://img.shields.io/badge/tests-24%20passing-brightgreen.svg)
![GUI](https://img.shields.io/badge/GUI-wxWidgets-43A047.svg)

## Introduction

**Problem definition.** A _definite integral_ ∫ₐᵇ f(x) dx is the signed area under a
function `f` between two bounds `a` and `b`. For many functions this area has no
closed-form solution, so it must be approximated **numerically**. This project is a
command-line program that lets a user enter an arbitrary function of `x` and an
interval, and then estimates the integral using one of several classical methods.

**Overview of the solution.** The program is built in **C++23** and is split into
two layers:

- an **engine** that turns the typed function into an _Abstract Syntax Tree_ (AST),
  evaluates it, and runs the numerical integration; and
- a **command-line interface (CLI)** that presents menus, collects input, and
  formats the results.

It offers two modes of operation: an **interactive mode** for a single function and
a **parameter (batch) mode** that reads many functions from a CSV file and writes a
CSV report.

**Terminology.**

| Term                            | Meaning                                                                                         |
| :------------------------------ | :---------------------------------------------------------------------------------------------- |
| Definite integral               | Signed area under `f(x)` between `a` and `b`.                                                   |
| Numerical integration           | Approximating that area by summing many small slices.                                           |
| AST (expression tree)           | A tree representation of a parsed mathematical expression.                                      |
| Adaptive method                 | Keeps doubling the number of slices until the result stops changing by more than the tolerance. |
| Tolerance                       | The accuracy target, entered as an integer _k_ and applied as 10⁻ᵏ.                             |
| Riemann / Trapezoidal / Simpson | Families of numerical-integration rules of increasing accuracy.                                 |

---

## Program Interface

This section covers only how to activate and deactivate the program; the
step-by-step communication is described under [Program Execution](#program-execution).

### Environment and prerequisites

The program is platform-agnostic and builds with **CMake** and any C++23 compiler.

- **Linux (Ubuntu/Debian):** `sudo apt-get install cmake g++ build-essential`
- **macOS:** `xcode-select --install` (or install Xcode)
- **Windows:** Visual Studio (C++ workload) or MinGW-w64, plus CMake

### Building

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

To also build the wxWidgets GUI (requires wxWidgets installed):

```bash
cmake -S . -B build -DBUILD_GUI=ON
cmake --build build
```

To build and run the unit tests:

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

### Starting the program

The program takes **no command-line parameters**. Run the produced executable:

```bash
./build/IntegrationCalculator      # from the project root
# or, from inside build/:
./IntegrationCalculator
```

### Terminating the program

From the main menu, choose **`/e`** (Exit). The program can also be ended at any
prompt by sending end-of-input (**Ctrl+D** on Linux/macOS, **Ctrl+Z** then Enter on
Windows), which the program treats as a clean exit.

---

## Program Execution

This is the user manual: what the user does and what the program shows. No
algorithms or data structures are described here (see [Program Structure](#program-structure)).

### Main menu

On start the program prints a banner and a menu:

| Command | Mode        | Description                                   |
| :------ | :---------- | :-------------------------------------------- |
| `/i`    | Interactive | Integrate a single function entered by hand.  |
| `/p`    | Parameter   | Batch-process many functions from a CSV file. |
| `/e`    | Exit        | Close the program.                            |

(The leading `/` is optional - `i` and `/i` are both accepted.)

### Interactive mode (`/i`)

1. **Choose an integration method** by its two-letter code:

   | Code | Method               |
   | :--- | :------------------- |
   | `rr` | Right Riemann Sum    |
   | `lr` | Left Riemann Sum     |
   | `mr` | Midpoint Riemann Sum |
   | `tr` | Trapezoidal Rule     |
   | `sr` | Simpson's Rule       |

2. **Enter the interval start `a`**, then **end `b`** - a number or a constant
   expression (e.g. `0`, `pi`, `pi/4`, `2*e`).
3. **Enter the tolerance** as a whole number from 1-15 (e.g. `4` means 10⁻⁴).
4. **Enter the function** in terms of `x` (e.g. `sin(x) + 2*x`).

At any of the input prompts the user may type **`back`** to correct the previous
value, or **`q`** to cancel and return to the menu.

The program then prints a results table containing the **area**, the **minimum**,
**maximum**, **mean** and **RMS** of the function across the interval, the **delta**
(max - min), and the **number of segments** the adaptive method used. If the
function is undefined somewhere on the interval (e.g. a singularity) a warning is
shown; if the function uses an unknown name, an error is shown instead.

### Parameter mode (`/p`)

1. Choose an integration method (same codes as above).
2. Enter the **input filename**. If the file cannot be opened the program re-asks
   rather than returning to the menu.
3. The program reads each line, integrates it, and writes the results to
   **`output.csv`** in the working directory, reporting how many functions were
   processed.

---

## Input and Output

### Interactive input

- **Bounds / tolerance:** a number, or a constant expression that contains no `x`
  (`pi`, `e`, `pi/2`, `2^3`). Tolerance must be an integer in 1-15.
- **Function:** any expression in `x` using the supported syntax below.

#### Supported syntax

| Operators     | `+` `-` `*` `/` `^` (`^` is right-associative)              |
| :------------ | :---------------------------------------------------------- |
| **Functions** | `sin` `cos` `tan` `exp` `ln`/`log` `sqrt` `abs` `pow(x, y)` |
| **Constants** | `pi` / `PI` (π), `e`                                        |
| **Variable**  | `x`                                                         |

Example expressions:

```text
sin(x) + 2*x
x^2 - 3*x + 2
exp(-x) * cos(x)
1 / (1 + x^2)
pow(x, 3) + abs(x)
```

### File input (parameter mode)

Each line of the input CSV is one job in the form `function,a,b`:

```text
sin(x),0,pi
x^2,0,1
exp(-x),0,5
```

A leading header row (or any row whose `a`/`b` are not numbers) is skipped silently.
The bounds may themselves be constant expressions (`0,pi`).

### File output (parameter mode)

`output.csv` always has **10 columns**:

```text
Function,Interval Start,Interval End,Segments,Area,Min Value,Max Value,Mean Value,RMS Value,Delta
```

Rows that cannot be processed keep the 10-column shape and carry a marker in the
second column:

| Marker                    | Meaning                                                        |
| :------------------------ | :------------------------------------------------------------- |
| `INVALID FORMAT - <line>` | The line does not contain two commas.                          |
| `INVALID BOUNDS`          | `a` is not strictly less than `b`.                             |
| `PARSE ERROR`             | The function text could not be parsed.                         |
| `EVAL ERROR`              | The function parsed but calls something unknown (e.g. a typo). |

**Example.** Input:

```text
function,start,end
sin(x),0,pi
2*x,5,1
foo(x),0,1
```

Output:

```text
Function,Interval Start,Interval End,Segments,Area,Min Value,Max Value,Mean Value,RMS Value,Delta
sin(x),0.00,3.14,320,2.0000,0.0000,1.0000,0.6346,0.7060,0.0000
2*x,INVALID BOUNDS,,,,,,,,
foo(x),EVAL ERROR,,,,,,,,
```

---

## Program Structure

This is the technical manual. The source is divided into an `engine/` layer (no
dependency on the CLI) and a `cli/` layer that depends on the engine.

```text
cli-entry.cpp                   program entry point + main loop
gui-entry.cpp                   GUI entry point (wxWidgets)
engine/
  expression/
    tree.{h,cpp}                AST node classes + evaluation
    parser.{h,cpp}              recursive-descent parser
    exception.h                 expression exception hierarchy
  integration/
    integrator.{h,cpp}          IntegrationParameters, method dispatch
    stats.{h,cpp}               IntegrationStats + statistics sampling
    method/
      riemann.h                 shared adaptive Riemann template (left/right/mid)
      right-riemann.cpp         right-edge policy wrapper
      left-riemann.cpp          left-edge policy wrapper
      midpoint-riemann.cpp      midpoint policy wrapper
      trapezoidal.cpp           trapezoidal rule
      simpsons.cpp              Simpson's rule
  csv/
    processor.{h,cpp}           CSV batch processing
cli/
  logic/
    navigation.{h,cpp}          menu flow, step-based back/cancel loop
    execution.{h,cpp}           integration dispatch and CSV execution
  input/input-provider.{h,cpp}  interactive input wizard (back/q/EOF)
  content/                      all user-facing text (messages, banners)
  utils/console.{h,cpp}         output helpers + TextBlock
gui/
  core/                         wxApp subclass
  main/                         main window, menu bar, UI construction
  dialog/                       auth, history, help dialogs
  plotting/                     custom wxPanel integration plot
  auth/                         user store and history persistence
  content/                      GUI string constants
  ui/                           shared widget factory helpers
tests/
  parser.cpp                    parser and expression evaluation tests
  integration.cpp               all five integration methods
  stats.cpp                     IntegrationStats tests
  csv.cpp                       CSV processor tests
  utils.h                       shared test helpers (approxEqual, check)
```

### Data structures

- **Expression tree** (`engine/expression/tree.h`). Two abstract interfaces,
  `Evaluable` (`evaluate`) and `Printable` (`toString`), are combined by the
  abstract base `Node` (which also declares `clone`). Four concrete node types -
  `NumberNode`, `VariableNode`, `OperatorNode`, `FunctionNode` - derive from `Node`.
- **`IntegrationParameters`** (`integrator.h`) - value object holding the bounds and
  tolerance.
- **`IntegrationStats`** (`stats.h`) - holds the area plus min/max/mean/RMS/delta and
  the segment count.
- **`TextBlock`** (`console.h`) - a block of text lines used for banners.
- **Enums** - `IntegrationMethod` (the five rules) and `RowKind` (`OK`/`SKIP`/`INVALID`
  for CSV rows).

### Key algorithms

- **Recursive-descent parsing** (`parser.cpp`). One function per precedence level
  enforces operator precedence and associativity:

  ```text
  parseExpression  →  + and -      (lowest precedence)
  parseTerm        →  * and /
  parseExponent    →  ^            (right-associative)
  parseFactor      →  numbers, variables, functions, parentheses, unary minus
  ```

- **Polymorphic evaluation** (`tree.cpp`). Each node implements `evaluate(double x)`;
  integration just calls `root->evaluate(x)` at many sample points and virtual
  dispatch selects the right node behavior.
- **Adaptive integration** (`method/*.cpp`). Every rule starts with a few segments
  and **doubles** until two successive estimates agree within the tolerance (capped
  to avoid infinite loops on non-converging functions).

### Error handling

The engine reports failures with a small **exception hierarchy**
(`engine/expression/exception.h`): `ExpressionException` (base) →
`ParseException` (bad syntax) and `EvaluationException` (e.g. unknown function).
`parseExpression` throws `ParseException`; `FunctionNode::evaluate` throws
`EvaluationException`. The CLI and CSV layers wrap these in `try`/`catch` and turn
them into user-facing messages or `output.csv` markers. `evaluateConstant` is a
boundary adapter that converts these exceptions into a `bool` for callers that treat
a non-number as normal (e.g. CSV header detection).

### Memory management

The AST is built with `new` and freed with a single `delete root;`. This works
because base-class destructors are `virtual`, so deleting a `Node*` recursively
frees the whole subtree. Each `try`/`catch` site deletes any tree it owns on every
path, so thrown exceptions do not leak.

### C++ techniques used

| Category      | Technique                                   | Where                                                                                                        |
| :------------ | :------------------------------------------ | :----------------------------------------------------------------------------------------------------------- |
| **Mandatory** | Object-oriented design                      | node classes, `IntegrationStats`, etc.                                                                       |
|               | Dynamic memory `new`/`delete`               | AST in `parser.cpp` / `tree.cpp`                                                                             |
|               | Exception handling `try`/`catch`            | `exception.h` + the four catch sites                                                                         |
|               | File management                             | CSV read/write in `execution.cpp` / `processor.cpp`                                                          |
| **Extra**     | Inheritance                                 | `NumberNode`/… derive from `Node`                                                                            |
|               | Polymorphism (abstract + virtual)           | pure-virtual `evaluate`/`toString`/`clone`                                                                   |
|               | Multiple inheritance                        | `Node : public Evaluable, public Printable`                                                                  |
|               | Operator overloading (local **and** global) | local: `IntegrationParameters::operator==`/`!=`; global: `operator<<` for `IntegrationStats` and `TextBlock` |
|               | Function templates                          | `adaptiveRiemannSum<SamplePoint>` in `method/riemann.h`                                                      |
|               | `std::expected` (C++23)                     | `evaluateConstant` return type; `selectIntegrationMethod` in `navigation.cpp`                                |

---

## Examples (optional)

**Interactive - Simpson's rule on `x^2` over [0, 1]:**

```text
Enter your choice: /i
Enter an option in interactive mode: sr
Enter the start of the interval (a): 0
Enter the end of the interval (b): 1
Enter tolerance (integer, e.g., 4 for 0.0001): 4
Enter a function in terms of x (e.g., 'sin(x)+2*x'): x^2
→ Result Area ≈ 0.3333   (analytic value 1/3)
```

**Interactive - recovering from a typo (`back`) and from an unknown function:**

```text
Enter a function in terms of x: foo(x)
Could not evaluate the function: unknown function 'foo'.
```

---

## Testing and Verification

The project has a formal unit test suite under `tests/`, driven by CTest. Tests are
split by concern — one file per engine subsystem — and registered individually so
failures point directly to the broken component.

```bash
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build
```

**Coverage (24 tests, all passing).**

| Suite           | What is tested                                                                           |
| :-------------- | :--------------------------------------------------------------------------------------- |
| `parser.*`      | Constant folding, `pi`/`e`, malformed and empty input, `sin`, negation, unknown function |
| `integration.*` | All five methods on `x²∈[0,1]=1/3`, plus linear, constant, `sin(x)`, degenerate interval |
| `stats.*`       | Constant-function min/max/mean/delta, `isFinite`                                         |
| `csv.*`         | Valid rows, malformed row, empty input, reversed bounds                                  |

**Build verification.** The project compiles cleanly under
`-Wall -Wextra -pedantic` (set in `CMakeLists.txt`) with no warnings.

**Test environment.** macOS (arm64), Apple Clang 21.0.0, CMake 4.3.2.

---

## Improvements and Extensions

**Strong points.** Clear layer separation (engine vs. CLI), an extensible method
list, robust error reporting via exceptions, and no memory leaks.

**Possible extensions.**

- More built-in functions (e.g. `asin`, `sinh`) and user-defined constants.
- Configurable output precision and an option to choose the output filename.
- Richer handling of singularities instead of relying on the adaptive cap.

**Deviations from the Application Form.** None; the project was implemented as originally planned.

---

## Difficulties Encountered

- **Operator precedence and associativity** in the recursive-descent parser - making
  `^` right-associative while `*`/`/` stay left-associative.
- **Manual memory management** across the AST - ensuring every `new` has a matching
  `delete`, including on parser failure paths and when an exception is thrown.
- **Choosing a convergence criterion** for the adaptive methods so they stop at the
  requested tolerance without looping forever on non-converging functions.

---

## Conclusion

The project delivers a working numerical-integration calculator available as both a
command-line tool and a wxWidgets GUI application. It parses arbitrary functions of
`x` into an expression tree, integrates them with five adaptive methods, and reports
both the area and a set of statistics over the interval. It meets all four mandatory
requirements — object-oriented design, dynamic memory management, exception handling,
and file management — and demonstrates six extra-credit techniques: inheritance,
polymorphism, multiple inheritance, operator overloading in both local and global
forms, function templates (`adaptiveRiemannSum`), and `std::expected` for error
propagation without exceptions. A formal CTest suite of 24 tests covers the parser,
all integration methods, statistics, and the CSV processor. The clean separation
between the engine and the CLI/GUI layers kept the program easy to test and extend.

---

## References

_Recommended: IEEE citation style._

- [1] "Recursive descent parser," Wikipedia. <https://en.wikipedia.org/wiki/Recursive_descent_parser>
- [2] "Simpson's rule," Wikipedia. <https://en.wikipedia.org/wiki/Simpson%27s_rule>
- [3] "Riemann sum," Wikipedia. <https://en.wikipedia.org/wiki/Riemann_sum>
- [4] CMake Documentation. <https://cmake.org/documentation/>

### AI-use disclosure

In the interest of academic honesty, parts of this project were developed with the
assistance of an AI tool: the build configuration and the structure of the
recursive-descent parser (including unary-minus and function-call parsing, see the
comments in `engine/expression/parser.cpp`). All AI-assisted code was reviewed by
the author.

---

## Academic context

| Field           | Value                                                    |
| :-------------- | :------------------------------------------------------- |
| Course          | Basics of Programming 2 — BMEVIIIAA03                   |
| University      | Budapest University of Technology and Economics (BME)    |
| Project title   | Integration Calculator                                   |
| Submission date | 2026-05-24                                               |
