//  ---------------------------------------------------------------------------
//
//  @file       TwSimpleGLFW3.c
//  @brief      A simple example that uses AntTweakBar with 
//              OpenGL and the GLFW3 windowing system.
//
//              AntTweakBar: http://anttweakbar.sourceforge.net/doc
//              OpenGL:      http://www.opengl.org
//              GLFW3:       http://www.glfw.org
//  
//  @author     Nikolai Kiselev
//  @date       2025/06/05
//
//  ---------------------------------------------------------------------------

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

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

static void mousebuttonCallback(GLFWwindow* window, int button, int action, int mods)
{
  if (TwEventMouseButtonGLFW(button, action)) return;
}

static void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
  if (TwEventMousePosGLFW((int)xpos, (int)ypos)) return;
}

static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  static double pos = 0;
  pos += yoffset;
  if (TwEventMouseWheelGLFW((int)pos)) return;
}

static void resizeCallback(GLFWwindow* _window, int _width, int _height)
{
  if (_height == 0) _height = 1;
    float aspect = (float)_width / (float)_height;
    float near = 1.0f, far = 100.0f;
    float fov = 45.0f;
    float top = tan(fov * 0.01745329251f) * near;
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    glViewport(0, 0, _width, _height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left, right, bottom, top, near, far);

  // Notify AntTweakBar of the window size
  TwWindowSize(_width, _height);
}

int main() 
{ 
  GLFWwindow* window; // GLFW3 window
  TwBar *bar;         // Pointer to a tweak bar
  
  double time = 0, dt;// Current time and enlapsed time
  double turn = 0;    // Model turn counter
  double speed = 0.3; // Model rotation speed
  int wire = 0;       // Draw model in wireframe?
  float bgColor[] = { 0.1f, 0.2f, 0.4f };         // Background color 
  unsigned char cubeColor[] = { 255, 0, 0, 128 }; // Model color (32bits RGBA)

  // Intialize GLFW   
  if (!glfwInit()) {
      fprintf(stderr, "GLFW initialization failed\n");
      return 1;
  }

  // No version/profile hints: this AntTweakBar build only supports the
  // OpenGL compatibility profile (TW_OPENGL_CORE crashes), so we let GLFW
  // create its default (non-core) context.

  window = glfwCreateWindow(640, 480, "AntTweakBar + GLFW3", NULL, NULL);
  if (!window)
  {
      fprintf(stderr, "Cannot open GLFW window\n");
      glfwTerminate();
      return 1;
  }
  
  glfwMakeContextCurrent(window);
  //

  // Load OpenGL functions
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      fprintf(stderr, "Failed to initialize GLAD\n");
      return -1;
  }
  // glfwSetWindowTitle(window, "AntTweakBar simple example using GLFW3");
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));


  // Initialize AntTweakBar
  if (!TwInit(TW_OPENGL, NULL)) {
      const char* err = TwGetLastError();
      fprintf(stderr, "TwInit failed: %s\n", err ? err : "Unknown error");
      fflush(stderr);
      return 1;
  }
  int width = 0, height = 0;
  do {
      glfwGetFramebufferSize(window, &width, &height);
      glfwPollEvents(); // Ensure GLFW event loop updates window size
  } while (width == 0 || height == 0); // Wait until we get a real size

  resizeCallback(window, width, height);



  // Create a tweak bar
  bar = TwNewBar("TweakBar");
  TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.' "); // Message added to the help bar.

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
  char* myString = (char*)malloc(512);
  strcpy(myString, "Hello world");
  // Register as editable string (CDSTRING with default 512 chars)
  TwAddVarRW(bar, "Text", TW_TYPE_CDSTRING, &myString, " label='Text to change' ");


  glfwSetKeyCallback(window, keyCallback);
  glfwSetCharCallback(window, charCallback);
  glfwSetMouseButtonCallback(window, mousebuttonCallback);
  glfwSetCursorPosCallback(window, mousePosCallback);
  glfwSetScrollCallback(window, mouseScrollCallback);
  glfwSetWindowSizeCallback(window, resizeCallback);


  // Initialize time
  time = glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
      int width, height;
      glfwGetFramebufferSize(window, &width, &height);
      glViewport(0, 0, width, height);

      // Just clear the background
      glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // Draw tweak bar only
      TwDraw();

      glfwSwapBuffers(window);
      glfwPollEvents();
  }





  // Terminate AntTweakBar and GLFW
  TwTerminate();
  glfwTerminate();

  return 0;
}

