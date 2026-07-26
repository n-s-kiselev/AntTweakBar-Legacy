//  ---------------------------------------------------------------------------
//
//  @file       TwMultiCubesGLFW.c
//  @brief      A simple example that uses AntTweakBar with GLFW2 and OpenGL.
//              Ported from TwSimpleSDL.c (originally SDL 1.2-based) to GLFW2.
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

static void setProjection(int width, int height)
{
    float near = 1.0f, far = 10.0f;
    float fovy = 40.0f * 0.01745329251f; // 40 degrees, in radians
    float aspect = (float)width / (float)height;
    float top = tanf(fovy * 0.5f) * near;
    float right = top * aspect;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-right, right, -top, top, near, far);
    glMatrixMode(GL_MODELVIEW);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    int twMod = 0;
    if (mods & GLFW_MOD_SHIFT) twMod |= TW_KMOD_SHIFT;
    if (mods & GLFW_MOD_CONTROL) twMod |= TW_KMOD_CTRL;
    if (mods & GLFW_MOD_ALT) twMod |= TW_KMOD_ALT;

    if (key == GLFW_KEY_ESCAPE) {
        if (TwKeyPressed(TW_KEY_ESCAPE, twMod)) return;
        glfwSetWindowShouldClose(window, 1);
        return;
    }
    if (key >= 0 && key < 128) TwKeyPressed(key, twMod);
}

static void charCallback(GLFWwindow *window, unsigned int key)
{
    (void)window;
    TwKeyPressed((int)key, 0);
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
    if (fbHeight == 0) fbHeight = 1;
    g_Width = fbWidth;
    g_Height = fbHeight;
    setProjection(fbWidth, fbHeight);
    TwWindowSize(fbWidth, fbHeight);

    int winWidth = fbWidth, winHeight = fbHeight;
    glfwGetWindowSize(window, &winWidth, &winHeight);
    g_MouseScaleX = (winWidth > 0) ? (double)fbWidth / winWidth : 1.0;
    g_MouseScaleY = (winHeight > 0) ? (double)fbHeight / winHeight : 1.0;
}

int main(void)
{
    int n, numCubes = 30;
    float color0[] = { 1.0f, 0.5f, 0.0f };
    float color1[] = { 0.5f, 1.0f, 0.0f };
    double ka = 5.3, kb = 1.7, kc = 4.1;
    int quit = 0;

    if (!glfwInit()) {
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    GLFWwindow *window = glfwCreateWindow(g_Width, g_Height, "AntTweakBar + GLFW: multiple cubes", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Cannot open GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

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
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");

    TwAddVarRO(bar, "Width", TW_TYPE_INT32, &g_Width,
               " label='Wnd width' help='Width of the graphics window (in pixels)' ");
    TwAddVarRO(bar, "Height", TW_TYPE_INT32, &g_Height,
               " label='Wnd height' help='Height of the graphics window (in pixels)' ");
    TwAddVarRW(bar, "NumCubes", TW_TYPE_INT32, &numCubes,
               " label='Number of cubes' min=1 max=100 keyIncr=c keyDecr=C help='Defines the number of cubes in the scene.' ");
    TwAddVarRW(bar, "ka", TW_TYPE_DOUBLE, &ka,
               " label='X path coeff' keyIncr=1 keyDecr=CTRL+1 min=-10 max=10 step=0.01 ");
    TwAddVarRW(bar, "kb", TW_TYPE_DOUBLE, &kb,
               " label='Y path coeff' keyIncr=2 keyDecr=CTRL+2 min=-10 max=10 step=0.01 ");
    TwAddVarRW(bar, "kc", TW_TYPE_DOUBLE, &kc,
               " label='Z path coeff' keyIncr=3 keyDecr=CTRL+3 min=-10 max=10 step=0.01 ");
    TwAddVarRW(bar, "color0", TW_TYPE_COLOR3F, &color0,
               " label='Start color' help='Color of the first cube.' ");
    TwAddVarRW(bar, "color1", TW_TYPE_COLOR3F, &color1,
               " label='End color' help='Color of the last cube. Cube colors are interpolated between the Start and End colors.' ");
    TwAddVarRW(bar, "Quit", TW_TYPE_BOOL32, &quit,
               " label='Quit?' true='+' false='-' key='ESC' help='Quit program.' ");

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    while (!quit && !glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.75f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        {
            float near = 1.0f, far = 10.0f;
            float fovy = 40.0f * 0.01745329251f;
            float aspect = (float)g_Width / (float)g_Height;
            float top = tanf(fovy * 0.5f) * near;
            float right = top * aspect;
            glFrustum(-right, right, -top, top, near, far);
        }
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslated(0, 0, -3); // camera at (0,0,3) looking at the origin (gluLookAt equivalent)

        for (n = 0; n < numCubes; ++n) {
            double t = 0.05 * n - glfwGetTime() / 2.0;
            double r = 5.0 * n + glfwGetTime() * 100.0;
            float c = (float)n / numCubes;

            glPushMatrix();
            glTranslated(0.6 * cos(ka * t), 0.6 * cos(kb * t), 0.6 * sin(kc * t));
            glRotated(r, 0.2, 0.7, 0.2);
            glScaled(0.1, 0.1, 0.1);
            glTranslated(-0.5, -0.5, -0.5);

            glColor3f((1.0f - c) * color0[0] + c * color1[0],
                      (1.0f - c) * color0[1] + c * color1[1],
                      (1.0f - c) * color0[2] + c * color1[2]);

            glBegin(GL_QUADS);
                glNormal3f(0,0,-1); glVertex3f(0,0,0); glVertex3f(0,1,0); glVertex3f(1,1,0); glVertex3f(1,0,0);
                glNormal3f(0,0,+1); glVertex3f(0,0,1); glVertex3f(1,0,1); glVertex3f(1,1,1); glVertex3f(0,1,1);
                glNormal3f(-1,0,0); glVertex3f(0,0,0); glVertex3f(0,0,1); glVertex3f(0,1,1); glVertex3f(0,1,0);
                glNormal3f(+1,0,0); glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1);
                glNormal3f(0,-1,0); glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1);
                glNormal3f(0,+1,0); glVertex3f(0,1,0); glVertex3f(0,1,1); glVertex3f(1,1,1); glVertex3f(1,1,0);
            glEnd();

            glPopMatrix();
        }

        TwDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TwTerminate();
    glfwTerminate();
    return 0;
}
