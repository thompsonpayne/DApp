# AGENTS

This repository is a small Qt (Widgets) C++ application built with CMake.
Use this document as the default operating guide for agentic changes.

## Repository map
- CMake project root: `CMakeLists.txt`
- C++ sources: `src/`
- C++ headers: `src/*.h`
- UI assets: `ui/` (currently `ui/index.html`)
- Resources: `resources/` (`resources/migrations.qrc`, `resources/migrations/*.sql`)
- Build output: `build/` (already present; treat as generated)
- App binary: `build/app` (or `build/app.app/...` on macOS)

## Build, run, lint, test
### Configure and build
- Configure (out-of-source): `cmake -S . -B build`
- Build: `cmake --build build`
- Clean rebuild: delete `build/` and re-run configure/build
- Compile commands DB: `build/compile_commands.json` is enabled by CMake

### Run
- Run (macOS/Linux, likely): `./build/app`
- Run (macOS app bundle if generated): `./build/app.app/Contents/MacOS/app`

### Lint/format
- No lint or format commands are defined in this repo.
- No `.clang-format` or `.editorconfig` was found at the root.
- If you add linting later, prefer `clang-format` + `clang-tidy` wired via CMake.

### Tests
- No tests or `enable_testing()` are configured in `CMakeLists.txt`.
- If tests are added later via CTest:
- Run all tests: `ctest --test-dir build`
- Run a single test: `ctest --test-dir build -R <test-name-or-regex>`

## Dependencies and resources
- Qt: Qt6 Widgets and Qt6 Sql are required.
- JSON: nlohmann/json is pulled via CMake `FetchContent`.
- Migrations: SQL files live in `resources/migrations/*.sql` and are bundled via
  `resources/migrations.qrc`.
- UI HTML: `ui/index.html` is copied into the build directory by CMake.

## Database and migrations
- Database is SQLite (`QSQLITE`). Location is under the OS app data directory
  (`QStandardPaths::AppDataLocation`).
- Migration versions are stored via `PRAGMA user_version`.
- Each migration is applied in a transaction; keep migration files idempotent.
- SQL statements are split on semicolons; avoid stray `;` inside SQL strings.
- When adding a migration:
  - Add a new `resources/migrations/NNN_*.sql` file.
  - Update `resources/migrations.qrc` to include it.
  - Update the migration list in `src/database.cpp`.

## Coding style (C++/Qt)
Follow existing patterns in the `src/` files unless the task requires a change.

### Language and tooling
- C++ standard: C++23 (see `CMakeLists.txt`).
- Prefer Qt types (`QString`, `QDate`, `QWidget`) for UI and Qt APIs.
- Prefer standard library types for data and logic (`std::vector`,
  `std::optional`, `std::string`).
- Avoid mixing Qt and std types unnecessarily; convert at the UI boundary.

### Formatting
- Indentation: 2 spaces.
- Braces: K&R style (`if (...) {` on the same line).
- Lines wrap around ~100 chars; break long argument lists vertically.
- Use `const` and `auto` for locals when the type is obvious.
- Keep UI layout code grouped and vertically aligned for readability.

### Includes and ordering
- In a `.cpp` file, include its matching header first (if it has one).
- Then include Qt headers, then C/C++ standard headers.
- Keep includes minimal; prefer forward declarations in headers.
- Use canonical Qt class names (e.g., `QListWidget`).

### Naming
- Types: `PascalCase` (`OrderDraft`, `OrderFormDialog`).
- Methods and variables: `lowerCamelCase` (`openDialogBtn`, `orderDate`).
- File names: `lower_snake_case` (`main_window.cpp`, `order_form_dialog.h`).
- Constants: prefer `const` variables; avoid all-caps macros.
- UI labels: sentence case strings (e.g., "Create Order").

### Qt patterns
- Prefer parent ownership for widgets to avoid manual deletes.
- Use `QObject::connect` with lambdas for simple handlers.
- Keep UI signal handlers short; delegate complex logic to helpers.
- Avoid blocking the UI thread with long-running work.

### Error handling
- Prefer early returns and simple user-facing validation messages.
- For UI errors, use labels/dialogs rather than `printf`.
- For non-UI logic, prefer return values or `std::optional`.
- When a method exposes `lastErr`, keep it updated on failure paths.

### SQL and data access
- Use prepared statements with bound values (`QSqlQuery::prepare`).
- Keep SQL in raw string literals for readability.
- Store dates as ISO strings (`Qt::ISODate`) and parse consistently.
- Use `std::optional` for queries that can return no rows.

### Logging and diagnostics
- Prefer `qDebug().noquote()` for Qt-facing logs.
- Use `std::println` only for simple console-only output.
- Do not leave verbose logging in hot paths unless requested.

## UI (HTML/JS) style
- `ui/index.html` is plain HTML with inline CSS/JS.
- Keep styles simple and readable; use the existing font stack.
- Favor small functions and early returns in DOM event handlers.
- Avoid introducing build tooling unless required by the task.

## CMake conventions
- Do not modify `build/` or generated `_deps/` content.
- When adding new sources, update `qt_add_executable` target.
- When adding resources, update `resources/migrations.qrc` and target sources.
- Keep `CMAKE_CXX_EXTENSIONS OFF` unless compiler extensions are required.

## External rules
- No Cursor rules found (`.cursor/rules/` or `.cursorrules`).
- No GitHub Copilot rules found (`.github/copilot-instructions.md`).
- If these files appear later, update this section to reflect them.

## Safe operations for agents
- Do not edit files under `build/`.
- Only touch `ui/` if the task explicitly concerns the HTML UI.
- Prefer small, focused changes in `src/` with minimal side effects.
- Avoid reformatting entire files unless necessary for a change.

## If you add new files
- Use `.cpp` for sources and `.h`/`.hpp` for headers.
- Add new sources to `CMakeLists.txt`.
- Keep headers free of heavy UI layout code when possible.
- Update `resources/migrations.qrc` for any new resource files.

## Quick checklist before finishing
- Build still succeeds with `cmake --build build`.
- UI changes are visible without extra tooling.
- Formatting matches the existing style and spacing.
- No changes to generated content under `build/`.
