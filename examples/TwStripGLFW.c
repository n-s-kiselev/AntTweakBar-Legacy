//  ---------------------------------------------------------------------------
//
//  @file       TwStripGLFW.c
//  @brief      A simple example that uses AntTweakBar with GLFW2 and OpenGL.
//              Ported from TwSimpleDX9.cpp (originally Direct3D9-based).
//              Draws an animated color-gradient triangle strip.
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

static int g_Width = 640, g_Height = 480;

// AntTweakBar lays out widgets and hit-tests in raw pixel units with no DPI
// awareness. On a Retina/HiDPI display, GLFW's window size (screen
// coordinates, used by mouse callbacks) and framebuffer size (actual
// pixels, used for rendering) differ by the display's content scale. We
// keep TwWindowSize/glViewport in framebuffer-pixel units (matching the
// real render target) and scale mouse coordinates from screen coordinates
// into that same framebuffer-pixel space before forwarding them to
// AntTweakBar. On a standard (non-HiDPI) display framebuffer size equals
// window size, so this scale is exactly 1.0 and everything behaves as before.
static double g_MouseScaleX = 1.0, g_MouseScaleY = 1.0;

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    int twMod = 0;
    if (mods & GLFW_MOD_SHIFT) twMod |= TW_KMOD_SHIFT;
    if (mods & GLFW_MOD_CONTROL) twMod |= TW_KMOD_CTRL;
    if (mods & GLFW_MOD_ALT) twMod |= TW_KMOD_ALT;
    if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, 1);
    else if (key >= 0 && key < 128) TwKeyPressed(key, twMod);
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
    g_Width = fbWidth;
    g_Height = fbHeight;
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
    int numSec = 100;             // number of strip sections
    float color[] = { 1, 0, 0 };  // strip color
    unsigned char bgColor[] = { 128, 196, 196, 255 }; // background color (32bits RGBA: R,G,B,A)

    if (!glfwInit()) {
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    GLFWwindow *window = glfwCreateWindow(g_Width, g_Height, "AntTweakBar + GLFW: gradient strip", NULL, NULL);
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
    TwDefine(" TweakBar color='128 224 160' text=dark ");

    TwAddVarRW(bar, "NumSec", TW_TYPE_INT32, &numSec,
               " label='Strip length' min=1 max=1000 keyIncr=s keyDecr=S help='Number of segments of the strip.' ");
    TwAddVarRW(bar, "Color", TW_TYPE_COLOR3F, &color, " label='Strip color' ");
    TwAddVarRW(bar, "BgColor", TW_TYPE_COLOR32, &bgColor, " label='Background color' ");
    TwAddVarRO(bar, "Width", TW_TYPE_INT32, &g_Width, " label='wnd width' help='Current graphics window width.' ");
    TwAddVarRO(bar, "Height", TW_TYPE_INT32, &g_Height, " label='wnd height' help='Current graphics window height.' ");

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(bgColor[2] / 255.0f, bgColor[1] / 255.0f, bgColor[0] / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float t = (float)glfwGetTime();
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= numSec; ++i) {
            float s = (float)i / 100.0f;
            float x0 = 0.05f + 0.7f * cosf(2.0f * s + 5.0f * t);
            float x1 = x0 + (0.25f + 0.1f * cosf(s + t));
            float y = 0.7f * (0.7f + 0.3f * sinf(s + t)) * sinf(1.5f * s + 3.0f * t);
            float sc = (float)i / numSec;

            glColor3f(color[0] * sc, color[1] * sc, color[2] * sc);
            glVertex2f(x0, y);
            glVertex2f(x1, y);
        }
        glEnd();

        TwDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TwTerminate();
    glfwTerminate();
    return 0;
}
