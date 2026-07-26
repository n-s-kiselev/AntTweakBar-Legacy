Building the examples

From the repository root, build the library first, then the examples:

    gcc nob.c -o nob
    ./nob
    ./nob -examples

Each example is compiled statically against lib/libAntTweakBar.a and the
resulting executables are placed in build/examples/. `./nob -examples` fails
with a clear message if lib/libAntTweakBar.a doesn't exist yet.

External dependencies (examples only; the library itself has none):

- GLFW3 - vendored under vendor/glfw and built from source as part of
  `./nob -examples` on every platform (Linux, macOS, Windows/MinGW). No
  system GLFW3 install needed.
- GLUT - required only by the three GLUT-based examples below.
  - On macOS, the system's built-in (deprecated) GLUT.framework is used
    automatically; no install needed. FreeGLUT itself isn't vendored on
    macOS because upstream's own Cocoa backend is still labeled
    experimental (see docs/plans/vendor-glfw-freeglut.md); the system
    framework is the better-tested option today.
  - On Linux and Windows/MinGW, FreeGLUT is vendored under vendor/freeglut
    and built from source as part of `./nob -examples`. No system
    freeglut install needed. Linux still needs the system X11 development
    headers/libs (`sudo apt install xorg-dev mesa-common-dev` on
    Ubuntu/Debian, or the equivalent on other distros) - `./nob` checks
    for these before building the library itself.

GLAD (OpenGL loader) is vendored under vendor/glad and needs no separate
install.

This AntTweakBar build does not support the OpenGL Core Profile
(TW_OPENGL_CORE crashes) - every example uses the OpenGL compatibility
profile (TW_OPENGL) instead, via either immediate-mode or fixed-function-lit
rendering.

Known limitation - custom cursors on macOS with GLFW: AntTweakBar's cursor
code predates GLFW3 and sets the cursor directly via the native platform API
(NSCursor on macOS), bypassing the windowing toolkit's own cursor tracking.
GLFW's Cocoa backend reasserts its own tracked cursor (the plain arrow, since
these examples never call glfwSetCursor()) on every mouse move, so
AntTweakBar's custom cursor icons (resize handles, etc.) never stay visible
in the GLFW examples on macOS - only the plain system arrow shows. This does
not affect the GLUT examples on macOS, or GLFW/GLUT on Linux (see
docs/plans/vendor-glfw-freeglut.md for the Linux-side fix and the full
root-cause analysis of both platforms).

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
