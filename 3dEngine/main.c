#include "src/include/SDL2/SDL_main.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include "src/include/GL/glew.h"
#include <GL/GLU.h>
#include <stdio.h>
#include <stdbool.h>
#include "src/include/SDL2/SDL.h"
#include "src/include/SDL2/SDL_video.h"
#include "math.h"
#include "shader.h"
#include "macro.h"
#include "camera.h"
#define  CIMGUI_USE_SDL2
#define  CIMGUI_USE_OPENGL3
#define  CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "src/include/cimgui/cimgui.h"
#include "src/include/cimgui/cimgui_impl.h"
#include "src/include/cglm/cglm.h"
#define STB_IMAGE_IMPLEMENTATION
#include "src/include/STBI/stb_image.h"
#define WIDTH 1280
#define HEIGHT 720

static struct
{
    SDL_Window *window;
    SDL_Event event;
    SDL_GLContext glContext;
    float deltaTime;
    const Uint8 *keyboardStates;
    bool IsGameRuning;
    bool isMouseLocked;
    bool showDemoWindow;
} state;

static vec3 cubePositions[] = 
    {
      {0.0f,  0.0f,  0.0f},
      {2.0f,  5.0f, -15.0f},
      {-1.5f, -2.2f, -2.5f},
      {-3.8f, -2.0f, -12.3f},
      {2.4f, -0.4f, -3.5f},
      {1.5f,  2.0f, -2.5f}
    };
void Update(Camera *camera,float deltaTime, shaderStruct *shader);



bool isKeyDown(SDL_Scancode scanCode)
{
  return state.keyboardStates[scanCode];
}

GLenum errorCheck (int checkNumber)
{
    GLenum code;
    const GLubyte* string;
    code = glGetError();
    if (code != GL_NO_ERROR) {
        string = gluErrorString(code);
        printf("Opengl Error: %d , check number %d\n",code,checkNumber);
  }
    return code;
}

void Render(shaderStruct*shader, GLuint vao, GLuint* textures, float time)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  igNewFrame();
  if(state.showDemoWindow)
    igShowDemoWindow(&state.showDemoWindow);
  igRender();
  glViewport(0, 0,WIDTH , HEIGHT);
  glClearColor(0.2f, 0.3f, 0.3f, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  UseShader(shader);
  glBindVertexArray(vao);
  for(int i = 0; i < 6; i++)
  {
   mat4 model;
   glm_mat4_identity(model);
   glm_translate(model, cubePositions[i]);
   float angle = 20.0f * i;
   glm_rotate(model, DEG2RAD(time/ 10.0f), (vec3){1.0f,0.2f,0.0f});
   SetMatrix4f(shader, "model", model);
   glDrawArrays(GL_TRIANGLES, 0,36);
  }
  SDL_GL_SwapWindow(state.window);
}


int main(int argc, char** argv)
{

 //SDL
  SDL_Init(SDL_INIT_EVERYTHING);
  state.window = SDL_CreateWindow("Injection", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_OPENGL);

  //Logic
  state.isMouseLocked = true;
  state.IsGameRuning = true;
  state.showDemoWindow = true;

//OpenGL
  state.glContext = SDL_GL_CreateContext(state.window);
  glewInit();
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
  glEnable(GL_DEPTH_TEST);
//ImGUI
   igCreateContext(NULL);
   ImGuiIO* ioptr = igGetIO_Nil();
   ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
   ImGui_ImplSDL2_InitForOpenGL(state.window, state.glContext);
   ImGui_ImplOpenGL3_Init("#version 330");
  
  ImVec4 clearColor;
  clearColor.x = 0.45f;
  clearColor.y = 0.55f;
  clearColor.z = 0.60f;
  clearColor.w = 1.00f;
//stbi
  stbi_set_flip_vertically_on_load(true); 

  Camera mainCamera;
  shaderStruct defaultShader;
  InitializeCamera(&mainCamera, (vec3){0.0f,0.0f,-3.0f}, (vec3){0.0f, 1.0f, 0.0f}, DEFAULT_CAM_YAW, DEFAULT_CAM_PITCH, 0.1f, 100.f);
unsigned int VBO; 
unsigned int VAO;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
  glGenVertexArrays(1,&VAO);
  glGenBuffers(1,&VBO);


  glBindVertexArray(VAO);  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);



  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  

  glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 *sizeof(float)));
  glEnableVertexAttribArray(1);


  glBindBuffer(GL_ARRAY_BUFFER,0);

  glBindVertexArray(0);


  int width, height, nrChannels;
  unsigned char *data = stbi_load("E:\\projects\\3dengine\\textures\\happy.png", &width, &height, &nrChannels, 0);
  unsigned int textures[1];
  glGenTextures(1, &textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  

  CreateShader(&defaultShader, "e:\\projects\\3dengine\\shaders\\defaultvertex.vs","e:\\projects\\3dengine\\shaders\\defaultfragment.fs");
  stbi_image_free(data);
  mat4 view;

  mat4 projection;
  glm_perspective(DEG2RAD(mainCamera.fov), WIDTH / HEIGHT, 0.1f, 100.0f, projection); 
  UseShader(&defaultShader);
  SetInt(&defaultShader, "texture1", 0);
  SetMatrix4f(&defaultShader, "projection", projection);


  float lastTickTime = 0;
  state.deltaTime = 0;
  while(state.IsGameRuning)
  {
    int i = 0;
    float angle = 30.0f * i;
    float time = SDL_GetTicks();
    state.deltaTime = (time - lastTickTime) / 1000.0f;
    Update(&mainCamera,state.deltaTime,&defaultShader);
    SetFloat(&defaultShader,"time", time);
    GetViewMatrixFromCamera(mainCamera, view);
    SetMatrix4f(&defaultShader, "view", view);
    Render(&defaultShader,VAO,textures,time);
    SDL_Delay(1);
    lastTickTime = time;
  }
  ImGui_ImplSDL2_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  igDestroyContext(NULL);
  SDL_GL_DeleteContext(state.glContext);
  SDL_Quit();
  return 0;
}



void Update(Camera *camera,float deltaTime,shaderStruct *shader)
{
    state.keyboardStates = SDL_GetKeyboardState(NULL);
    if(isKeyDown(SDL_SCANCODE_W))
    {
      UpdateCameraMovement(camera, FORWARD, deltaTime);
    }
    if(isKeyDown(SDL_SCANCODE_S))
    {
      UpdateCameraMovement(camera, BACKWARD, deltaTime);
    }
    if(isKeyDown(SDL_SCANCODE_A))
    {
      UpdateCameraMovement(camera, LEFT, deltaTime);
    }
    if(isKeyDown(SDL_SCANCODE_D))
    {
      UpdateCameraMovement(camera, RIGHT, deltaTime);
    }
    while(SDL_PollEvent(&state.event) != 0)
    {
      ImGui_ImplSDL2_ProcessEvent(&state.event);
      if(state.event.type == SDL_QUIT)
        state.IsGameRuning = false;
      if(state.event.type == SDL_MOUSEMOTION)
      {
        float dx, dy;
        dx = state.event.motion.xrel;
        dy = state.event.motion.yrel;
        if(state.isMouseLocked)
        ProcessMouseMovement(camera, dx, dy, true);
      }
      if(state.event.type == SDL_KEYDOWN)
      {
        switch(state.event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            state.IsGameRuning= false;
          case SDLK_f:
            if(state.isMouseLocked)
            {
              state.isMouseLocked = false;
              break;
            }
            state.isMouseLocked = true;
            break;
        }
      }
   }
  SDL_SetRelativeMouseMode(state.isMouseLocked);
  UseShader(shader);
}

