Building the examples

From the repository root, build the library first, then the examples:

    gcc nob.c -o nob
    ./nob
    ./nob -examples

Each example is compiled statically against lib/libAntTweakBar.a and the
resulting executables are placed in build/examples/. `./nob -examples` fails
with a clear message if lib/libAntTweakBar.a doesn't exist yet.

External dependencies (examples only; the library itself has none):

- GLFW 2.7.9 - vendored under vendor/glfw2 and built from source as part of
  `./nob -examples` on every platform (Linux, macOS, Windows/MinGW). No
  system GLFW install needed.
- FreeGLUT - required only by the two GLUT-based examples below, vendored
  under vendor/freeglut and built from source as part of `./nob -examples`
  on Linux, macOS, and Windows/MinGW. No system GLUT/FreeGLUT install is
  needed, and macOS does not use its deprecated system GLUT.framework.
  Linux still needs the system X11 development
    headers/libs (`sudo apt install xorg-dev mesa-common-dev` on
    Ubuntu/Debian, or the equivalent on other distros) - `./nob` checks
    for these before building the library itself.

GLAD (OpenGL loader) is vendored under vendor/glad and needs no separate
install.

This AntTweakBar build does not support the OpenGL Core Profile
(TW_OPENGL_CORE crashes) - every example uses the OpenGL compatibility
profile (TW_OPENGL) instead, via either immediate-mode or fixed-function
rendering with legacy OpenGL lighting.

AntTweakBar's cursor and event integration was originally written for GLFW2,
so every GLFW example uses the local GLFW2 build. A small TwGLFW2.h adapter
centralizes its single-window callback API and Retina framebuffer scaling.
The separate AntTweakBarGLFW3 project contains GLFW3 and the proposed design
for a future toolkit-owned cursor integration. The vendored GLFW2 Cocoa
backend has one narrow patch disabling an obsolete fullscreen-only display
API; no example requests GLFW_FULLSCREEN.

Examples:

- TwSimpleGLUT.c and TwString.cpp (std::string / C-dynamic-string demo) -
  FreeGLUT-based. TwDualGLUT.c is retained as source but is not included in
  the examples build because its dual-window behavior is unreliable.
- TwSimpleGLFW.c, TwAdvanced1.cpp (richest demo: enums, custom structs,
  groups) - GLFW2 + GLAD.
- TwMultiCubesGLFW.c, TwQuadGLFW.c, TwStripGLFW.c, TwTriangleGLFW.c,
  TwSpongeGLFW.cpp (Menger sponge), TwParticlesGLFW.c - GLFW2 + GLAD ports of
  the library's original SDL, DirectX9/10/11, and SFML examples (this
  checkout only targets the OpenGL backend; see
  docs/plans/examples-glfw-port.md for details).
- TwSimpleGLFW2.c - the original GLFW2 reference example.
