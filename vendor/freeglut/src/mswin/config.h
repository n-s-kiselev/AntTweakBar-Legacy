// config.h for the Win32 backend (Windows/MinGW), used with -DHAVE_CONFIG_H.
// Hand-written equivalent of upstream's altbuild/config.h.msvc6, adjusted for
// MinGW-w64's GCC toolchain (which, unlike MSVC6, does provide unistd.h,
// sys/time.h, stdint.h, and a real "long long" type).

#define FREEGLUT_PRINT_WARNINGS
#define FREEGLUT_PRINT_ERRORS

#define HAVE_SYS_TYPES_H
#define HAVE_UNISTD_H
#define HAVE_SYS_TIME_H
#define HAVE_FCNTL_H
#define HAVE_ERRNO_H
#define HAVE_GETTIMEOFDAY
#define HAVE_VFPRINTF
#define HAVE_STDINT_H
#define HAVE_INTTYPES_H
#define HAVE_ULONG_LONG

// Windows has no XParseGeometry() (no Xlib); freeglut's own replacement
// implementation (src/util/xparsegeometry_repl.c) is compiled in instead.
#define NEED_XPARSEGEOMETRY_IMPL
#undef HAVE_U__INT64
