# COLogger

*[Leer esto en español](README.es.md)*

It's a lightweight, C++20 logging library for Windows, built around a small macro API and a printf-free, type-safe format string system.

COLogger lets an application log to a colorized console, an append-only log file, or both — with log levels, compile-time verbosity stripping, and inline value formatting (including hex) — without pulling in a heavyweight logging framework.

The goal of this project was to understand how the windows console worked and how to build a lightweight logger from scratch.

## Features

- **Console + file sinks**, enabled independently (`InitLogSystem(useConsole, logToFile)`)
- **Colorized console output** via ANSI/VT100 escape codes, one color per log level
- **Five log levels**: `Verbose`, `Log`, `Warning`, `Error`, `Critical`
- **Type-safe inline formatting** — substitute values into a message without `printf`-style format specifiers, with optional hex formatting per argument
- **Compile-time log stripping** — disable verbose/noisy log levels entirely in release builds with a single preprocessor define, at zero runtime cost
- **C ABI export path** for consuming the logger from C, C#, or any language that can call into a Windows DLL

## Quick start

```cpp
#include "OutputLogger.h"

int main()
{
    // Log to both the console and output.log
    InitLogSystem(true, true);

    OLOG_L("Application started");
    OLOG_WF("Player {0} disconnected after {1} seconds", playerName, sessionSeconds);
    OLOG_EF("Failed with error code {#0}", errorCode); // {#0} = print as hex

    OLOG_CLEAR(); // clear the console
}
```

Sample console output:

```
[2026-08-17 14:02:11] Log: Application started
[2026-08-17 14:02:13] Warning: Player Alex disconnected after 42 seconds
[2026-08-17 14:02:15] Error: Failed with error code 0x1a
```

## API reference

| Macro | Description |
|---|---|
| `InitLogSystem(useConsole, logToFile)` | Initializes the global logger instance. Call once at startup. |
| `OLOG(level, message)` | Logs a message at an arbitrary `OLoggerLevel`. |
| `OLOG_V` / `OLOG_L` / `OLOG_W` / `OLOG_E` / `OLOG_C` | Shorthand for `OLOG` at Verbose / Log / Warning / Error / Critical. |
| `OFORMAT(message, ...)` | Builds a formatted string from a template and arguments (see below). Used standalone or inside the `*_F` macros. |
| `OLOG_VF` / `OLOG_LF` / `OLOG_WF` / `OLOG_EF` / `OLOG_CF` | Combines `OLOG` + `OFORMAT` for each level. |
| `OLOG_CLEAR()` | Clears the console. |

### Format string syntax

`OFORMAT` (and the `*_F` log macros) take a message template with `{N}` placeholders, substituted in argument order:

```cpp
OFORMAT("{0} + {1} = {2}", a, b, a + b);
```

Prefix a placeholder's index with `#` to render that argument in hexadecimal instead of decimal:

```cpp
OFORMAT("Pointer: {#0}", (int) address);
```

Supported argument types: `bool`, `std::string`, `char`, `char*`, `byte`, `short`, `int`, `unsigned int`, `long`, `unsigned long`, `long long`, `unsigned long long`, `float`, `double`, `long double`.

### Stripping log levels at compile time

Define one of the following before including `OutputLogger.h` to compile out lower-priority log calls entirely (useful for release builds):

- `ODIS_V` — strips `OLOG_V` / `OLOG_VF`
- `ODIS_VLW` — strips `OLOG_V`, `OLOG_L`, `OLOG_W` and their `*_F` variants

## Building

COLogger targets **Windows** and is built with **CMake** (3.20+, C++20).

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Alternatively, open the repo folder directly in Visual Studio 2022 or Visual Studio 2026. Any other CMake generator (Ninja, etc.) works too, as long as the compiler is MSVC-compatible.

Building produces two targets at once, both named `COLogger`:

- `COLogger_static` — static library
- `COLogger_shared` — shared library (DLL) with the C ABI enabled automatically (`DLL` is defined for this target only)

When built as the top-level project, binaries land in `bin/<Config> - <Platform>/Static/` and `bin/<Config> - <Platform>/Shared/`, with `OutputLogger.h` copied alongside each.

### Consuming from another CMake project

```cmake
add_subdirectory(external/COLogger)
target_link_libraries(MyApp PRIVATE COLogger_static) # or COLogger_shared
```

When included this way (i.e. not the top-level CMake project), COLogger skips redirecting output to `bin/` and copying headers — it just builds the library targets for you to link against.

## Using it from C or C#

Defining the `DLL` preprocessor symbol exposes a flat C ABI at the bottom of `OutputLogger.h`, intended for non-C++ consumers. The `COLogger_shared` CMake target already sets this for you, so building it gives you the exported functions immediately:

```c
void InitLogSys(bool useConsole, bool logToFile);
void Log(int logLevel, char* message);
void ClearConsole();
```

This lets a C or C# project (via P/Invoke) drive the logger without linking against C++ name-mangled symbols. Note that the templated `OFORMAT`/`FormatMsg` API can't cross a DLL boundary, so formatted logging from the C ABI isn't available — build the string on the caller's side before passing it to `Log`.

## Project structure

```
COLogger/
├── CMakeLists.txt            # Top-level CMake config
└── COLogger/
    ├── CMakeLists.txt         # Library targets (static + shared)
    ├── OutputLogger.h         # Public API: OutputLogger class + logging macros
    ├── OutputLogger.cpp       # Implementation
    └── pch.h / pch.cpp        # Precompiled header
```

## Project status

COLogger is an actively evolving personal project. Known gaps and in-progress work:

- There's a known memory allocation issue in the per-message timestamp generation that's still being tracked down.
- No automated test suite yet.
- Bad handling of types when formatting, ends in an abrupt Exception.
- Should have a more abstract representation of a 'sink', so it's more easy to create new ones.
- Add formatting for C ABI enabled libraries.

Contributions, issues, and suggestions are welcome.
