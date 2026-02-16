# Logistics App

This is a learning project for practicing C++ desktop application development.

## Prerequisites

- C++23 compiler (Clang or GCC)
- CMake 3.16+
- Qt 6 with Widgets and Sql modules

## Tech stack

- C++23
- Qt 6 (Widgets, Sql)
- CMake
- SQLite
- nlohmann/json

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/app
```

macOS app bundle (if generated):

```bash
./build/app.app/Contents/MacOS/app
```
