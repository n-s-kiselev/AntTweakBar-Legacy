#define NOB_IMPLEMENTATION
#include "vendor/nob/nob.h"

#define SRC_FOLDER           "src/"
#define INCLUDE_FOLDER       "include/"
#define BUILD_FOLDER         "build/"
#define BUILD_STATIC_FOLDER  "build/static/"
#define BUILD_SHARED_FOLDER  "build/shared/"
#define LIB_FOLDER           "lib/"
#define NOB_HEADER           "vendor/nob/nob.h"

#define LIB_STATIC LIB_FOLDER "libAntTweakBar.a"

#if defined(_WIN32)
#define LIB_SHARED LIB_FOLDER "libAntTweakBar.dll"
#define LIB_IMPORT LIB_FOLDER "libAntTweakBar.dll.a"
#elif defined(__APPLE__)
#define LIB_SHARED LIB_FOLDER "libAntTweakBar.dylib"
#else
#define LIB_SHARED        LIB_FOLDER "libAntTweakBar.so"
#define LIB_SHARED_SONAME LIB_FOLDER "libAntTweakBar.so.1"
#define LIB_SHARED_SONAME_NAME "libAntTweakBar.so.1"
#endif

#define EXAMPLES_FOLDER       "examples/"
#define EXAMPLES_BUILD_FOLDER "build/examples/"
#define GLAD_INCLUDE          "vendor/glad/include/"
#define GLAD_SRC              "vendor/glad/src/glad.c"
#define GLAD_OBJ              EXAMPLES_BUILD_FOLDER "glad.o"

// GLFW3 is vendored (unity build, see vendor/glfw/glfw_unity.c) so examples
// need no system GLFW install on any platform.
#define GLFW_INCLUDE          "vendor/glfw/include/"
#define GLFW_SRC              "vendor/glfw/glfw_unity.c"
#define GLFW_OBJ              EXAMPLES_BUILD_FOLDER "glfw.o"

// FreeGLUT is vendored for Linux and Windows (see build_freeglut()); macOS
// keeps using its built-in (deprecated) GLUT.framework instead, since
// upstream FreeGLUT's Cocoa backend is still explicitly labeled experimental.
#define FREEGLUT_INCLUDE      "vendor/freeglut/include/"
#define FREEGLUT_SRC_FOLDER   "vendor/freeglut/src/"
#define FREEGLUT_BUILD_FOLDER EXAMPLES_BUILD_FOLDER "freeglut/"
#define FREEGLUT_LIB          EXAMPLES_BUILD_FOLDER "libfreeglut.a"

#if defined(_WIN32)
#define EXE_EXT ".exe"
#else
#define EXE_EXT ""
#endif

typedef enum {
    EXAMPLE_GLUT, // links against GLUT/freeglut
    EXAMPLE_GLFW, // links against GLFW3 + GLAD
} Example_Kind;

typedef struct {
    const char *source;
    Example_Kind kind;
} Example;

// Examples already ported to modern GLFW3+GLAD are built as-is; the ones
// listed as GLFW ports below replace the original SDL/DirectX9/10/11/SFML
// examples (see docs/plans/examples-glfw-port.md).
static const Example examples[] = {
    { EXAMPLES_FOLDER "TwSimpleGLUT.c",     EXAMPLE_GLUT },
    { EXAMPLES_FOLDER "TwDualGLUT.c",       EXAMPLE_GLUT },
    { EXAMPLES_FOLDER "TwString.cpp",       EXAMPLE_GLUT },
    { EXAMPLES_FOLDER "TwSimpleGLFW.c",     EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwAdvanced1.cpp",    EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwMultiCubesGLFW.c", EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwQuadGLFW.c",       EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwStripGLFW.c",      EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwTriangleGLFW.c",   EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwSpongeGLFW.cpp",   EXAMPLE_GLFW },
    { EXAMPLES_FOLDER "TwParticlesGLFW.c",  EXAMPLE_GLFW },
};

// Vendored FreeGLUT sources (Linux/Windows only - see build_freeglut()).
// Core list mirrors upstream's own non-CMake build (altbuild/Makefile);
// backend lists mirror CMakeLists.txt's WIN32/X11 source lists.
#if !defined(__APPLE__)
#if defined(_WIN32)
#define FREEGLUT_CONFIG_INCLUDE FREEGLUT_SRC_FOLDER "mswin/"
#else
#define FREEGLUT_CONFIG_INCLUDE FREEGLUT_SRC_FOLDER "x11/"
#endif

static const char *freeglut_core_sources[] = {
    FREEGLUT_SRC_FOLDER "fg_callbacks.c",
    FREEGLUT_SRC_FOLDER "fg_cursor.c",
    FREEGLUT_SRC_FOLDER "fg_display.c",
    FREEGLUT_SRC_FOLDER "fg_ext.c",
    FREEGLUT_SRC_FOLDER "fg_font_data.c",
    FREEGLUT_SRC_FOLDER "fg_font.c",
    FREEGLUT_SRC_FOLDER "fg_gamemode.c",
    FREEGLUT_SRC_FOLDER "fg_geometry.c",
    FREEGLUT_SRC_FOLDER "fg_gl2.c",
    FREEGLUT_SRC_FOLDER "fg_init.c",
    FREEGLUT_SRC_FOLDER "fg_input_devices.c",
    FREEGLUT_SRC_FOLDER "fg_joystick.c",
    FREEGLUT_SRC_FOLDER "fg_main.c",
    FREEGLUT_SRC_FOLDER "fg_menu.c",
    FREEGLUT_SRC_FOLDER "fg_misc.c",
    FREEGLUT_SRC_FOLDER "fg_overlay.c",
    FREEGLUT_SRC_FOLDER "fg_spaceball.c",
    FREEGLUT_SRC_FOLDER "fg_state.c",
    FREEGLUT_SRC_FOLDER "fg_stroke_mono_roman.c",
    FREEGLUT_SRC_FOLDER "fg_stroke_roman.c",
    FREEGLUT_SRC_FOLDER "fg_structure.c",
    FREEGLUT_SRC_FOLDER "fg_teapot.c",
    FREEGLUT_SRC_FOLDER "fg_videoresize.c",
    FREEGLUT_SRC_FOLDER "fg_window.c",
};

#if defined(_WIN32)
static const char *freeglut_platform_sources[] = {
    FREEGLUT_SRC_FOLDER "mswin/fg_cursor_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_display_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_ext_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_gamemode_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_init_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_input_devices_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_joystick_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_main_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_menu_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_spaceball_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_state_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_structure_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_window_mswin.c",
    FREEGLUT_SRC_FOLDER "mswin/fg_cmap_mswin.c",
    // Windows has no XParseGeometry() (no Xlib); freeglut supplies its own.
    FREEGLUT_SRC_FOLDER "util/xparsegeometry_repl.c",
};
#else
static const char *freeglut_platform_sources[] = {
    FREEGLUT_SRC_FOLDER "x11/fg_cursor_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_ext_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_gamemode_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_glutfont_definitions_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_init_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_input_devices_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_joystick_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_main_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_menu_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_spaceball_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_state_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_structure_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_window_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_xinput_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_cmap_x11.c",
    FREEGLUT_SRC_FOLDER "x11/fg_display_x11_glx.c",
    FREEGLUT_SRC_FOLDER "x11/fg_state_x11_glx.c",
    FREEGLUT_SRC_FOLDER "x11/fg_window_x11_glx.c",
};
#endif
#endif // !defined(__APPLE__)

// Sources common to every platform: OpenGL backend + windowing-helper stubs
// (GLFW/GLUT/SDL/SFML helpers only need the vendored "Mini*.h" headers, not
// the real libraries, so they build with no extra dependencies).
static const char *common_sources[] = {
    SRC_FOLDER "TwColors.cpp",
    SRC_FOLDER "TwFonts.cpp",
    SRC_FOLDER "TwOpenGL.cpp",
    SRC_FOLDER "TwOpenGLCore.cpp",
    SRC_FOLDER "TwBar.cpp",
    SRC_FOLDER "TwMgr.cpp",
    SRC_FOLDER "TwPrecomp.cpp",
    SRC_FOLDER "LoadOGL.cpp",
    SRC_FOLDER "LoadOGLCore.cpp",
    SRC_FOLDER "TwEventGLFW.c",
    SRC_FOLDER "TwEventGLUT.c",
    SRC_FOLDER "TwEventSDL.c",
    SRC_FOLDER "TwEventSDL12.c",
    SRC_FOLDER "TwEventSDL13.c",
    SRC_FOLDER "TwEventSFML.cpp",
};

#if defined(_WIN32)
static const char *platform_sources[] = { SRC_FOLDER "TwEventWin.c" };
#elif defined(__APPLE__)
static const char *platform_sources[] = { NULL };
#define PLATFORM_SOURCES_COUNT 0
#else
static const char *platform_sources[] = { SRC_FOLDER "TwEventX11.c" };
#endif

#if !defined(PLATFORM_SOURCES_COUNT)
#define PLATFORM_SOURCES_COUNT NOB_ARRAY_LEN(platform_sources)
#endif

static void collect_sources(Nob_File_Paths *sources)
{
    for (size_t i = 0; i < NOB_ARRAY_LEN(common_sources); ++i) {
        nob_da_append(sources, common_sources[i]);
    }
    for (size_t i = 0; i < PLATFORM_SOURCES_COUNT; ++i) {
        nob_da_append(sources, platform_sources[i]);
    }
}

static bool delete_if_exists(const char *path)
{
    if (nob_file_exists(path)) return nob_delete_file(path);
    return true;
}

// Deletes every regular file directly inside folder (not recursive), so a
// stale build output left over from a since-renamed/removed source (e.g. an
// old example binary) doesn't block clean() from removing the folder itself.
static bool clear_directory(const char *folder)
{
    if (!nob_file_exists(folder)) return true;

    Nob_File_Paths children = {0};
    if (!nob_read_entire_dir(folder, &children)) return false;

    bool ok = true;
    for (size_t i = 0; i < children.count; ++i) {
        const char *name = children.items[i];
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        ok = delete_if_exists(nob_temp_sprintf("%s%s", folder, name)) && ok;
    }
    return ok;
}

static bool build_needed(const char *output, const char **inputs, size_t inputs_count)
{
    int result = nob_needs_rebuild(output, inputs, inputs_count);
    if (result < 0) exit(1);
    return result > 0;
}

static bool collect_regular_file(Nob_Walk_Entry entry)
{
    Nob_File_Paths *paths = (Nob_File_Paths *)entry.data;

    if (entry.type == NOB_FILE_REGULAR) {
        nob_da_append(paths, nob_temp_strdup(entry.path));
    }

    return true;
}

static bool collect_tree_files(Nob_File_Paths *paths, const char *root)
{
    return nob_walk_dir(root, collect_regular_file, .data = paths);
}

static void add_common_build_deps(Nob_File_Paths *paths, const char *nob_exe)
{
    nob_da_append(paths, "nob.c");
    nob_da_append(paths, nob_exe);
    nob_da_append(paths, NOB_HEADER);
}

static bool check_linux_desktop_deps(void)
{
#if !defined(_WIN32) && !defined(__APPLE__)
    if (!nob_file_exists("/usr/include/X11/Xlib.h")) {
        nob_log(NOB_ERROR, "Missing X11 development headers: X11/Xlib.h");
        nob_log(NOB_ERROR, "On Ubuntu/Debian install them with:");
        nob_log(NOB_ERROR, "    sudo apt update && sudo apt install xorg-dev mesa-common-dev");
        return false;
    }
#endif
    return true;
}

static bool make_dirs(void)
{
    return nob_mkdir_if_not_exists(BUILD_FOLDER)
        && nob_mkdir_if_not_exists(BUILD_STATIC_FOLDER)
        && nob_mkdir_if_not_exists(BUILD_SHARED_FOLDER)
        && nob_mkdir_if_not_exists(LIB_FOLDER);
}

static const char *object_path(const char *folder, const char *source)
{
    char *base = nob_temp_strdup(nob_path_name(source));
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    return nob_temp_sprintf("%s%s.o", folder, base);
}

static bool is_cpp_source(const char *source)
{
    return nob_sv_ends_with_cstr(nob_sv_from_cstr(source), ".cpp");
}

static const char *compiler_for_source(const char *source)
{
#if defined(__APPLE__)
    // Makefile.osx compiles every source (including the plain .c helpers) as
    // Objective-C++, since TwPrecomp.h pulls in Foundation/AppKit on macOS.
    (void)source;
    return "c++";
#else
    return is_cpp_source(source) ? "c++" : "cc";
#endif
}

static void append_platform_defines(Nob_Cmd *cmd)
{
#if defined(_WIN32)
    nob_cmd_append(cmd, "-D_WINDOWS");
#elif defined(__APPLE__)
    // Objective-C++ mode itself is requested via "-x objective-c++" below,
    // where it's added (not here) since it applies to a subset of callers.
    nob_cmd_append(cmd, "-D_MACOSX");
#else
    nob_cmd_append(cmd, "-D_UNIX");
#endif
}

static bool build_object(const char *source, const char *folder, const char *tw_define,
                          Nob_File_Paths *common_deps)
{
    const char *output = object_path(folder, source);

    Nob_File_Paths inputs = {0};
    nob_da_append(&inputs, source);
    for (size_t i = 0; i < common_deps->count; ++i) {
        nob_da_append(&inputs, common_deps->items[i]);
    }

    if (!build_needed(output, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", output);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, compiler_for_source(source));
    nob_cmd_append(&cmd, "-Wall", "-O2", "-fno-strict-aliasing", "-I" INCLUDE_FOLDER, tw_define);
    append_platform_defines(&cmd);
#if defined(__APPLE__)
    // Every source (including the plain .c helpers) is compiled as Objective-C++;
    // -x makes that explicit instead of relying on a driver/extension mismatch.
    nob_cmd_append(&cmd, "-x", "objective-c++");
#endif
#if !defined(_WIN32) && !defined(__APPLE__)
    if (strcmp(folder, BUILD_SHARED_FOLDER) == 0) nob_cmd_append(&cmd, "-fPIC");
#endif
    nob_cmd_append(&cmd, "-c", source, "-o", output);
    return nob_cmd_run(&cmd);
}

static bool build_static_archive(Nob_File_Paths *objects, const char *nob_exe)
{
    Nob_File_Paths inputs = {0};
    for (size_t i = 0; i < objects->count; ++i) nob_da_append(&inputs, objects->items[i]);
    add_common_build_deps(&inputs, nob_exe);

    if (!build_needed(LIB_STATIC, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", LIB_STATIC);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "ar", "rcs", LIB_STATIC);
    for (size_t i = 0; i < objects->count; ++i) nob_cmd_append(&cmd, objects->items[i]);
    return nob_cmd_run(&cmd);
}

static void append_shared_link_flags(Nob_Cmd *cmd)
{
#if defined(_WIN32)
    nob_cmd_append(cmd, "-shared", "-o", LIB_SHARED, "-Wl,--out-implib," LIB_IMPORT);
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-dynamiclib", "-o", LIB_SHARED);
#else
    nob_cmd_append(cmd, "-shared", "-Wl,-soname," LIB_SHARED_SONAME_NAME, "-o", LIB_SHARED);
#endif
}

static void append_shared_link_libs(Nob_Cmd *cmd)
{
#if defined(_WIN32)
    nob_cmd_append(cmd, "-lopengl32", "-lgdi32");
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "OpenGL", "-framework", "AppKit", "-framework", "Foundation");
#else
    nob_cmd_append(cmd, "-lX11", "-lGL", "-lpthread", "-ldl");
#endif
}

static bool link_shared_library(Nob_File_Paths *objects, const char *nob_exe)
{
    Nob_File_Paths inputs = {0};
    for (size_t i = 0; i < objects->count; ++i) nob_da_append(&inputs, objects->items[i]);
    add_common_build_deps(&inputs, nob_exe);

    if (!build_needed(LIB_SHARED, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", LIB_SHARED);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "c++");
    append_shared_link_flags(&cmd);
    for (size_t i = 0; i < objects->count; ++i) nob_cmd_append(&cmd, objects->items[i]);
    append_shared_link_libs(&cmd);
    if (!nob_cmd_run(&cmd)) return false;

#if !defined(_WIN32) && !defined(__APPLE__)
    if (!delete_if_exists(LIB_SHARED_SONAME)) return false;
    Nob_Cmd ln = {0};
    nob_cmd_append(&ln, "ln", "-sf", nob_path_name(LIB_SHARED), LIB_SHARED_SONAME);
    if (!nob_cmd_run(&ln)) return false;
#endif

    return true;
}

static bool build_all(const char *nob_exe)
{
    if (!check_linux_desktop_deps()) return false;
    if (!make_dirs()) return false;

    Nob_File_Paths sources = {0};
    collect_sources(&sources);

    Nob_File_Paths common_deps = {0};
    if (!collect_tree_files(&common_deps, SRC_FOLDER)) return false;
    if (!collect_tree_files(&common_deps, INCLUDE_FOLDER)) return false;
    add_common_build_deps(&common_deps, nob_exe);

    Nob_File_Paths static_objects = {0};
    Nob_File_Paths shared_objects = {0};

    for (size_t i = 0; i < sources.count; ++i) {
        if (!build_object(sources.items[i], BUILD_STATIC_FOLDER, "-DTW_STATIC", &common_deps)) return false;
        nob_da_append(&static_objects, object_path(BUILD_STATIC_FOLDER, sources.items[i]));

        if (!build_object(sources.items[i], BUILD_SHARED_FOLDER, "-DTW_EXPORTS", &common_deps)) return false;
        nob_da_append(&shared_objects, object_path(BUILD_SHARED_FOLDER, sources.items[i]));
    }

    if (!build_static_archive(&static_objects, nob_exe)) return false;
    if (!link_shared_library(&shared_objects, nob_exe)) return false;

    nob_log(NOB_INFO, "built %s and %s", LIB_STATIC, LIB_SHARED);
    return true;
}

static const char *example_executable_path(const char *source)
{
    char *base = nob_temp_strdup(nob_path_name(source));
    char *dot = strrchr(base, '.');
    if (dot) *dot = '\0';
    return nob_temp_sprintf("%s%s" EXE_EXT, EXAMPLES_BUILD_FOLDER, base);
}

static bool check_examples_deps(void)
{
    if (!nob_file_exists(LIB_STATIC)) {
        nob_log(NOB_ERROR, "%s does not exist yet.", LIB_STATIC);
        nob_log(NOB_ERROR, "Run `./nob` first to build the library, then `./nob -examples`.");
        return false;
    }
    return true;
}

static bool build_glad(const char *nob_exe)
{
    const char *inputs[] = { GLAD_SRC, "nob.c", nob_exe, NOB_HEADER };
    if (!build_needed(GLAD_OBJ, inputs, NOB_ARRAY_LEN(inputs))) {
        nob_log(NOB_INFO, "%s is up to date", GLAD_OBJ);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "cc", "-O2", "-I" GLAD_INCLUDE, "-c", GLAD_SRC, "-o", GLAD_OBJ);
    return nob_cmd_run(&cmd);
}

// Compiles the vendored GLFW3 unity build (vendor/glfw/glfw_unity.c) into a
// single object, following raylib's rglfw.c pattern, so examples link a
// built-in GLFW instead of requiring one to be installed system-wide.
static bool build_glfw(const char *nob_exe)
{
    const char *inputs[] = { GLFW_SRC, "nob.c", nob_exe, NOB_HEADER };
    if (!build_needed(GLFW_OBJ, inputs, NOB_ARRAY_LEN(inputs))) {
        nob_log(NOB_INFO, "%s is up to date", GLFW_OBJ);
        return true;
    }

    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, "cc", "-O2", "-I" GLFW_INCLUDE);
#if defined(_WIN32)
    nob_cmd_append(&cmd, "-D_GLFW_WIN32");
#elif defined(__APPLE__)
    // glfw_unity.c #includes Objective-C (.m) sources under _GLFW_COCOA.
    nob_cmd_append(&cmd, "-D_GLFW_COCOA", "-x", "objective-c");
#else
    nob_cmd_append(&cmd, "-D_GLFW_X11");
#endif
    nob_cmd_append(&cmd, "-c", GLFW_SRC, "-o", GLFW_OBJ);
    return nob_cmd_run(&cmd);
}

#if !defined(__APPLE__)
// Compiles the vendored FreeGLUT sources (see freeglut_core_sources /
// freeglut_platform_sources) into build/examples/freeglut/*.o and archives
// them into FREEGLUT_LIB, mirroring build_object()/build_static_archive()'s
// per-file-object approach for the main library. A single unity build (like
// GLFW's) was deliberately not used here: unlike GLFW, FreeGLUT's upstream
// build systems compile every source as a separate translation unit and
// don't document unity-build safety, so merging ~40 files into one TU is an
// unverified risk not worth taking for a dependency this project cannot
// build-test on every platform in this session.
static bool build_freeglut(const char *nob_exe)
{
    if (!nob_mkdir_if_not_exists(FREEGLUT_BUILD_FOLDER)) return false;

    Nob_File_Paths sources = {0};
    for (size_t i = 0; i < NOB_ARRAY_LEN(freeglut_core_sources); ++i) {
        nob_da_append(&sources, freeglut_core_sources[i]);
    }
    for (size_t i = 0; i < NOB_ARRAY_LEN(freeglut_platform_sources); ++i) {
        nob_da_append(&sources, freeglut_platform_sources[i]);
    }

    Nob_File_Paths objects = {0};
    for (size_t i = 0; i < sources.count; ++i) {
        const char *source = sources.items[i];
        const char *output = object_path(FREEGLUT_BUILD_FOLDER, source);
        const char *inputs[] = { source, "nob.c", nob_exe, NOB_HEADER };

        if (build_needed(output, inputs, NOB_ARRAY_LEN(inputs))) {
            Nob_Cmd cmd = {0};
            nob_cmd_append(&cmd, "cc", "-O2", "-DHAVE_CONFIG_H",
                           "-I" FREEGLUT_CONFIG_INCLUDE, "-I" FREEGLUT_SRC_FOLDER, "-I" FREEGLUT_INCLUDE);
            // Neither FreeGLUT's own sources nor these examples call any
            // GLU function; skip freeglut_std.h's unconditional GL/glu.h
            // include (a separate dev package on many distros) and provide
            // GL/gl.h (already required to build the library itself) instead.
            nob_cmd_append(&cmd, "-DFREEGLUT_NO_GL_INCLUDE", "-include", "GL/gl.h");
#if defined(_WIN32)
            nob_cmd_append(&cmd, "-DFREEGLUT_STATIC");
#endif
            nob_cmd_append(&cmd, "-c", source, "-o", output);
            if (!nob_cmd_run(&cmd)) return false;
        } else {
            nob_log(NOB_INFO, "%s is up to date", output);
        }
        nob_da_append(&objects, output);
    }

    Nob_File_Paths lib_inputs = {0};
    for (size_t i = 0; i < objects.count; ++i) nob_da_append(&lib_inputs, objects.items[i]);
    add_common_build_deps(&lib_inputs, nob_exe);

    if (!build_needed(FREEGLUT_LIB, lib_inputs.items, lib_inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", FREEGLUT_LIB);
        return true;
    }

    Nob_Cmd ar = {0};
    nob_cmd_append(&ar, "ar", "rcs", FREEGLUT_LIB);
    for (size_t i = 0; i < objects.count; ++i) nob_cmd_append(&ar, objects.items[i]);
    return nob_cmd_run(&ar);
}
#endif // !defined(__APPLE__)

static void append_glut_flags(Nob_Cmd *cmd)
{
#if defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "GLUT", "-framework", "OpenGL");
#elif defined(_WIN32)
    nob_cmd_append(cmd, FREEGLUT_LIB, "-lopengl32", "-lgdi32", "-lwinmm", "-luser32");
#else
    // -lX11 -lpthread -ldl: lib/libAntTweakBar.a itself needs these on Linux
    // (e.g. TwMgr.cpp's XCreateBitmapFromData for X11 cursors, LoadOGL.cpp's
    // dlopen-based GL loading) - since examples link the archive statically,
    // these must be listed here too, not just for the vendored FreeGLUT's
    // own X11/extension libs. No -lGLU: neither FreeGLUT nor these examples
    // call any GLU function (see the FREEGLUT_NO_GL_INCLUDE comment above).
    nob_cmd_append(cmd, FREEGLUT_LIB, "-lGL", "-lX11", "-lXrandr", "-lXi", "-lXxf86vm",
                        "-lpthread", "-ldl", "-lm");
#endif
}

static void append_glfw_flags(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd, "-I" GLFW_INCLUDE);
}

static void append_glfw_libs(Nob_Cmd *cmd)
{
    nob_cmd_append(cmd, GLFW_OBJ);
#if defined(_WIN32)
    nob_cmd_append(cmd, "-lopengl32", "-lgdi32");
#elif defined(__APPLE__)
    nob_cmd_append(cmd, "-framework", "Cocoa", "-framework", "IOKit", "-framework", "CoreVideo",
                        "-framework", "OpenGL");
#else
    nob_cmd_append(cmd, "-lGL", "-lX11", "-lXrandr", "-lXi", "-lXxf86vm", "-ldl", "-lpthread");
#endif
}

static bool build_example(const Example *example, const char *nob_exe)
{
    const char *output = example_executable_path(example->source);

    Nob_File_Paths inputs = {0};
    nob_da_append(&inputs, example->source);
    nob_da_append(&inputs, LIB_STATIC);
    nob_da_append(&inputs, GLAD_OBJ);
    if (example->kind == EXAMPLE_GLFW) {
        nob_da_append(&inputs, GLFW_OBJ);
    }
#if !defined(__APPLE__)
    else {
        nob_da_append(&inputs, FREEGLUT_LIB);
    }
#endif
    add_common_build_deps(&inputs, nob_exe);

    if (!build_needed(output, inputs.items, inputs.count)) {
        nob_log(NOB_INFO, "%s is up to date", output);
        return true;
    }

    Nob_Cmd cmd = {0};
    // Always use the C++ driver here (regardless of the example's own
    // source extension): this step both compiles and links against
    // lib/libAntTweakBar.a, which always contains C++ object code (TwBar.cpp,
    // TwMgr.cpp, etc.), so the link needs libstdc++ pulled in automatically -
    // matching link_shared_library()'s own unconditional "c++" choice for the
    // same reason. Using "cc" here (as compiler_for_source() would for a
    // plain .c example) leaves operator new/delete, RTTI, and exception
    // symbols undefined on Linux.
    nob_cmd_append(&cmd, "c++");
    nob_cmd_append(&cmd, "-Wall", "-O2", "-DTW_STATIC", "-I" INCLUDE_FOLDER, "-I" GLAD_INCLUDE);

    if (example->kind == EXAMPLE_GLUT) {
#if defined(__APPLE__)
        // TwSimpleGLUT.c/TwDualGLUT.c/TwString.cpp branch on the library's own
        // _MACOSX macro (not __APPLE__) to pick <GLUT/glut.h> over freeglut.
        nob_cmd_append(&cmd, "-D_MACOSX");
#else
        nob_cmd_append(&cmd, "-I" FREEGLUT_INCLUDE);
        // None of these examples call GLU, so skip freeglut_std.h's
        // GL/glu.h include (see the matching comment in build_freeglut()).
        nob_cmd_append(&cmd, "-DFREEGLUT_NO_GL_INCLUDE");
        // TwSimpleGLUT.c/TwDualGLUT.c already #include <glad/glad.h> (and
        // call gladLoadGL()) before their GLUT header, which fully replaces
        // GL/gl.h and pre-empts its include guard - forcing GL/gl.h here too
        // would make glad.h see the system header "already included" and
        // error out. TwString.cpp has no glad.h of its own, so it still
        // needs GL/gl.h's declarations restored some other way.
        if (strcmp(example->source, EXAMPLES_FOLDER "TwString.cpp") == 0) {
            nob_cmd_append(&cmd, "-include", "GL/gl.h");
        }
#if defined(_WIN32)
        nob_cmd_append(&cmd, "-DFREEGLUT_STATIC");
#endif
#endif
    } else {
        append_glfw_flags(&cmd);
    }

    nob_cmd_append(&cmd, example->source, GLAD_OBJ, LIB_STATIC);
    nob_cmd_append(&cmd, "-o", output);

    if (example->kind == EXAMPLE_GLUT) {
        append_glut_flags(&cmd);
    } else {
        append_glfw_libs(&cmd);
    }

#if defined(__APPLE__)
    // libAntTweakBar.a uses NSCursor/NSImage internally (compiled as
    // Objective-C++); a plain C/C++ example linking it needs the ObjC
    // runtime explicitly since its own translation unit isn't ObjC.
    nob_cmd_append(&cmd, "-framework", "AppKit", "-framework", "Foundation", "-lobjc");
#endif

    return nob_cmd_run(&cmd);
}

static bool build_examples(const char *nob_exe)
{
    if (!check_examples_deps()) return false;
    if (!nob_mkdir_if_not_exists(EXAMPLES_BUILD_FOLDER)) return false;
    if (!build_glad(nob_exe)) return false;
    if (!build_glfw(nob_exe)) return false;
#if !defined(__APPLE__)
    if (!build_freeglut(nob_exe)) return false;
#endif

    for (size_t i = 0; i < NOB_ARRAY_LEN(examples); ++i) {
        if (!build_example(&examples[i], nob_exe)) return false;
    }

    nob_log(NOB_INFO, "built %zu examples into %s", NOB_ARRAY_LEN(examples), EXAMPLES_BUILD_FOLDER);
    return true;
}

static bool clean(void)
{
    bool ok = true;

    ok = delete_if_exists(LIB_STATIC) && ok;
    ok = delete_if_exists(LIB_SHARED) && ok;
#if defined(_WIN32)
    ok = delete_if_exists(LIB_IMPORT) && ok;
#elif !defined(__APPLE__)
    ok = delete_if_exists(LIB_SHARED_SONAME) && ok;
#endif

    // clear_directory() (not just the known current examples/sources) so a
    // stale binary/object left over from a since-renamed or removed
    // example/source doesn't block removing the folder itself.
    ok = clear_directory(FREEGLUT_BUILD_FOLDER) && ok; // nested under EXAMPLES_BUILD_FOLDER
    ok = delete_if_exists(FREEGLUT_BUILD_FOLDER) && ok;
    ok = clear_directory(EXAMPLES_BUILD_FOLDER) && ok;
    ok = delete_if_exists(EXAMPLES_BUILD_FOLDER) && ok;

    ok = clear_directory(BUILD_STATIC_FOLDER) && ok;
    ok = delete_if_exists(BUILD_STATIC_FOLDER) && ok;
    ok = clear_directory(BUILD_SHARED_FOLDER) && ok;
    ok = delete_if_exists(BUILD_SHARED_FOLDER) && ok;
    ok = clear_directory(BUILD_FOLDER) && ok; // e.g. a stray .DS_Store
    ok = delete_if_exists(BUILD_FOLDER) && ok;

    return ok;
}

static void usage(const char *program)
{
    printf("usage: %s [-clean] [-examples] [-help]\n", program);
    printf("  -clean     remove generated build files and exit\n");
    printf("  -examples  build the example programs against lib/libAntTweakBar.a\n");
    printf("             (requires the library to already be built with ./nob)\n");
    printf("  -help      print this help and exit\n");
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF_PLUS(argc, argv, NOB_HEADER);

    const char *nob_exe = argv[0];
    bool clean_requested = false;
    bool examples_requested = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-clean") == 0) {
            clean_requested = true;
        } else if (strcmp(argv[i], "-examples") == 0) {
            examples_requested = true;
        } else if (strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "--help") == 0) {
            usage(argv[0]);
            return 0;
        } else {
            nob_log(NOB_ERROR, "unknown argument: %s", argv[i]);
            usage(argv[0]);
            return 1;
        }
    }

    if (clean_requested) return clean() ? 0 : 1;
    if (examples_requested) return build_examples(nob_exe) ? 0 : 1;
    return build_all(nob_exe) ? 0 : 1;
}
