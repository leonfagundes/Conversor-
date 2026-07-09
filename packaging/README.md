# Packaging

Packages must be self-contained per operating system. Runtime engines should not be committed to git; release automation should place platform binaries beside the app during packaging.

Expected packaged engine layout:

```text
engines/
  ffmpeg/
  imagemagick/
  libreoffice/
  pandoc/
```

The application should resolve these paths relative to the installed executable or app bundle.

Planned targets:

- Windows: MSI
- Linux: AppImage
- macOS: DMG app bundle

Large engine binaries belong in release artifacts, not source control.
