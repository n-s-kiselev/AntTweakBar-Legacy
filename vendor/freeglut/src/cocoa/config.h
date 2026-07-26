// config.h for FreeGLUT's native Cocoa backend, used with -DHAVE_CONFIG_H.
// The values match the feature checks performed by upstream FreeGLUT 3.8.0's
// CMake build on a modern macOS toolchain.

#define FREEGLUT_PRINT_WARNINGS
#define FREEGLUT_PRINT_ERRORS

#define HAVE_SYS_TYPES_H
#define HAVE_UNISTD_H
#define HAVE_SYS_TIME_H
#define HAVE_STDBOOL_H
#define HAVE_SYS_PARAM_H
#define HAVE_SYS_IOCTL_H
#define HAVE_FCNTL_H
#define HAVE_ERRNO_H
#define HAVE_GETTIMEOFDAY
#define HAVE_VFPRINTF
#define HAVE_STDINT_H
#define HAVE_INTTYPES_H
#define HAVE_ULONG_LONG

#define NEED_XPARSEGEOMETRY_IMPL
#undef HAVE_U__INT64
