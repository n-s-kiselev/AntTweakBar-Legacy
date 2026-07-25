//  ---------------------------------------------------------------------------
//
//  @file       TwQuadGLFW.c
//  @brief      An example that uses AntTweakBar with GLFW3 and OpenGL.
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
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static float quat[4] = {0, 0, 0, 1};
static float color[] = {0.8f, 1.0f, 0.2f};
static const float FLOAT_PI = 3.14159265f;

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
    TwEventMousePosGLFW((int)x, (int)y);
}

static void windowSizeCallback(GLFWwindow *window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);
}

int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    // No version/profile hints: this AntTweakBar build only supports the
    // OpenGL compatibility profile (TW_OPENGL_CORE crashes), so we let GLFW
    // create its default (non-core) context.
    GLFWwindow *window = glfwCreateWindow(480, 480, "AntTweakBar + GLFW: quad", NULL, NULL);
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

    if (!TwInit(TW_OPENGL, NULL)) {
        fprintf(stderr, "AntTweakBar initialization failed: %s\n", TwGetLastError());
        return 1;
    }

    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        windowSizeCallback(window, width, height);
    }

    TwBar *bar = TwNewBar("TweakBar");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");
    TwAddVarRW(bar, "Rotation", TW_TYPE_QUAT4F, &quat, " opened=true help='Rectangle rotation' ");
    TwAddVarRW(bar, "Color", TW_TYPE_COLOR3F, &color, " opened=true help='Rectangle color' ");

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

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
