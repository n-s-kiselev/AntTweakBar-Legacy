//  ---------------------------------------------------------------------------
//
//  @file       TwSpongeGLFW.cpp
//
//  @brief      Example that uses AntTweakBar with GLFW2 and OpenGL. Ported
//              from TwSimpleDX11.cpp+.hlsl (originally Direct3D11-based).
//
//              It draws a Menger sponge, aka Sierpinski cube:
//              http://en.wikipedia.org/wiki/Menger_sponge .
//
//              Cubes shading is augmented with some simple ambient occlusion
//              applied by subdividing each cube face into a 3x3 grid.
//              AntTweakBar is used to add some interactive controls.
//
//              The sponge mesh-generation math below is unchanged from the
//              original Direct3D11 example (it never depended on Direct3D);
//              only the rendering backend (device/buffers) and the
//              window/event loop are ported, from Direct3D11+Win32 to
//              OpenGL (fixed-function pipeline)+GLFW2. This AntTweakBar
//              build does not support the OpenGL Core Profile
//              (TW_OPENGL_CORE crashes), so shading uses fixed-function
//              lighting (GL_LIGHT0) instead of the original's custom vertex
//              shader; the sponge's per-vertex ambient-occlusion colors and
//              normals feed the fixed-function pipeline directly.
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              GLFW:        http://www.glfw.org
//
//  ---------------------------------------------------------------------------

#include <glad/glad.h>
#include "TwGLFW2.h"
#include <AntTweakBar.h>

#include <cmath>
#include <cstdio>
#include <vector>

// ----------------------------------------------------------------------
// Geometry data structures and portable math (unchanged from TwSimpleDX11.cpp)
// ----------------------------------------------------------------------

struct Vector3
{
    float v[3];
    static Vector3 ZERO;
};
Vector3 Vector3::ZERO = { 0, 0, 0 };
struct Matrix4x4
{
    float m[4][4];
    static Matrix4x4 IDENTITY;
};
Matrix4x4 Matrix4x4::IDENTITY = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
struct Quaternion
{
    float q[4];
    static Quaternion IDENTITY;
};
Quaternion Quaternion::IDENTITY = { 0, 0, 0, 1 };
const float FLOAT_PI = 3.14159265f;
struct Vertex
{
    Vector3 Position;
    Vector3 Normal;
    unsigned int AmbientColor; // R,G,B,A bytes at increasing addresses (matches DXGI_FORMAT_R8G8B8A8_UNORM)
};

// Each cube face is split into a 3x3 grid
const int CUBE_FACE_TRIANGLE_COUNT = 2 * 3 * 3; // 18 triangles to be drawn for each face
// Faces color of the sponge wrt to recursion level
const unsigned int COLORS[] = { 0xffffffff, 0xff007fff, 0xff7fff00, 0xffff007f, 0xff0000ff, 0xff00ff00, 0xffff0000 };


// Scene globals
Quaternion g_SpongeRotation;                 // model rotation, set by InitScene
int g_SpongeLevel = 2;                       // number of recursions
bool g_SpongeAO = true;                      // apply ambient occlusion
unsigned int g_SpongeIndicesCount = 0;       // set by BuildSponge
Vector3 g_LightDir = {-0.5f, -0.2f, 1};      // light direction vector
float g_CamDistance = 0.7f;                  // camera distance
float g_BackgroundColor[] = {0, 0, 0.5f, 1}; // background color
bool g_Animate = true;                       // enable animation
float g_AnimationSpeed = 0.2f;               // animation speed


// Some math operators and functions.
Vector3 operator+(const Vector3& a, const Vector3& b)
{
    Vector3 out;
    out.v[0] = a.v[0] + b.v[0];
    out.v[1] = a.v[1] + b.v[1];
    out.v[2] = a.v[2] + b.v[2];
    return out;
}

Vector3 operator*(float s, const Vector3& a)
{
    Vector3 out;
    out.v[0] = s * a.v[0];
    out.v[1] = s * a.v[1];
    out.v[2] = s * a.v[2];
    return out;
}

float Length(const Vector3& a)
{
    return sqrt(a.v[0]*a.v[0] + a.v[1]*a.v[1] + a.v[2]*a.v[2]);
}

Matrix4x4 Translation(const Vector3& t)
{
    Matrix4x4 out(Matrix4x4::IDENTITY);
    out.m[3][0] = t.v[0];
    out.m[3][1] = t.v[1];
    out.m[3][2] = t.v[2];
    return out;
}

Matrix4x4 Scale(float s)
{
    Matrix4x4 out(Matrix4x4::IDENTITY);
    out.m[0][0] = out.m[1][1] = out.m[2][2] = s;
    return out;
}

Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 out(Matrix4x4::IDENTITY);
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            out.m[i][j] = a.m[i][0]*b.m[0][j] + a.m[i][1]*b.m[1][j] + a.m[i][2]*b.m[2][j] + a.m[i][3]*b.m[3][j];
    return out;
}

Vector3 operator*(const Vector3& p, const Matrix4x4& a)
{
    Vector3 out;
    float rw = 1.f / (p.v[0]*a.m[0][3] + p.v[1]*a.m[1][3] + p.v[2]*a.m[2][3] + a.m[3][3]);
    out.v[0] = rw  * (p.v[0]*a.m[0][0] + p.v[1]*a.m[1][0] + p.v[2]*a.m[2][0] + a.m[3][0]);
    out.v[1] = rw  * (p.v[0]*a.m[0][1] + p.v[1]*a.m[1][1] + p.v[2]*a.m[2][1] + a.m[3][1]);
    out.v[2] = rw  * (p.v[0]*a.m[0][2] + p.v[1]*a.m[1][2] + p.v[2]*a.m[2][2] + a.m[3][2]);
    return out;
}

Quaternion RotationFromAxisAngle(const Vector3& axis, float angle)
{
    Quaternion out;
    float norm = Length(axis);
    float sina2 = sin(0.5f * angle);
    out.q[0] = sina2 * axis.v[0] / norm;
    out.q[1] = sina2 * axis.v[1] / norm;
    out.q[2] = sina2 * axis.v[2] / norm;
    out.q[3] = cos(0.5f * angle);
    return out;
}

void AxisAngleFromRotation(Vector3& outAxis, float& outAngle, const Quaternion& quat)
{
    float sina2 = sqrt(quat.q[0]*quat.q[0] + quat.q[1]*quat.q[1] + quat.q[2]*quat.q[2]);
    outAngle = 2.0f * atan2(sina2, quat.q[3]);
    float r = (sina2 > 0) ? (1.0f / sina2) : 0;
    outAxis.v[0] = r * quat.q[0];
    outAxis.v[1] = r * quat.q[1];
    outAxis.v[2] = r * quat.q[2];
}

// Append vertices and indices of a cube to the index and vertex buffers.
// The cube has gradient ambient-occlusion defined per edge.
void AppendCubeToBuffers(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                         const Matrix4x4& xform, float aoRatio, const bool aoEdges[12],
                         const unsigned int faceColors[6])
{
    unsigned int indicesOffset = (unsigned int)vertices.size();

    const float R = 0.5f; // unit cube radius
    const Vector3 A[6] = { {-R, -R, -R}, {+R, -R, -R}, {+R, -R, +R}, {-R, -R, +R}, {-R, +R, -R}, {-R, -R, -R} };
    const Vector3 B[6] = { {+R, -R, -R}, {+R, -R, +R}, {-R, -R, +R}, {-R, -R, -R}, {+R, +R, -R}, {+R, -R, -R} };
    const Vector3 C[6] = { {-R, +R, -R}, {+R, +R, -R}, {+R, +R, +R}, {-R, +R, +R}, {-R, +R, +R}, {-R, -R, +R} };
    const Vector3 D[6] = { {+R, +R, -R}, {+R, +R, +R}, {-R, +R, +R}, {-R, +R, -R}, {+R, +R, +R}, {+R, -R, +R} };
    const Vector3 N[6] = { { 0,  0, -1}, {+1,  0,  0}, { 0,  0, +1}, {-1,  0,  0}, { 0, +1,  0}, { 0, -1,  0} };
    const int E[6][4] = { {0, 1, 2, 3}, {8, 7, 9, 1}, {4, 5, 6, 7}, {11, 3, 10, 5}, {2, 9, 6, 10}, {0, 8, 4, 11} };

    int face, i, j;
    float u, v;
    bool ao;
    Vertex vertex;
    for (face = 0; face < 6; face++)
        for (j = 0; j < 4; j++)
        {
            v = (j == 1) ? aoRatio : ((j == 2) ? 1.0f - aoRatio : j/3);
            for (i = 0; i < 4; i++)
            {
                u = (i == 1) ? aoRatio : ((i == 2) ? 1.0f - aoRatio : i/3);

                vertex.Position = (1.0f - v) * ((1.0f - u) * A[face] + u * B[face])
                                  + v * ((1.0f - u) * C[face] + u * D[face]);
                vertex.Position = vertex.Position * xform;

                vertex.Normal = N[face];

                ao  = (j == 0) && aoEdges[E[face][0]];
                ao |= (i == 3) && aoEdges[E[face][1]];
                ao |= (j == 3) && aoEdges[E[face][2]];
                ao |= (i == 0) && aoEdges[E[face][3]];

                #define DARKEN(r, s) ( (unsigned int)(float(r)*(s)) > 255 ? 255 : (unsigned int)(float(r)*(s)) )
                #define DARKEN_COLOR(c, s) ( 0xff000000 | (DARKEN(((c)>>16)&0xff, s)<<16) | (DARKEN(((c)>>8)&0xff, s)<<8) | DARKEN((c)&0xff, s) )
                vertex.AmbientColor = ao ? DARKEN_COLOR(faceColors[face], 0.75f) : faceColors[face];

                vertices.push_back(vertex);
            }
        }

    const unsigned short I[CUBE_FACE_TRIANGLE_COUNT][3] =
    {
        {0, 5, 4}, {0, 1, 5},  {1, 6, 5}, {1, 2, 6},  {3, 6, 2}, {3, 7, 6},
        {4, 9, 8}, {4, 5, 9},  {5, 10, 9}, {5, 6, 10},  {6, 11, 10}, {6, 7, 11},
        {8, 9, 12}, {9, 13, 12},  {9, 14, 13}, {9, 10, 14},  {10, 15, 14}, {10, 11, 15}
    };
    int tri;
    for (face = 0; face < 6; face++)
        for (tri = 0; tri < CUBE_FACE_TRIANGLE_COUNT; tri++)
            for (i = 0; i < 3; i++)
                indices.push_back(indicesOffset + I[tri][i] + 16*face); // 16 vertices per face
}


// Recursive function called to fill the vertex and index buffers with the cubes forming the Menger sponge.
void FillSpongeBuffers(int level, int levelMax, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                       const Vector3& center, bool aoEnabled, const bool aoEdges[12], const unsigned int faceColors[6])
{
    float scale = pow(1.0f/3.0f, level);

    if (level == levelMax)
    {
        float aoRatio = pow(3.0f, level) * 0.02f;
        if (aoRatio > 0.4999f)
            aoRatio = 0.4999f;
        Matrix4x4 xform = Scale(scale) * Translation(center);
        AppendCubeToBuffers(vertices, indices, xform, aoRatio, aoEdges, faceColors);
    }
    else
    {
        struct Local
        {
            static void ApplyAO(int i, int j, bool& e0, bool& e1, bool& e2, bool& e3)
            {
                if (i == -1 && j == 0) e0 = e1 = true;
                if (i == +1 && j <= 0) e1 = false;
                if (i == +1 && j >= 0) e0 = false;

                if (i == +1 && j == 0) e2 = e3 = true;
                if (i == -1 && j <= 0) e2 = false;
                if (i == -1 && j >= 0) e3 = false;

                if (j == -1 && i == 0) e1 = e2 = true;
                if (j == +1 && i <= 0) e1 = false;
                if (j == +1 && i >= 0) e2 = false;

                if (j == +1 && i == 0) e0 = e3 = true;
                if (j == -1 && i <= 0) e0 = false;
                if (j == -1 && i >= 0) e3 = false;
            }
        };

        bool aoEdgesCopy[12];
        unsigned int faceColorsCopy[6];
        int i, j, k, l;
        for (i = -1; i <= 1; i++)
            for (j = -1; j <= 1; j++)
                for (k = -1; k <= 1; k++)
                    if ( !( (i == 0 && j == 0) || (i == 0 && k == 0) || (j == 0 && k == 0) ) )
                    {
                        float s = 1.0f/3.0f * scale;
                        Vector3 t = { center.v[0] + s * i, center.v[1] + s * j, center.v[2] + s * k };

                        for (l = 0; l < 12; l++)
                            aoEdgesCopy[l] = aoEdges[l];
                        if (aoEnabled)
                        {
                            Local::ApplyAO( i, j, aoEdgesCopy[8], aoEdgesCopy[9], aoEdgesCopy[10], aoEdgesCopy[11]); // z direction
                            Local::ApplyAO( i, k, aoEdgesCopy[1], aoEdgesCopy[7], aoEdgesCopy[5],  aoEdgesCopy[3] ); // y direction
                            Local::ApplyAO(-k, j, aoEdgesCopy[0], aoEdgesCopy[2], aoEdgesCopy[6],  aoEdgesCopy[4] ); // x direction
                        }

                        for (l = 0; l < 6; l++)
                            faceColorsCopy[l] = faceColors[l];
                        if (k == +1) faceColorsCopy[0] = COLORS[level+1];
                        if (i == -1) faceColorsCopy[1] = COLORS[level+1];
                        if (k == -1) faceColorsCopy[2] = COLORS[level+1];
                        if (i == +1) faceColorsCopy[3] = COLORS[level+1];
                        if (j == -1) faceColorsCopy[4] = COLORS[level+1];
                        if (j == +1) faceColorsCopy[5] = COLORS[level+1];

                        FillSpongeBuffers(level + 1, levelMax, vertices, indices, t, aoEnabled, aoEdgesCopy, faceColorsCopy);
                    }
    }
}


// ----------------------------------------------------------------------
// OpenGL rendering (replaces Direct3D11 device/shaders/buffers)
// ----------------------------------------------------------------------

static int g_Width = 640, g_Height = 480;
static std::vector<Vertex> g_Vertices;
static std::vector<unsigned int> g_Indices;

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

static void InitRenderStates()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Mimic the original shader's "(1-lightCoeff) + lightCoeff*NdotL" mix
    // (lightCoeff=0.85) with fixed-function lighting: a small constant
    // ambient term plus a dominant per-vertex diffuse term.
    GLfloat globalAmbient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat lightDiffuse[] = { 0.85f, 0.85f, 0.85f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

// Build sponge vertices/indices (replaces the D3D11 vertex/index buffer creation
// with plain client-side arrays, since this AntTweakBar build doesn't support
// the OpenGL Core Profile buffer/shader pipeline)
bool BuildSponge(int levelMax, bool aoEnabled)
{
    g_Vertices.clear();
    g_Indices.clear();
    bool aoEdges[12] = { false, false, false, false, false, false, false, false, false, false, false, false };
    unsigned int faceColors[6] = { COLORS[0], COLORS[0], COLORS[0], COLORS[0], COLORS[0], COLORS[0] };
    FillSpongeBuffers(0, levelMax, g_Vertices, g_Indices, Vector3::ZERO, aoEnabled, aoEdges, faceColors);

    g_SpongeIndicesCount = (unsigned int)g_Indices.size();
    return true;
}

static void DrawSponge()
{
    if (g_SpongeIndicesCount == 0) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &g_Vertices[0].Position);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), &g_Vertices[0].Normal);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &g_Vertices[0].AmbientColor);

    glDrawElements(GL_TRIANGLES, (GLsizei)g_SpongeIndicesCount, GL_UNSIGNED_INT, g_Indices.data());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// Callback functions called by AntTweakBar to set/get the sponge recursion level and AO flag
void TW_CALL SetSpongeLevelCB(const void *value, void * /*clientData*/)
{
    g_SpongeLevel = *static_cast<const int *>(value);
    BuildSponge(g_SpongeLevel, g_SpongeAO);
}
void TW_CALL GetSpongeLevelCB(void *value, void * /*clientData*/)
{
    *static_cast<int *>(value) = g_SpongeLevel;
}
void TW_CALL SetSpongeAOCB(const void *value, void * /*clientData*/)
{
    g_SpongeAO = *static_cast<const bool *>(value);
    BuildSponge(g_SpongeLevel, g_SpongeAO);
}
void TW_CALL GetSpongeAOCB(void *value, void * /*clientData*/)
{
    *static_cast<bool *>(value) = g_SpongeAO;
}

static void Render()
{
    glClearColor(g_BackgroundColor[0], g_BackgroundColor[1], g_BackgroundColor[2], g_BackgroundColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = (float)g_Width / (float)g_Height;
    float near = 0.1f, far = 100.0f;
    float top = tanf(FLOAT_PI/8.0f) * near; // half of a FLOAT_PI/4 (45 degree) vertical FOV
    float right = top * aspectRatio;
    glFrustum(-right, right, -top, top, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // The original Direct3D projection is left-handed and looks toward +Z.
    // OpenGL's compatibility projection looks toward -Z, so preserve the
    // original horizontal offset but reverse its depth translation.
    float dist = g_CamDistance + 0.4f;
    Vector3 camPosInv = { dist * 0.3f, dist * 0.0f, dist * 2.0f };
    glTranslatef(camPosInv.v[0], camPosInv.v[1], -camPosInv.v[2]);

    // Light direction is fixed in view space (doesn't rotate with the sponge),
    // so it's set before applying the sponge's own rotation below.
    Vector3 lightDirNorm = (1.0f / Length(g_LightDir)) * g_LightDir;
    GLfloat lightPos[4] = { -lightDirNorm.v[0], -lightDirNorm.v[1], -lightDirNorm.v[2], 0.0f }; // directional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    Vector3 axis = Vector3::ZERO;
    float angle = 0;
    AxisAngleFromRotation(axis, angle, g_SpongeRotation);
    glRotatef(angle * 180.0f / FLOAT_PI, axis.v[0], axis.v[1], axis.v[2]);

    DrawSponge();

    TwDraw();
}

// Rotating sponge
static void Anim()
{
    static double s_PrevTime = 0;
    double time = glfwGetTime();
    float dt = (float)(time - s_PrevTime);
    if (g_Animate && dt > 0 && dt < 0.2f)
    {
        Vector3 axis = Vector3::ZERO;
        float angle = 0;
        AxisAngleFromRotation(axis, angle, g_SpongeRotation);
        if (Length(axis) < 1.0e-6f)
            axis.v[1] = 1;
        angle += g_AnimationSpeed * dt;
        if (angle >= 2.0f*FLOAT_PI)
            angle -= 2.0f*FLOAT_PI;
        else if (angle <= 0)
            angle += 2.0f*FLOAT_PI;
        g_SpongeRotation = RotationFromAxisAngle(axis, angle);
    }
    s_PrevTime = time;
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
    glViewport(0, 0, fbWidth, fbHeight);
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
    GLFWwindow *window = glfwCreateWindow(g_Width, g_Height, "AntTweakBar + GLFW: Menger sponge", NULL, NULL);
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
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitRenderStates();

    Vector3 axis = {-1, 1, 0};
    g_SpongeRotation = RotationFromAxisAngle(axis, FLOAT_PI/4);
    BuildSponge(g_SpongeLevel, g_SpongeAO);

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
    tw_glfw2_set_bar_size(bar, 224, 320);
    TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' ");

    TwAddVarCB(bar, "Level", TW_TYPE_INT32, SetSpongeLevelCB, GetSpongeLevelCB, NULL, "min=0 max=3 group=Sponge keyincr=l keydecr=L");
    TwAddVarCB(bar, "Ambient Occlusion", TW_TYPE_BOOLCPP, SetSpongeAOCB, GetSpongeAOCB, NULL, "group=Sponge key=o");
    TwAddVarRW(bar, "Rotation", TW_TYPE_QUAT4F, &g_SpongeRotation, "opened=true axisz=-z group=Sponge");
    TwAddVarRW(bar, "Animation", TW_TYPE_BOOLCPP, &g_Animate, "group=Sponge key=a");
    TwAddVarRW(bar, "Animation speed", TW_TYPE_FLOAT, &g_AnimationSpeed, "min=-10 max=10 step=0.1 group=Sponge keyincr=+ keydecr=-");
    TwAddVarRW(bar, "Light direction", TW_TYPE_DIR3F, &g_LightDir, "opened=true axisz=-z showval=false");
    TwAddVarRW(bar, "Camera distance", TW_TYPE_FLOAT, &g_CamDistance, "min=0 max=4 step=0.01 keyincr=PGUP keydecr=PGDOWN");
    TwAddVarRW(bar, "Background", TW_TYPE_COLOR4F, &g_BackgroundColor, "colormode=hls");

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mousePosCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    while (!glfwWindowShouldClose(window)) {
        Anim();
        Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    TwTerminate();
    glfwTerminate();
    return 0;
}
