// glfw_unity.c - single-file compilation of the vendored GLFW 3.4 sources
// under src/, so the examples link against a built-in GLFW instead of
// requiring one to be installed system-wide. Pattern follows raylib's
// rglfw.c (see https://github.com/raysan5/raylib).
//
// Platform backend selection (must be defined by the build before this file
// is compiled - see append_glfw_flags() in nob.c):
//   _GLFW_WIN32   Win32 API (Windows/MinGW)
//   _GLFW_COCOA   Cocoa frameworks (macOS)
//   _GLFW_X11     X Window System (Linux)

#if (defined(__linux__)) && (_POSIX_C_SOURCE < 199309L)
#   undef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 199309L // Required for CLOCK_MONOTONIC without gnu ext.
#endif
#if defined(__linux__) && !defined(_GNU_SOURCE)
#   undef _GNU_SOURCE
#   define _GNU_SOURCE // Required for ppoll() without gnu ext.
#endif

// Common modules shared by every platform
#include "src/init.c"
#include "src/platform.c"
#include "src/context.c"
#include "src/monitor.c"
#include "src/window.c"
#include "src/input.c"
#include "src/vulkan.c"

#if defined(_GLFW_WIN32)
#   include "src/win32_init.c"
#   include "src/win32_module.c"
#   include "src/win32_monitor.c"
#   include "src/win32_window.c"
#   include "src/win32_joystick.c"
#   include "src/win32_time.c"
#   include "src/win32_thread.c"
#   include "src/wgl_context.c"

#   include "src/egl_context.c"
#   include "src/osmesa_context.c"
#endif

#if defined(_GLFW_X11)
#   include "src/posix_module.c"
#   include "src/posix_thread.c"
#   include "src/posix_time.c"
#   include "src/posix_poll.c"
#   include "src/linux_joystick.c"
#   include "src/xkb_unicode.c"

#   include "src/egl_context.c"
#   include "src/osmesa_context.c"

#   include "src/x11_init.c"
#   include "src/x11_monitor.c"
#   include "src/x11_window.c"
#   include "src/glx_context.c"
#endif

#if defined(_GLFW_COCOA)
#   include "src/posix_module.c"
#   include "src/posix_thread.c"
#   include "src/cocoa_time.c"
#   include "src/cocoa_init.m"
#   include "src/cocoa_joystick.m"
#   include "src/cocoa_monitor.m"
#   include "src/cocoa_window.m"
#   include "src/nsgl_context.m"

#   include "src/egl_context.c"
#   include "src/osmesa_context.c"
#endif
