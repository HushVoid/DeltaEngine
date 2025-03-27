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
#include "src/include/cglm/cglm.h"
#include "src/include/generated/dcimgui.h"
#include "src/include/generated/backends/dcimgui_impl_sdl2.h"
#include "src/include/generated/backends/dcimgui_impl_opengl3.h"
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
    bool showAnotherWindow;
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

static struct
{
  unsigned int diffuseMap;
  unsigned int specularMap;
  float shininess;
} material;

static struct
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float intensity;
} light;
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

static ImVec4 clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
//static ImVec4 meshColor = { 1.0f, 1.0f, 1.0f, 1.0f};
static float f = 0.0f;
void Render(shaderStruct*objectshader,shaderStruct*lightShader,ImGuiIO* io, GLuint vao, GLuint lightVAO, GLuint* textures, float time)
{ 
  cImGui_ImplOpenGL3_NewFrame();
  cImGui_ImplSDL2_NewFrame();
  ImGui_NewFrame();
 {
      static int counter = 0;

      ImGui_Begin("Inspecta!", NULL, ImGuiWindowFlags_None);                          // Create a window called "Hello, world!" and append into it.

      ImGui_Text("This is some useful text.");               // Display some text (you can use a format strings too)
      ImGui_Checkbox("Demo Window", &state.showDemoWindow);      // Edit bools storing our window open/close state
      ImGui_Checkbox("Material Window", &state.showAnotherWindow);
      //ImGui_SliderFloat("Mix value", &f, 0, 1);
      ImGui_ColorEdit3("clear color", (float*)&clearColor , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
 //     ImGui_ColorEdit3("cube color", (float*)&meshColor , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_ColorEdit3("light ambient", light.ambient , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_ColorEdit3("light diffuse", light.diffuse , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_ColorEdit3("light specular", light.specular , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_InputFloat3("light pos", light.position);
      ImGui_SliderFloat("light intensity", &light.intensity, 0.001f, 100.0f);
    // 
      if (ImGui_Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
          counter++;
      ImGui_SameLine();
      ImGui_Text("counter = %d", counter);

      ImGui_Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
      ImGui_End();
  }
  if(state.showDemoWindow)
    ImGui_ShowDemoWindow(&state.showDemoWindow);
  if(state.showAnotherWindow)
  {
      ImGui_Begin("Material", NULL, ImGuiWindowFlags_None);
  
      ImGui_SliderFloat("Shininess", &material.shininess, 0.0f, 512.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui_End();
  }
  ImGui_Render();
  glViewport(0, 0,WIDTH , HEIGHT);
  glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, material.diffuseMap);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, material.specularMap);
  UseShader(objectshader);
  glBindVertexArray(vao);
  //SetShaderFloat(objectshader, "mixValue", f);
 // SetShaderFloat4(objectshader, "myColor", (vec4){meshColor.x * meshColor.w ,meshColor.y * meshColor.w,meshColor.z * meshColor.w,meshColor.w});

  //setting the material
  
  SetShaderFloat(objectshader, "material.shininess", material.shininess);

  SetShaderFloat3(objectshader, "light.ambient", light.ambient);
  SetShaderFloat3(objectshader, "light.diffuse", light.diffuse);
  SetShaderFloat3(objectshader, "light.specular", light.specular);


  
 for(int i = 0; i < 6; i++)
  {
   mat4 model;
   mat3 normalMatrix;
   glm_mat4_identity(model);
   glm_translate(model, cubePositions[i]);
   float angle = 20.0f * i;
   glm_rotate(model, DEG2RAD(time/ 10.0f), (vec3){1.0f,0.2f,0.0f});
   glm_mat4_pick3(model, normalMatrix);
   glm_mat3_inv(normalMatrix, normalMatrix);
   glm_mat3_transpose(normalMatrix);
   SetShaderMatrix4f(objectshader, "model", model);
   SetShaderMatrix3f(objectshader,"normalMatrix", normalMatrix);
   glDrawArrays(GL_TRIANGLES, 0,36);
  }
  mat4 model;
  glm_mat4_identity(model);
  glm_translate(model, light.position);
  glm_scale(model,(vec3){0.7f,0.7f,0.7f});
  UseShader(lightShader);
  glBindVertexArray(lightVAO);
  SetShaderMatrix4f(lightShader, "model", model);
  SetShaderFloat3(lightShader, "light.ambient", light.ambient);
  SetShaderFloat3(lightShader, "light.diffuse", light.diffuse);
  SetShaderFloat3(lightShader, "light.specular", light.specular);
  //glDrawArrays(GL_TRIANGLES, 0, 36);
  cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());
  SDL_GL_SwapWindow(state.window);
}


int main(int argc, char** argv)
{

 //SDL
  SDL_Init(SDL_INIT_EVERYTHING);
  state.window = SDL_CreateWindow("Delta", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_OPENGL);

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
   ImGui_CreateContext(NULL);
   ImGuiIO* ioptr = ImGui_GetIO();
   ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
   cImGui_ImplSDL2_InitForOpenGL(state.window, state.glContext);
   cImGui_ImplOpenGL3_InitEx("#version 330");
  
//stbi
  stbi_set_flip_vertically_on_load(true); 

//light
  light.position[2] = -5.0f;
  light.intensity = 1.0f; 

  material.shininess = 2.0f;

  light.specular[0] = 1.0f;
  light.specular[1] = 1.0f;
  light.specular[2] = 1.0f;

  light.ambient[0] = 0.1f;
  light.ambient[1] = 0.1f;
  light.ambient[2] = 0.1f;

  light.diffuse[0] = 0.5f;
  light.diffuse[1] = 0.5f;
  light.diffuse[2] = 0.5f;

  Camera mainCamera;
  shaderStruct objectshader;
  shaderStruct lightShader;
  InitializeCamera(&mainCamera, (vec3){0.0f,0.0f,-3.0f}, (vec3){0.0f, 1.0f, 0.0f}, DEFAULT_CAM_YAW, DEFAULT_CAM_PITCH, 0.1f, 100.f);
unsigned int VBO; 
unsigned int VAO;

float vertices[] = {//3 vertex coords //2 texture coords //3 normals
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
};
  glGenVertexArrays(1,&VAO);
  glGenBuffers(1,&VBO);


  glBindVertexArray(VAO);  
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 *sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2,3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 *sizeof(float)));
  glEnableVertexAttribArray(2);


  glBindBuffer(GL_ARRAY_BUFFER,0);

  glBindVertexArray(0);

  unsigned int lightVAO;
  glGenVertexArrays(1,&lightVAO);
  glBindVertexArray(lightVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);


  int width, height, nrChannels;
  unsigned char *data = stbi_load("E:\\projects\\deltaengine\\3dengine\\textures\\woodboxtest.png", &width, &height, &nrChannels, 0);
  unsigned int textures[2];
  glGenTextures(1, &material.diffuseMap);
  glBindTexture(GL_TEXTURE_2D, material.diffuseMap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  data = stbi_load("E:\\projects\\deltaengine\\3dengine\\textures\\woddboxtest_specular.png", &width, &height, &nrChannels, 0);
  glGenTextures(1, &material.specularMap);
  glBindTexture(GL_TEXTURE_2D, material.specularMap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  CreateShader(&objectshader, "e:\\projects\\deltaengine\\3dengine\\shaders\\defaultvertex.vs","e:\\projects\\deltaengine\\3dengine\\shaders\\defaultfragment.fs");
  CreateShader(&lightShader, "e:\\projects\\deltaengine\\3dengine\\shaders\\lightCubeVertex.vs","e:\\projects\\deltaengine\\3dengine\\shaders\\lightFragmentShader.fs");
  stbi_image_free(data);
  mat4 view;
  mat4 projection;
  glm_perspective(DEG2RAD(mainCamera.fov), WIDTH / HEIGHT, 0.1f, 100.0f, projection); 
  UseShader(&objectshader);
  SetShaderInt(&objectshader, "material.diffuse", 0);
  SetShaderInt(&objectshader, "material.specular", 1);
  SetShaderMatrix4f(&objectshader, "projection", projection);
  UseShader(&lightShader);
  SetShaderMatrix4f(&lightShader, "projection", projection);

  float lastTickTime = 0;
  state.deltaTime = 0;
  while(state.IsGameRuning)
  {
    int i = 0;
    float time = SDL_GetTicks();
    state.deltaTime = (time - lastTickTime) / 1000.0f;
    Update(&mainCamera,state.deltaTime,&objectshader);
    SetShaderFloat(&objectshader,"time", time);
    GetViewMatrixFromCamera(mainCamera, view);
    SetShaderMatrix4f(&objectshader, "view", view);
    SetShaderFloat3(&objectshader, "viewPos", mainCamera.position);
    UseShader(&lightShader);
    SetShaderFloat(&lightShader,"time", time);
    SetShaderMatrix4f(&lightShader,"view", view);
    Render(&objectshader,&lightShader,ioptr,VAO,lightVAO,textures,time);
    SDL_Delay(1);
    lastTickTime = time;
  }
  cImGui_ImplSDL2_Shutdown();
  cImGui_ImplOpenGL3_Shutdown();
  ImGui_DestroyContext(NULL);
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
      cImGui_ImplSDL2_ProcessEvent(&state.event);
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
  SetShaderFloat3(shader, "light.position", camera->position);
  SetShaderFloat3(shader, "light.direction", camera->frontVec);
  SetShaderFloat(shader, "light.cutOff", cos(DEG2RAD(12.5f)));
  SetShaderFloat(shader, "light.outerCutOff", cos(DEG2RAD(17.5f)));
}

