//  ---------------------------------------------------------------------------
//
//  @file       TwQuadGLFW.c
//  @brief      An example that uses AntTweakBar with GLFW2 and OpenGL.
//              Ported from TwGLCoreSDL.c (originally SDL 1.3 + OpenGL Core
//              Profile-based). This AntTweakBar build does not support the
//              OpenGL Core Profile (TW_OPENGL_CORE crashes), so this example
//              draws the same rotating, tweakable-color quad with the fixed-
//              function pipeline instead of a custom shader.
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              GLFW:        http://www.glfw.org
//
//  ---------------------------------------------------------------------------

#include <glad/glad.h>
#include "TwGLFW2.h"
#include <AntTweakBar.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static float quat[4] = {0, 0, 0, 1};
static float color[] = {0.8f, 1.0f, 0.2f};
static const float FLOAT_PI = 3.14159265f;

// AntTweakBar lays out widgets and hit-tests in raw pixel units with no DPI
// awareness. On a Retina/HiDPI display, GLFW's window size (screen
// coordinates, used by mouse callbacks) and framebuffer size (actual
// pixels, used for rendering) differ by the display's content scale. We
// keep TwWindowSize/glViewport in framebuffer-pixel units (matching the
// real render target) and scale mouse coordinates from screen coordinates
// into that same framebuffer-pixel space before forwarding them to
// AntTweakBar.
static double g_MouseScaleX = 1.0, g_MouseScaleY = 1.0;

// Build a column-major 4x4 rotation matrix (as consumed by glMultMatrixf) from a quaternion.
static void quatToMatrix(const float q[4], float m[16])
{
    float x = q[0], y = q[1], z = q[2], w = q[3];
    float xx = x*x, yy = y*y, zz = z*z;
    float xy = x*y, xz = x*z, yz = y*z;
    float wx = w*x, wy = w*y, wz = w*z;

    m[0]  = 1 - 2*(yy+zz); m[1]  = 2*(xy+wz);     m[2]  = 2*(xz-wy);     m[3]  = 0;
    m[4]  = 2*(xy-wz);     m[5]  = 1 - 2*(xx+zz); m[6]  = 2*(yz+wx);     m[7]  = 0;
    m[8]  = 2*(xz+wy);     m[9]  = 2*(yz-wx);     m[10] = 1 - 2*(xx+yy); m[11] = 0;
    m[12] = 0;             m[13] = 0;             m[14] = 0;             m[15] = 1;
}

static void render(void)
{
    float m[16];
    quatToMatrix(quat, m);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(m);

    glColor3fv(color);
    glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(-0.5f,  0.5f, 0);
        glVertex3f(-0.5f, -0.5f, 0);
        glVertex3f( 0.5f,  0.5f, 0);
        glVertex3f( 0.5f, -0.5f, 0);
    glEnd();
}

// Forwards to the library's own GLFW2 key/char translation (TwEventGLFW.c),
// which already tracks GLFW_KEY_LSUPER/RSUPER (Command on macOS) into
// TW_KMOD_META so Command+C/Command+V work there, not just Control+C/V.
// This ignores the (GLFW3-style) mods this shim also reports: TwEventKeyGLFW
// derives modifier state itself from raw key press/release events.
static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode; (void)mods;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { glfwSetWindowShouldClose(window, 1); return; }
    TwEventKeyGLFW(key, action);
}

// TwEventKeyGLFW() above only forwards Ctrl/Cmd-held keys and non-printable
// special keys; plain typed characters (needed e.g. to type into an
// edit-in-place numeric/text field) come through here instead.
static void charCallback(GLFWwindow *window, unsigned int key)
{
    (void)window;
    TwEventCharGLFW((int)key, GLFW_PRESS);
}

static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    (void)window; (void)mods;
    TwEventMouseButtonGLFW(button, action);
}

static void mousePosCallback(GLFWwindow *window, double x, double y)
{
    (void)window;
    TwEventMousePosGLFW((int)(x * g_MouseScaleX), (int)(y * g_MouseScaleY));
}

// Registered as the FRAMEBUFFER size callback (not the window size callback):
// GLFW reports this in actual pixels, matching glViewport/TwWindowSize, and
// firing consistently (unlike mixing window-size and framebuffer-size calls)
// is what keeps the render target and AntTweakBar's own canvas in sync.
static void framebufferSizeCallback(GLFWwindow *window, int fbWidth, int fbHeight)
{
    if (fbWidth == 0) fbWidth = 1;
    if (fbHeight == 0) fbHeight = 1;
    glViewport(0, 0, fbWidth, fbHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (fbWidth >= fbHeight) {
        double aspect = (double)fbWidth / fbHeight;
        glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    } else {
        double aspect = (double)fbHeight / fbWidth;
        glOrtho(-1.0, 1.0, -aspect, aspect, -1.0, 1.0);
    }
    glMatrixMode(GL_MODELVIEW);
    TwWindowSize(fbWidth, fbHeight);

    int winWidth = fbWidth, winHeight = fbHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    g_MouseScaleX = (winWidth > 0) ? (double)fbWidth / winWidth : 1.0;
    g_MouseScaleY = (winHeight > 0) ? (double)fbHeight / winHeight : 1.0;
}

int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    // No version/profile hints: this AntTweakBar build only supports the
    // OpenGL compatibility profile (TW_OPENGL_CORE crashes), so we let GLFW
    // create its default (non-core) context. Retina/HiDPI framebuffers are
    // left enabled (full resolution); see framebufferSizeCallback/
    // mousePosCallback above for how the resulting scale mismatch is handled.
    GLFWwindow *window = glfwCreateWindow(480, 480, "AntTweakBar + GLFW: quad", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Cannot open GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Not gladLoadGLLoader(glfwGetProcAddress): GLFW2's own glfwGetProcAddress
    // (unlike GLFW3's) is a thin wglGetProcAddress() wrapper with no fallback
    // to GetProcAddress() on opengl32.dll, so it fails to resolve OpenGL 1.1
    // core functions on Windows. gladLoadGL() is self-contained and already
    // has that fallback built in (see get_proc() in vendor/glad/src/glad.c).
    if (!gladLoadGL()) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }

    // AntTweakBar draws every widget (buttons, sliders, panel, swatches) at a
    // fixed number of pixels with no DPI awareness, so on a HiDPI/Retina
    // display (where those pixels are physically smaller) the whole bar looks
    // too small compared to a standard display. AntTweakBar's own "fontscaling"
    // global parameter (must be set via TwDefine before TwInit) scales the
    // font metrics that ALL of its widget-layout math derives from (row
    // height, button/slider size, panel size, ...), so scaling it by the
    // window's content scale factor makes the whole bar - not just its text -
    // render at a comparable physical size to a standard display, without
    // touching any library source. On a standard (non-HiDPI) display the
    // content scale is 1.0, so this is a no-op there.
    {
        float xscale = 1.0f, yscale = 1.0f;
        glfwGetWindowContentScale(window, &xscale, &yscale);
        char fontScalingDef[64];
        snprintf(fontScalingDef, sizeof(fontScalingDef), "GLOBAL fontscaling=%g", (double)xscale);
        TwDefine(fontScalingDef);
    }

    if (!TwInit(TW_OPENGL, NULL)) {
        fprintf(stderr, "AntTweakBar initialization failed: %s\n", TwGetLastError());
        return 1;
    }

    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        framebufferSizeCallback(window, fbWidth, fbHeight);
    }

    TwBar *bar = TwNewBar("TweakBar");
    tw_glfw2_set_bar_size(bar, 200, 320);
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");
    TwAddVarRW(bar, "Rotation", TW_TYPE_QUAT4F, &quat, " opened=true help='Rectangle rotation' ");
    TwAddVarRW(bar, "Color", TW_TYPE_COLOR3F, &color, " opened=true help='Rectangle color' ");

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.75f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float angle = (float)glfwGetTime() * 40.0f * (FLOAT_PI / 180.0f);
        quat[0] = quat[1] = 0;
        quat[2] = sinf(angle / 2.0f);
        quat[3] = cosf(angle / 2.0f);

        render();
        TwDraw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TwTerminate();
    glfwTerminate();
    return 0;
}
