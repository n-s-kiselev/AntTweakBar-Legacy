//  ---------------------------------------------------------------------------
//
//  @file       TwTriangleGLFW.c
//  @brief      A simple example that uses AntTweakBar with GLFW3 and OpenGL.
//              Ported from TwSimpleDX10.cpp (originally Direct3D10-based).
//              Draws a triangle and allows the user to tweak its vertex
//              positions and colors, using a custom TwDefineStruct'd 2D point.
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              GLFW:        http://www.glfw.org
//
//  ---------------------------------------------------------------------------

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NB_VERTS 3

typedef struct { float X, Y; } Point;

static int g_Angle = 0;
static float g_Scale = 1;
static Point g_Positions[NB_VERTS] = { {0.0f, 0.5f}, {0.5f, -0.5f}, {-0.5f, -0.5f} };
static float g_Colors[NB_VERTS][4] = { {0, 1, 1, 1}, {1, 0, 1, 1}, {1, 1, 0, 1} };
static int g_Width = 640, g_Height = 480;

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
    if (height == 0) height = 1;
    g_Width = width;
    g_Height = height;
    glViewport(0, 0, width, height);
    TwWindowSize(width, height);
}

int main(void)
{
    if (!glfwInit()) {
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    GLFWwindow *window = glfwCreateWindow(g_Width, g_Height, "AntTweakBar + GLFW: triangle", NULL, NULL);
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

    TwAddVarRW(bar, "Rotation", TW_TYPE_INT32, &g_Angle,
               " KeyIncr=r KeyDecr=R Help='Rotates the triangle (angle in degree).' ");
    TwAddVarRW(bar, "Scale", TW_TYPE_FLOAT, &g_Scale,
               " Min=-2 Max=2 Step=0.01 KeyIncr=s KeyDecr=S Help='Scales the triangle (1=original size).' ");

    TwStructMember pointMembers[] = {
        { "X", TW_TYPE_FLOAT, offsetof(Point, X), " Min=-1 Max=1 Step=0.01 " },
        { "Y", TW_TYPE_FLOAT, offsetof(Point, Y), " Min=-1 Max=1 Step=0.01 " }
    };
    TwType pointType = TwDefineStruct("POINT", pointMembers, 2, sizeof(Point), NULL, NULL);

    TwAddVarRW(bar, "Color0", TW_TYPE_COLOR4F, &g_Colors[0], " Alpha HLS Group='Vertex 0' Label=Color ");
    TwAddVarRW(bar, "Pos0", pointType, &g_Positions[0], " Group='Vertex 0' Label='Position' ");
    TwAddVarRW(bar, "Color1", TW_TYPE_COLOR4F, &g_Colors[1], " Alpha HLS Group='Vertex 1' Label=Color ");
    TwAddVarRW(bar, "Pos1", pointType, &g_Positions[1], " Group='Vertex 1' Label='Position' ");
    TwAddVarRW(bar, "Color2", TW_TYPE_COLOR4F, &g_Colors[2], " Alpha HLS Group='Vertex 2' Label=Color ");
    TwAddVarRW(bar, "Pos2", pointType, &g_Positions[2], " Group='Vertex 2' Label='Position' ");

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.125f, 0.125f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float a = (float)g_Angle * (3.14159265358979f / 180.0f);
        float ca = cosf(a), sa = sinf(a);
        float ratio = (float)g_Height / (float)g_Width;

        glBegin(GL_TRIANGLES);
        for (int i = 0; i < NB_VERTS; ++i) {
            float x = g_Scale * (ca * g_Positions[i].X - sa * g_Positions[i].Y) * ratio;
            float y = g_Scale * (sa * g_Positions[i].X + ca * g_Positions[i].Y);
            glColor4fv(g_Colors[i]);
            glVertex2f(x, y);
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
