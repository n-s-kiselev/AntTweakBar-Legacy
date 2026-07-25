Building the examples

From the repository root, build the library first, then the examples:

    gcc nob.c -o nob
    ./nob
    ./nob -examples

Each example is compiled statically against lib/libAntTweakBar.a and the
resulting executables are placed in build/examples/. `./nob -examples` fails
with a clear message if lib/libAntTweakBar.a doesn't exist yet.

External dependencies (examples only; the library itself has none):

- GLFW3 (https://www.glfw.org) - required by every example below except the
  three GLUT-based ones. Install via your platform's package manager
  (e.g. `apt install libglfw3-dev`, `brew install glfw`, or the MSYS2 package
  `mingw-w64-x86_64-glfw` on Windows) or let pkg-config find an existing build.
- GLUT - on macOS, the system's built-in (deprecated) GLUT.framework is used
  automatically, no install needed. On Linux, install freeglut
  (`apt install freeglut3-dev`). On Windows/MinGW, install freeglut via MSYS2
  (`mingw-w64-x86_64-freeglut`).

GLAD (OpenGL loader) is vendored under vendor/glad and needs no separate
install.

This AntTweakBar build does not support the OpenGL Core Profile
(TW_OPENGL_CORE crashes) - every example uses the OpenGL compatibility
profile (TW_OPENGL) instead, via either immediate-mode or fixed-function-lit
rendering.

Examples:

- TwSimpleGLUT.c, TwDualGLUT.c (dual window), TwString.cpp (std::string /
  C-dynamic-string demo) - GLUT-based.
- TwSimpleGLFW.c, TwAdvanced1.cpp (richest demo: enums, custom structs,
  groups) - GLFW3 + GLAD.
- TwMultiCubesGLFW.c, TwQuadGLFW.c, TwStripGLFW.c, TwTriangleGLFW.c,
  TwSpongeGLFW.cpp (Menger sponge), TwParticlesGLFW.c - GLFW3 + GLAD ports of
  the library's original SDL, DirectX9/10/11, and SFML examples (this
  checkout only targets the OpenGL backend; see
  docs/plans/examples-glfw-port.md for details).
