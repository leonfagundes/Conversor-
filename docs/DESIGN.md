# Design

## Product Direction

Conversor is a desktop utility for converting files without online converters, advertisements, or upload-based workflows. It is designed for users who want to add files, choose output formats, see quality implications, and convert locally.

The interface should stay compact, classic, and utilitarian. The main window is modeled after direct desktop tools such as Rufus: file input, output destination, grouped conversion options, conversion action, progress, and logs.

The frontend should avoid marketing-style surfaces, oversized spacing, decorative gradients, card-heavy layouts, and saturated accent colors. Controls should feel like a native desktop utility: group boxes, plain buttons, editable-looking fields for paths, tabbed format categories, a grid table, and restrained light/dark palettes.

## Platform

The app targets:

- Windows
- Linux
- macOS

Each distribution package should be self-contained for its operating system.

## Technology

The desktop app uses Qt 6 with C++.

The core conversion planning logic is written in standard C++ and kept independent from Qt. This keeps business rules testable without a GUI runtime and allows the Qt layer to remain thin.

## Architecture

### Core

The core layer handles:

- Extension normalization
- File category detection
- Equivalent extension grouping, such as `.jpg` and `.jpeg`
- Target format discovery by category
- Lossy/lossless quality warnings
- Conversion plan generation

### Desktop App

The Qt Widgets layer handles:

- Adding files and folders
- Displaying the file table
- Showing category tabs for images, audio, video, and documents
- Presenting output format choices
- Showing quality warnings
- Displaying progress and logs
- Keeping the window organized into classic desktop sections: files, destination, conversion options, and status

### Conversion Engines

Planned engines:

- FFmpeg for audio and video
- ImageMagick for images
- LibreOffice headless for office documents and PDF export
- Pandoc for markup-oriented documents

Engine execution is isolated behind adapters so packaging and process handling can evolve without changing the UI.

## Batch Rules

The app accepts multiple files. Files are grouped by category rather than exact extension. Equivalent extensions remain together where appropriate, such as `.jpg` and `.jpeg`.

Each category can have its own output options.

## Quality Rules

The app prioritizes quality by default:

- Avoid recoding when a stream copy is possible.
- Prefer lossless targets for preservation workflows.
- Show warnings for MP3, JPEG, and other lossy targets.
- Avoid promising lossless output when the selected target format cannot support it.
- Warn that some document conversions may alter layout or structure.

## Packaging

Packages should include only the engines needed for the target operating system.

Initial targets:

- Windows: MSI
- Linux: AppImage
- macOS: DMG app bundle
