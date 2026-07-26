// config.h for the X11/GLX backend (Linux), used with -DHAVE_CONFIG_H.
// Hand-written equivalent of upstream's altbuild/config.h.unix (freeglut has
// no autoconf/CMake step in this build, so the values normally probed by
// those are supplied directly here for a modern glibc/X11 Linux system).

#define HAVE_X11_EXTENSIONS_XF86VMODE_H
#define HAVE_X11_EXTENSIONS_XRANDR_H
#define HAVE_X11_EXTENSIONS_XINPUT2_H

#define FREEGLUT_PRINT_WARNINGS
#define FREEGLUT_PRINT_ERRORS

#define HAVE_SYS_TYPES_H
#define HAVE_UNISTD_H
#define HAVE_SYS_TIME_H
#define HAVE_SYS_PARAM_H
#define HAVE_SYS_IOCTL_H
#define HAVE_FCNTL_H
#define HAVE_ERRNO_H
#define HAVE_GETTIMEOFDAY
#define HAVE_VFPRINTF
#define HAVE_STDINT_H
#define HAVE_INTTYPES_H
#define HAVE_ULONG_LONG

// XParseGeometry is provided by libX11 itself on Linux
#undef NEED_XPARSEGEOMETRY_IMPL
#undef HAVE_U__INT64
