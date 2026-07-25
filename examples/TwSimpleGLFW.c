//  ---------------------------------------------------------------------------
//
//  @file       TwSimpleGLFW.c
//  @brief      A simple example that uses AntTweakBar with 
//              OpenGL and the GLFW windowing system.
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              GLFW:        http://www.glfw.org
//  
//  @author     Philippe Decaudin
//  @date       2006/05/20
//
//  ---------------------------------------------------------------------------

#include <glad/glad.h>
// #include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

float g_cameraPosX = 0.0f;
float g_cameraPosY = 0.0f;
float g_cameraPosZ = 5.0f;

bool g_cameraDragging = false;

double g_lastMouseX = 0.0;
double g_lastMouseY = 0.0;

// char g_userText[256] = "Hello AntTweakBar!\n";
char *g_userText = NULL; // Will be malloc'ed on first use

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS || action == GLFW_REPEAT)
  {
    int twMod = 0;
    bool ctrl;
    if (mods & GLFW_MOD_SHIFT) twMod |= TW_KMOD_SHIFT;
    if ((ctrl = (mods & GLFW_MOD_CONTROL))) twMod |= TW_KMOD_CTRL;
    if (mods & GLFW_MOD_ALT) twMod |= TW_KMOD_ALT;

    int twKey = 0;
    switch (key)
    {
    case GLFW_KEY_BACKSPACE: twKey = TW_KEY_BACKSPACE; break;
    case GLFW_KEY_TAB: twKey = TW_KEY_TAB; break;
    //case GLFW_KEY_???: twKey = TW_KEY_CLEAR; break;
    case GLFW_KEY_ENTER: twKey = TW_KEY_RETURN; break;
    case GLFW_KEY_PAUSE: twKey = TW_KEY_PAUSE; break;
    case GLFW_KEY_ESCAPE: twKey = TW_KEY_ESCAPE; break;
    case GLFW_KEY_SPACE: twKey = TW_KEY_SPACE; break;
    case GLFW_KEY_DELETE: twKey = TW_KEY_DELETE; break;
    case GLFW_KEY_UP: twKey = TW_KEY_UP; break;
    case GLFW_KEY_DOWN: twKey = TW_KEY_DOWN; break;
    case GLFW_KEY_RIGHT: twKey = TW_KEY_RIGHT; break;
    case GLFW_KEY_LEFT: twKey = TW_KEY_LEFT; break;
    case GLFW_KEY_INSERT: twKey = TW_KEY_INSERT; break;
    case GLFW_KEY_HOME: twKey = TW_KEY_HOME; break;
    case GLFW_KEY_END: twKey = TW_KEY_END; break;
    case GLFW_KEY_PAGE_UP: twKey = TW_KEY_PAGE_UP; break;
    case GLFW_KEY_PAGE_DOWN: twKey = TW_KEY_PAGE_DOWN; break;
    case GLFW_KEY_F1: twKey = TW_KEY_F1; break;
    case GLFW_KEY_F2: twKey = TW_KEY_F2; break;
    case GLFW_KEY_F3: twKey = TW_KEY_F3; break;
    case GLFW_KEY_F4: twKey = TW_KEY_F4; break;
    case GLFW_KEY_F5: twKey = TW_KEY_F5; break;
    case GLFW_KEY_F6: twKey = TW_KEY_F6; break;
    case GLFW_KEY_F7: twKey = TW_KEY_F7; break;
    case GLFW_KEY_F8: twKey = TW_KEY_F8; break;
    case GLFW_KEY_F9: twKey = TW_KEY_F9; break;
    case GLFW_KEY_F10: twKey = TW_KEY_F10; break;
    case GLFW_KEY_F11: twKey = TW_KEY_F11; break;
    case GLFW_KEY_F12: twKey = TW_KEY_F12; break;
    case GLFW_KEY_F13: twKey = TW_KEY_F13; break;
    case GLFW_KEY_F14: twKey = TW_KEY_F14; break;
    case GLFW_KEY_F15: twKey = TW_KEY_F15; break;
    }
    if (twKey == 0 && ctrl && key < 128)
    {
      twKey = key;
    }
    if (twKey != 0)
    {
      if (TwKeyPressed(twKey, twMod)) return;
    }
  }
}

static void charCallback(GLFWwindow* window, unsigned int key)
{
  if (TwKeyPressed(key, 0)) return;
}

static void mousebuttonCallback(GLFWwindow* _window, int _button, int _action, int _mods)
{
    if (TwEventMouseButtonGLFW(_button, _action)) return;

    if (_button == GLFW_MOUSE_BUTTON_LEFT) {
        if (_action == GLFW_PRESS) {
            g_cameraDragging = true;
            glfwGetCursorPos(_window, &g_lastMouseX, &g_lastMouseY);
        } else if (_action == GLFW_RELEASE) {
            g_cameraDragging = false;
        }
    }

    if (_button == GLFW_MOUSE_BUTTON_RIGHT) {
      if (_action == GLFW_PRESS) {
        g_cameraPosX = 0;
        g_cameraPosY = 0;
        g_cameraPosZ = 5.0f; // Reset camera position
      }
    }
}

static void mousePosCallback(GLFWwindow* _window, double _xpos, double _ypos)
{
  if (TwEventMousePosGLFW((int)_xpos, (int)_ypos)) return;

  if (g_cameraDragging) {
      double dx = _xpos - g_lastMouseX;
      double dy = _ypos - g_lastMouseY;

      int width, height;
      glfwGetWindowSize(_window, &width, &height);
      g_cameraPosX += (float)dx / width * 2.0f;  // Scale to screen
      g_cameraPosY -= (float)dy / height * 2.0f; // Inverted Y

      g_lastMouseX = _xpos;
      g_lastMouseY = _ypos;
  }
}

static void mouseScrollCallback(GLFWwindow* _window, double _xoffset, double _yoffset)
{
  static double pos = 0;
  pos += _yoffset;
  g_cameraPosZ -= (float)_yoffset * 0.05f; // Zoom sensitivity
  if (g_cameraPosZ < 1.0f) g_cameraPosZ = 1.0f; // Prevent too close
  if (g_cameraPosZ > 50.0f) g_cameraPosZ = 50.0f; // Prevent too far

  if (TwEventMouseWheelGLFW((int)pos)) return;
}

// static void framebufferSizeCallback(GLFWwindow* window, int fb_width, int fb_height)
// {
//     if (fb_height == 0) fb_height = 1;
//     float aspect = (float)fb_width / (float)fb_height;
//     float near = 1.0f, far = 100.0f;
//     float fov = 45.0f;
//     float top = tan(fov * 0.01745329251f) * near;
//     float bottom = -top;
//     float right = top * aspect;
//     float left = -right;

//     glViewport(0, 0, fb_width, fb_height);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     glFrustum(left, right, bottom, top, near, far);
// }

static void windowSizeCallback(GLFWwindow* window, int width, int height)
{
  if (height == 0) height = 1;
    float aspect = (float)width / (float)height;
    float near = 1.0f, far = 100.0f;
    float fov = 45.0f;
    float top = tan(fov * 0.01745329251f) * near;
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, near, far);

    TwWindowSize(width, height);
}

void TW_CALL ResetCubePosition(void *clientData)
{
  g_cameraPosX = 0;
  g_cameraPosY = 0;
  g_cameraPosZ = 5.0f; // Reset camera position
}

// Copy function for CDSTRING (required by AntTweakBar)
void TW_CALL CopyCDStringToClient(char **destPtr, const char *src)
{
    size_t len = src ? strlen(src) : 0;
    *destPtr = (char*)realloc(*destPtr, len + 1);
    if (*destPtr) {
        strcpy(*destPtr, src);
    }
}

void TW_CALL PrintTextCallback(void *clientData)
{
    printf("User text: %s\n", g_userText ? g_userText : "(null)");
    fflush(stdout);
}

// This example program draws a possibly transparent cube 
void DrawModel(int _wireframe)
{
  int pass, numPass;
  // Enable OpenGL transparency and light (could have been done once at init)
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHT0);    // use default light diffuse and position
  glEnable(GL_NORMALIZE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(3.0);
  
  if( _wireframe )
  {
      glDisable(GL_CULL_FACE);    
      glDisable(GL_LIGHTING);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      numPass = 1;
  }else{
      glEnable(GL_CULL_FACE); 
      glFrontFace(GL_CCW);
      glEnable(GL_LIGHTING);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      numPass = 2;
  }

  for(pass = 0; pass < numPass; ++pass)
  {
    // Since the material could be transparent, we draw the convex model in 2 passes:
    // first its back faces, and second its front faces.
    glCullFace( (pass==0) ? GL_FRONT : GL_BACK );

    // Draw the model (a cube)
    glBegin(GL_QUADS);
      // Front face (z = +0.5)
      glNormal3f(0, 0, 1);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      // Back face (z = -0.5)
      glNormal3f(0, 0, -1);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);

      // Left face (x = -0.5)
      glNormal3f(-1, 0, 0);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      // Right face (x = +0.5)
      glNormal3f(1, 0, 0);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);

      // Bottom face (y = -0.5)
      glNormal3f(0, -1, 0);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      // Top face (y = +0.5)
      glNormal3f(0, 1, 0);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
    glEnd();
  }
}

void error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
    fflush(stderr);
}


// Main
int main() 
{  
  GLFWwindow* window; // GLFW3 window
  TwBar *bar;         // Pointer to a tweak bar
    
  double time = 0, dt;// Current time and enlapsed time
  double turn = 0;    // Model turn counter
  double speed = 0.3; // Model rotation speed
  int wire = 0;       // Draw model in wireframe?
  float bgColor[] = { 73.0/255, 25.0/255, 100.0/255 };         // Background color 
  unsigned char cubeColor[] = { 255, 170, 0, 250 }; // Model color (32bits RGBA)


  // Set error callback
  glfwSetErrorCallback(error_callback);

  // Intialize GLFW   
  if(!glfwInit())
  {
      fprintf(stderr, "GLFW initialization failed\n");
      return 1;
  }

  // Disable Retina scaling for now
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);  
  window = glfwCreateWindow(800, 600, "AntTweakBar + GLFW2", NULL, NULL);
  if(!window)
  {
      fprintf(stderr, "Cannot open GLFW window\n");
      glfwTerminate();
      return -1;
  }

  glfwMakeContextCurrent(window);
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
  {
      fprintf(stderr, "Failed to initialize GLAD\n");
      return -2;
  }

  // Initialize AntTweakBar
  if (!TwInit(TW_OPENGL, NULL)) {
      const char* err = TwGetLastError();
      fprintf(stderr, "TwInit failed: %s\n", err ? err : "Unknown error");
      fflush(stderr);
      return -3;
  }
  {
    int width, hight;
    // glfwGetFramebufferSize(window, &width, &hight);
    // framebufferSizeCallback(window, width, hight);
    glfwGetWindowSize(window, &width, &hight); 
    windowSizeCallback(window, width, hight);
  }
  TwCopyCDStringToClientFunc(CopyCDStringToClient);
    
  // Create a tweak bar
  bar = TwNewBar("TweakBar");
  TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.
  TwDefine(" TweakBar size='220 530' color='100 100 50' alpha=200 ");
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

  // Add a button to reset the cube position
  TwAddButton(bar, "Reset Position", ResetCubePosition, NULL,
            " label='Reset Cube Position' key=r help='Reset pan and zoom.' ");

  // Add an editable text field to the tweak bar
  TwAddVarRW(bar, "Text", TW_TYPE_CDSTRING, &g_userText,
            " label='Input Text' help='Editable dynamic string.' ");

  TwAddButton(bar, "PrintText", PrintTextCallback, NULL,
              " label='Print Text' help='Prints the text to stdout' ");

  glfwSetKeyCallback(window, keyCallback);
  glfwSetCharCallback(window, charCallback);
  glfwSetMouseButtonCallback(window, mousebuttonCallback);
  glfwSetCursorPosCallback(window, mousePosCallback);
  glfwSetScrollCallback(window, mouseScrollCallback);
  glfwSetWindowSizeCallback(window, windowSizeCallback);
  // glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // Initialize time
  time = glfwGetTime();

  // Main loop (repeated while window is not closed and [ESC] is not pressed)
  while (!glfwWindowShouldClose(window))
  {
    // Clear frame buffer
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update rotation
    dt = glfwGetTime() - time;
    if (dt < 0) dt = 0;
    time += dt;
    turn += speed * dt;

    // Setup MODELVIEW matrix (projection is already set once)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLfloat light_pos[] = { 1.0f, 1.0f, 5.0f, 1.0f }; // w=1.0 = positional light
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glTranslated(g_cameraPosX, g_cameraPosY, -g_cameraPosZ); 
    glRotated(360.0 * turn, 0.4, 1, 0.2);

    // Draw model
    glColor4ubv(cubeColor);
    DrawModel(wire);

    // Draw tweak bars
    TwDraw();

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Terminate AntTweakBar and GLFW
  TwTerminate();
  glfwTerminate();

  return 0;
}