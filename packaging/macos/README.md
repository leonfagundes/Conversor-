# macOS Packaging

Target format: DMG containing a signed app bundle.

Expected bundled tools:

- FFmpeg executable set
- ImageMagick CLI tools
- LibreOffice headless runtime, if licensing and bundle size are acceptable
- Pandoc executable
- Qt frameworks and platform plugins required by the build

Engine binaries should live inside the app bundle under `Contents/Resources/engines/`.
