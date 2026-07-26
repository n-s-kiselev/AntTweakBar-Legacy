//  ---------------------------------------------------------------------------
//
//  @file       TwSimpleGLFW2.c
//  @brief      A simple example that uses AntTweakBar with
//              OpenGL and the GLFW windowing system.
//
//              Adapted, for static linking against the vendored GLFW 2.7.9
//              (vendor/glfw2/), from the original AntTweakBar 1.16 SDK
//              example of the same name. It remains the direct reference
//              implementation for the GLFW2 adapter used by other examples.
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              GLFW:        http://www.glfw.org
//
//  @author     Philippe Decaudin
//  @date       2006/05/20
//
//  ---------------------------------------------------------------------------

// GLFW_NO_GLU: this example never calls a GLU function (see ManualLookAt()
// below, which replaces the original gluLookAt()/gluPerspective() calls) -
// avoids requiring GL/glu.h, which isn't guaranteed to be installed
// (matching how the FreeGLUT examples already avoid the same dependency;
// see FREEGLUT_NO_GL_INCLUDE in nob.c).
#define GLFW_NO_GLU
#include <AntTweakBar.h>
#include <GL/glfw.h>

#include <math.h>
#include <stdio.h>


// Manual replacement for gluLookAt(), so this example doesn't need GLU.
static void ManualLookAt(double eyeX, double eyeY, double eyeZ,
                          double centerX, double centerY, double centerZ,
                          double upX, double upY, double upZ)
{
    double fx = centerX - eyeX, fy = centerY - eyeY, fz = centerZ - eyeZ;
    double flen = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;

    // side = forward x up
    double sx = fy*upZ - fz*upY;
    double sy = fz*upX - fx*upZ;
    double sz = fx*upY - fy*upX;
    double slen = sqrt(sx*sx + sy*sy + sz*sz);
    sx /= slen; sy /= slen; sz /= slen;

    // recompute up = side x forward, so the basis is orthonormal
    double ux = sy*fz - sz*fy;
    double uy = sz*fx - sx*fz;
    double uz = sx*fy - sy*fx;

    GLfloat m[16] = {
        (GLfloat)sx, (GLfloat)ux, (GLfloat)(-fx), 0.0f,
        (GLfloat)sy, (GLfloat)uy, (GLfloat)(-fy), 0.0f,
        (GLfloat)sz, (GLfloat)uz, (GLfloat)(-fz), 0.0f,
        0.0f,        0.0f,        0.0f,           1.0f
    };
    glMultMatrixf(m);
    glTranslated(-eyeX, -eyeY, -eyeZ);
}


// GLFW2 predates HiDPI/Retina awareness: on such displays it reports window
// size and mouse position in points, while the actual OpenGL framebuffer is
// allocated at the native pixel resolution (2x points on a typical Retina
// display) - passing the point-based size straight to glViewport()/
// TwWindowSize() then only fills a quarter of the real window. g_ScaleX/Y
// convert points to pixels; computed once in main() (see ComputeHiDPIScale()
// below) by comparing OpenGL's own default viewport - which already
// reflects the real pixel size - against the point-based window size GLFW2
// reports. On Linux/Windows, where GLFW2 already reports pixels directly,
// this naturally comes out as 1.0, so no platform-specific code is needed.
static double g_ScaleX = 1.0, g_ScaleY = 1.0;

// Must be called once right after glfwOpenWindow(), before this example's
// own glViewport() call ever runs (so OpenGL's default viewport still
// reflects the real framebuffer) and before TwInit() (so AntTweakBar's
// "fontscaling" - see main() - is set before it builds any widget).
static void ComputeHiDPIScale(void)
{
    int width = 1, height = 1;
    glfwGetWindowSize(&width, &height);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    if( width > 0 && height > 0 && vp[2] > 0 && vp[3] > 0 )
    {
        g_ScaleX = (double)vp[2] / width;
        g_ScaleY = (double)vp[3] / height;
    }
}


// Callback function called by GLFW when window size changes
void GLFWCALL WindowSizeCB(int width, int height)
{
    int pixelWidth  = (int)(width*g_ScaleX + 0.5);
    int pixelHeight = (int)(height*g_ScaleY + 0.5);

    // Set OpenGL viewport and camera
    glViewport(0, 0, pixelWidth, pixelHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Manual implementation of gluPerspective(40, (double)width/height, 1, 10)
    {
        double znear = 1, zfar = 10, fovy = 40;
        double top = znear * tan(fovy * (3.14159265358979323846/360.0));
        double aspect = (double)pixelWidth/pixelHeight;
        glFrustum(-top*aspect, top*aspect, -top, top, znear, zfar);
    }
    ManualLookAt(-1,0,3, 0,0,0, 0,1,0);

    // Send the new window size (in pixels) to AntTweakBar
    TwWindowSize(pixelWidth, pixelHeight);
}


// Callback function called by GLFW on mouse motion; scales the point-based
// position GLFW2 reports into the same pixel space as TwWindowSize() above.
void GLFWCALL MousePosCB(int x, int y)
{
    TwEventMousePosGLFW((int)(x*g_ScaleX), (int)(y*g_ScaleY));
}


// This example program draws a possibly transparent cube
void DrawModel(int wireframe)
{
    int pass, numPass;

    // Enable OpenGL transparency and light (could have been done once at init)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);    // use default light diffuse and position
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(3.0);

    if( wireframe )
    {
        glDisable(GL_CULL_FACE);
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        numPass = 1;
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        numPass = 2;
    }

    for( pass=0; pass<numPass; ++pass )
    {
        // Since the material could be transparent, we draw the convex model in 2 passes:
        // first its back faces, and second its front faces.
        glCullFace( (pass==0) ? GL_FRONT : GL_BACK );

        // Draw the model (a cube)
        glBegin(GL_QUADS);
            glNormal3f(0,0,-1); glVertex3f(0,0,0); glVertex3f(0,1,0); glVertex3f(1,1,0); glVertex3f(1,0,0); // front face
            glNormal3f(0,0,+1); glVertex3f(0,0,1); glVertex3f(1,0,1); glVertex3f(1,1,1); glVertex3f(0,1,1); // back face
            glNormal3f(-1,0,0); glVertex3f(0,0,0); glVertex3f(0,0,1); glVertex3f(0,1,1); glVertex3f(0,1,0); // left face
            glNormal3f(+1,0,0); glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1); // right face
            glNormal3f(0,-1,0); glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1); // bottom face
            glNormal3f(0,+1,0); glVertex3f(0,1,0); glVertex3f(0,1,1); glVertex3f(1,1,1); glVertex3f(1,1,0); // top face
        glEnd();
    }
}


// Main
int main()
{
    GLFWvidmode mode;   // GLFW video mode
    TwBar *bar;         // Pointer to a tweak bar

    double time = 0, dt;// Current time and enlapsed time
    double turn = 0;    // Model turn counter
    double speed = 0.3; // Model rotation speed
    int wire = 0;       // Draw model in wireframe?
    float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color
    unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)

    // Intialize GLFW
    if( !glfwInit() )
    {
        // An error occured
        fprintf(stderr, "GLFW initialization failed\n");
        return 1;
    }

    // Create a window
    glfwGetDesktopMode(&mode);
    if( !glfwOpenWindow(640, 480, mode.RedBits, mode.GreenBits, mode.BlueBits,
                        0, 16, 0, GLFW_WINDOW /* or GLFW_FULLSCREEN */) )
    {
        // A fatal error occured
        fprintf(stderr, "Cannot open GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwEnable(GLFW_MOUSE_CURSOR);
    glfwEnable(GLFW_KEY_REPEAT);
    glfwSetWindowTitle("AntTweakBar simple example using GLFW2");

    ComputeHiDPIScale();

    // AntTweakBar draws every widget (buttons, sliders, panel, swatches) at a
    // fixed number of pixels with no DPI awareness, so on a HiDPI/Retina
    // display (where those pixels are physically smaller) the whole bar looks
    // too small compared to a standard display. AntTweakBar's own
    // "fontscaling" global parameter (must be set via TwDefine before
    // TwInit) scales the font metrics that ALL of its widget-layout math
    // derives from (row height, button/slider size, panel size, ...), so
    // scaling it by g_ScaleX (see ComputeHiDPIScale() above - GLFW2 has no
    // content-scale query, so this measured equivalent makes the whole
    // bar render at a comparable physical size to a standard display,
    // without touching any library source. On a standard (non-HiDPI)
    // display g_ScaleX is 1.0, so this is a no-op there.
    {
        char fontScalingDef[64];
        snprintf(fontScalingDef, sizeof(fontScalingDef), "GLOBAL fontscaling=%g", g_ScaleX);
        TwDefine(fontScalingDef);
    }

    // Initialize AntTweakBar
    TwInit(TW_OPENGL, NULL);

    // Create a tweak bar
    bar = TwNewBar("TweakBar");
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW2 and OpenGL.' "); // Message added to the help bar.

    // Add 'speed' to 'bar': it is a modifable (RW) variable of type TW_TYPE_DOUBLE. Its key shortcuts are [s] and [S].
    TwAddVarRW(bar, "speed", TW_TYPE_DOUBLE, &speed,
               " label='Rot speed' min=0 max=2 step=0.01 keyIncr=s keyDecr=S help='Rotation speed (turns/second)' ");

    // Add 'wire' to 'bar': it is a modifable variable of type TW_TYPE_BOOL32 (32 bits boolean). Its key shortcut is [w].
    TwAddVarRW(bar, "wire", TW_TYPE_BOOL32, &wire,
               " label='Wireframe mode' key=w help='Toggle wireframe display mode.' ");

    // Add 'time' to 'bar': it is a read-only (RO) variable of type TW_TYPE_DOUBLE, with 1 precision digit
    TwAddVarRO(bar, "time", TW_TYPE_DOUBLE, &time, " label='Time' precision=1 help='Time (in seconds).' ");

    // Add 'bgColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR3F (3 floats color)
    TwAddVarRW(bar, "bgColor", TW_TYPE_COLOR3F, &bgColor, " label='Background color' ");

    // Add 'cubeColor' to 'bar': it is a modifable variable of type TW_TYPE_COLOR32 (32 bits color) with alpha
    TwAddVarRW(bar, "cubeColor", TW_TYPE_COLOR32, &cubeColor,
               " label='Cube color' alpha help='Color and transparency of the cube.' ");

    // Set GLFW event callbacks
    // - Redirect window size changes to the callback function WindowSizeCB
    glfwSetWindowSizeCallback(WindowSizeCB);
    // - Directly redirect GLFW mouse button events to AntTweakBar
    glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    // - Directly redirect GLFW mouse position events to AntTweakBar
    glfwSetMousePosCallback(MousePosCB);
    // - Directly redirect GLFW mouse wheel events to AntTweakBar
    glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);
    // - Directly redirect GLFW key events to AntTweakBar
    glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);
    // - Directly redirect GLFW char events to AntTweakBar
    glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);


    // Initialize time
    time = glfwGetTime();

    // Main loop (repeated while window is not closed and [ESC] is not pressed)
    while( glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC) )
    {
        // Clear frame buffer using bgColor
        glClearColor(bgColor[0], bgColor[1], bgColor[2], 1);
        glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

        // Rotate model
        dt = glfwGetTime() - time;
        if( dt < 0 ) dt = 0;
        time += dt;
        turn += speed*dt;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotated(360.0*turn, 0.4, 1, 0.2);
        glTranslated(-0.5, -0.5, -0.5);

        // Set color and draw model
        glColor4ubv(cubeColor);
        DrawModel(wire);

        // Draw tweak bars
        TwDraw();

        // Present frame buffer
        glfwSwapBuffers();
    }

    // Terminate AntTweakBar and GLFW
    TwTerminate();
    glfwTerminate();

    return 0;
}
