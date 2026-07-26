# FreeGLUT Cocoa migration

## Goal

Build the GLUT examples with the vendored FreeGLUT 3.8.0 source on macOS,
Linux, and Windows/MinGW, with no dependency on Apple's deprecated system
`GLUT.framework`. Remove `TwDualGLUT` from the examples built by `nob.c` on
all platforms while retaining its source file.

## Plan

- [x] Confirm the existing vendored FreeGLUT release and upstream Cocoa source
      list.
- [x] Add the matching, unmodified FreeGLUT 3.8.0 Cocoa backend and a small
      platform configuration header.
- [x] Extend `nob.c` to compile and link vendored FreeGLUT on macOS.
- [x] Remove `TwDualGLUT` from the cross-platform example target list.
- [x] Update current build documentation.
- [x] Clean-build the library and all examples, smoke-test affected examples,
      and review the final diff.

## Constraints and decisions

- `nob.c` remains the sole build system.
- Cocoa sources come from the same upstream FreeGLUT v3.8.0 tag as the existing
  vendored core/X11/Win32 files.
- `TwDualGLUT.c` is preserved; only its automatic build is removed.
- Existing uncommitted GLFW HiDPI bar-size work is preserved and validated
  together with this change, but remains a logically separate diff.

## Validation record

- `./nob -clean && ./nob && ./nob -examples` succeeded on macOS (Apple
  Silicon); 11 examples were built and `TwDualGLUT` was absent.
- `TwSimpleGLUT` and `TwString` each stayed running during a three-second
  launch smoke test.
- `otool -L build/examples/TwSimpleGLUT` contains Cocoa/OpenGL/IOKit/CoreVideo
  and no `GLUT.framework` dependency.
- FreeGLUT emits expected macOS OpenGL deprecation warnings; there were no
  new build errors after adding the fallback selected by upstream's
  `XParseGeometry` feature check.
