//  ---------------------------------------------------------------------------
//
//  @file       TwParticlesGLFW.c
//  @brief      A simple example that uses AntTweakBar with GLFW2 and OpenGL.
//              Ported from TwSimpleSFML.cpp (originally SFML-based).
//              This example draws moving cubic particles with some
//              interactive control on particles generation.
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

#define MAX_PARTICLES 2000

typedef struct {
    float Size;
    float Position[3];
    float Speed[3];
    float RotationAxis[3];
    float RotationAngle;  // in degrees
    float RotationSpeed;
    float Color[3];
    float Age;
    int Alive;
} Particle;

static Particle g_Particles[MAX_PARTICLES];
static int g_Width = 800, g_Height = 600;

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

static float Random(void)
{
    return 2.0f * ((float)rand() / RAND_MAX) - 1.0f;
}

static void SpawnParticle(Particle *p, float size, const float speedDir[3], float speedNorm, const float color[3])
{
    p->Size = size * (1.0f + 0.2f * Random());
    p->Position[0] = p->Position[1] = p->Position[2] = 0;
    p->Speed[0] = speedNorm * (speedDir[0] + 0.1f * Random());
    p->Speed[1] = speedNorm * (speedDir[1] + 0.1f * Random());
    p->Speed[2] = speedNorm * (speedDir[2] + 0.1f * Random());
    p->RotationAxis[0] = Random();
    p->RotationAxis[1] = Random();
    p->RotationAxis[2] = Random();
    p->RotationAngle = 360.0f * Random();
    p->RotationSpeed = 360.0f * Random();
    p->Color[0] = color[0] + 0.2f * Random();
    p->Color[1] = color[1] + 0.2f * Random();
    p->Color[2] = color[2] + 0.2f * Random();
    p->Age = 0;
    p->Alive = 1;
}

static void UpdateParticle(Particle *p, float dt)
{
    p->Position[0] += dt * p->Speed[0];
    p->Position[1] += dt * p->Speed[1];
    p->Position[2] += dt * p->Speed[2];
    p->Speed[1] -= dt * 9.81f; // gravity
    p->RotationAngle += dt * p->RotationSpeed;
    p->Age += dt;
}

static void setProjection(int width, int height)
{
    float near = 1.0f, far = 500.0f;
    float fovy = 90.0f * 0.01745329251f;
    float aspect = (float)width / (float)height;
    float top = tanf(fovy * 0.5f) * near;
    float right = top * aspect;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-right, right, -top, top, near, far);
    glMatrixMode(GL_MODELVIEW);
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

static void mouseScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    (void)window; (void)xoffset;
    static double pos = 0;
    pos += yoffset;
    TwEventMouseWheelGLFW((int)pos);
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
    float birthCount = 0;
    float birthRate = 20;               // number of particles generated per second
    float maxAge = 3.0f;                // particles life time
    float speedDir[3] = {0, 1, 0};      // initial particles speed direction
    float speedNorm = 7.0f;             // initial particles speed amplitude
    float size = 0.1f;                  // particles size
    float color[3] = {0.8f, 0.6f, 0};   // particles color
    float bgColor[3] = {0, 0.6f, 0.6f}; // background color

    if (!glfwInit()) {
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    GLFWwindow *window = glfwCreateWindow(g_Width, g_Height, "AntTweakBar + GLFW: particles", NULL, NULL);
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

    TwBar *bar = TwNewBar("Particles");
    tw_glfw2_set_bar_size(bar, 200, 320);
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");
    int barPos[2] = {16, 240};
    TwSetParam(bar, NULL, "position", TW_PARAM_INT32, 2, barPos);

    TwAddVarRW(bar, "Birth rate", TW_TYPE_FLOAT, &birthRate, " min=0.1 max=100 step=0.1 keyIncr='+' keyDecr='-' ");
    TwAddVarRW(bar, "Speed", TW_TYPE_FLOAT, &speedNorm, " min=0.1 max=10 step=0.1 keyIncr='s' keyDecr='S' ");
    TwAddVarRW(bar, "Direction", TW_TYPE_DIR3F, &speedDir, " opened=true showval=false ");
    TwAddVarRW(bar, "Color", TW_TYPE_COLOR3F, &color, " colorMode=hls opened=true ");
    TwAddVarRW(bar, "Background color", TW_TYPE_COLOR3F, &bgColor, " colorMode=hls opened=true ");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

    {
        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        framebufferSizeCallback(window, fbWidth, fbHeight);
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCharCallback(window, charCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    double time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = (float)(now - time);
        if (dt < 0) dt = 0;
        time = now;

        for (int i = 0; i < MAX_PARTICLES; ++i) {
            if (!g_Particles[i].Alive) continue;
            UpdateParticle(&g_Particles[i], dt);
            if (g_Particles[i].Age >= maxAge) g_Particles[i].Alive = 0;
        }

        birthCount += dt * birthRate;
        while (birthCount >= 1.0f) {
            for (int i = 0; i < MAX_PARTICLES; ++i) {
                if (!g_Particles[i].Alive) {
                    SpawnParticle(&g_Particles[i], size, speedDir, speedNorm, color);
                    break;
                }
            }
            birthCount -= 1.0f;
        }

        glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < MAX_PARTICLES; ++i) {
            if (!g_Particles[i].Alive) continue;
            Particle *p = &g_Particles[i];

            glColor3fv(p->Color);
            glLoadIdentity();
            glTranslatef(0.0f, -1.0f, -3.0f); // camera position
            glTranslatef(p->Position[0], p->Position[1], p->Position[2]);
            glScalef(p->Size, p->Size, p->Size);
            glRotatef(p->RotationAngle, p->RotationAxis[0], p->RotationAxis[1], p->RotationAxis[2]);

            glBegin(GL_QUADS);
                glNormal3f(0,0,-1); glVertex3f(0,0,0); glVertex3f(0,1,0); glVertex3f(1,1,0); glVertex3f(1,0,0);
                glNormal3f(0,0,+1); glVertex3f(0,0,1); glVertex3f(1,0,1); glVertex3f(1,1,1); glVertex3f(0,1,1);
                glNormal3f(-1,0,0); glVertex3f(0,0,0); glVertex3f(0,0,1); glVertex3f(0,1,1); glVertex3f(0,1,0);
                glNormal3f(+1,0,0); glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1);
                glNormal3f(0,-1,0); glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1);
                glNormal3f(0,+1,0); glVertex3f(0,1,0); glVertex3f(0,1,1); glVertex3f(1,1,1); glVertex3f(1,1,0);
            glEnd();
        }

        TwDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TwTerminate();
    glfwTerminate();
    return 0;
}
