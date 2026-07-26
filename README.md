# AntTweakBar (Legacy)

[AntTweakBar](https://anttweakbar.sourceforge.io/doc) (**ATB**) is a small and
easy-to-use C/C++ library developed by
[Philippe Decaudin](https://phildec.users.sourceforge.net/) that adds a
lightweight, intuitive GUI to OpenGL-based graphics programs for real-time
parameter tweaking. Representative screenshots can be found, for example, on
the [magnoom](https://github.com/n-s-kiselev/magnoom) page.

This is a maintained checkout of the legacy AntTweakBar development version,
built with a single cross-platform [`nob.c`](https://github.com/tsoding/nob.h)
build script instead of the original per-platform Makefiles/Visual Studio
project. Only the OpenGL backend is targeted (Direct3D9/10/11 and the OpenGL
Core Profile are not supported by this build).

## GLFW and OpenGL compatibility

The examples in this repository are compiled with the vendored GLFW v2
implementation and use AntTweakBar's legacy OpenGL compatibility-profile
backend.

For AntTweakBar with GLFW v3 and the OpenGL Core Profile (OpenGL 3.0 and
later), see [n-s-kiselev/AntTweakBarGLFW3](https://github.com/n-s-kiselev/AntTweakBarGLFW3).

## macOS custom cursors

AntTweakBar's original macOS cursor code packed each custom cursor into a
2-bit grayscale/alpha bitmap. Modern AppKit accepts that bitmap object but
interprets its representation as transparent, so the pointer disappears when
AntTweakBar selects a custom point or rotation cursor. Because the faulty
bitmap is created inside AntTweakBar, the symptom affects both GLFW and
FreeGLUT applications.

This version fixes the shared library code in `CTwMgr::PixmapCursor`: it
converts AntTweakBar's existing 32×32 picture and mask data into an explicit
32-bit RGBA `NSBitmapImageRep`, using the mask as the alpha channel, before
constructing the `NSCursor`. macOS ports based on older AntTweakBar source
should make the same conversion rather than reusing the packed 2-bit image.

## Building

Bootstrap the build tool once, from the repository root:

```sh
gcc nob.c -o nob
```

Then:

```sh
./nob            # build the library (lib/libAntTweakBar.a + the platform dynamic library)
./nob -clean     # remove all generated build output
./nob -examples  # build the example programs (requires ./nob to have run first)
./nob -help      # list all flags
```

`./nob` produces:

- `lib/libAntTweakBar.a` — static library
- `lib/libAntTweakBar.so` (Linux) / `lib/libAntTweakBar.dylib` (macOS) /
  `lib/libAntTweakBar.dll` + `lib/libAntTweakBar.dll.a` (Windows/MinGW) —
  dynamic library

`./nob -examples` compiles the supported examples listed in `nob.c` statically against
`lib/libAntTweakBar.a` into `build/examples/`, and fails with a clear message
if `lib/libAntTweakBar.a` doesn't exist yet (i.e. if `./nob` hasn't been run).
GLFW2 and FreeGLUT are vendored under `vendor/` and built
from source as part of this step, so no external GLFW/GLUT install is
needed on Linux, macOS, or Windows; see
[`examples/Readme.txt`](examples/Readme.txt) for details. The deprecated
macOS system `GLUT.framework` is not used.

Supported platforms: Linux, macOS, and Windows (MinGW).

## License

See [`License.txt`](License.txt).
