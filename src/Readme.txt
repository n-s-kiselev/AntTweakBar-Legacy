To build the library (static and dynamic, OpenGL backend) on Linux, macOS, or
Windows (MinGW), from the repository root:

    gcc nob.c -o nob
    ./nob -clean
    ./nob

This produces lib/libAntTweakBar.a plus the platform dynamic library
(libAntTweakBar.so on Linux, libAntTweakBar.dylib on macOS,
libAntTweakBar.dll + libAntTweakBar.dll.a on Windows).

Direct3D9/10/11 backends are not built; this checkout only targets OpenGL.
