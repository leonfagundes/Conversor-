# Conversor

Conversor is a local desktop file converter focused on privacy, simple batch workflows, and quality-first defaults.

The project targets Windows, Linux, and macOS with a native Qt/C++ desktop interface. Files are processed locally; nothing is uploaded to external services.

## Goals

- Convert audio, video, image, and document files locally.
- Support batch conversion grouped by file category.
- Prefer quality-preserving conversions when technically possible.
- Warn the user when the selected target format necessarily causes quality loss.
- Keep the interface compact and direct, inspired by tools such as Rufus.
- Package conversion engines with the app so the user can download and use it without manual setup.

## Current Status

This repository contains the first project skeleton:

- Pure C++ core for category detection, target format capabilities, and conversion planning.
- CTest-based core tests.
- Qt Widgets desktop shell that displays files, category tabs, output options, warnings, progress, and a conversion log.
- Engine command builders for FFmpeg, ImageMagick, LibreOffice, and Pandoc.
- Packaging notes for embedding FFmpeg, ImageMagick, LibreOffice, and Pandoc per platform.

The desktop app first looks for packaged engines beside the executable, then falls back to executables available on the system `PATH`.

## Requirements

For development:

- CMake 3.24 or newer
- C++20 compiler
- Qt 6 Widgets for the desktop app

The core library and tests do not depend on Qt.

## Build

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

If Qt 6 Widgets is not installed, CMake still builds the core library and tests, but skips the desktop executable.

## Project Layout

```text
src/core/      Pure C++ conversion planning logic
src/app/       Qt Widgets desktop shell
tests/         Core tests
docs/          Product and architecture documentation
packaging/     Platform packaging notes
```

## Privacy

The app is designed to run 100% locally. Conversion engines are expected to be packaged with each platform build, and the application does not require an internet connection to process files.
