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

`./nob -examples` compiles every example under `examples/` statically against
`lib/libAntTweakBar.a` into `build/examples/`, and fails with a clear message
if `lib/libAntTweakBar.a` doesn't exist yet (i.e. if `./nob` hasn't been run).
GLFW2 and (on Linux/Windows) FreeGLUT are vendored under `vendor/` and built
from source as part of this step, so no external GLFW/GLUT install is
needed; see [`examples/Readme.txt`](examples/Readme.txt) for details,
including macOS's use of the system `GLUT.framework`.

Supported platforms: Linux, macOS, and Windows (MinGW).

## License

See [`License.txt`](License.txt).
