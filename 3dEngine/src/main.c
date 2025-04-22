#include "allincludes.h"
#include <minwindef.h>

#define WIDTH 1600.0
#define HEIGHT 900.0

static struct
{
    SDL_Window *window;
    SDL_Event event;
    SDL_GLContext glContext;
    float deltaTime;
    Scene* loadedScene;
    Node* selected_node;
    const Uint8 *keyboardStates;
    bool IsGameRuning;
    bool isMouseLocked;
} state;

static struct
{
  unsigned int diffuseMap;
  unsigned int specularMap;
  float shininess;
} material;
bool isKeyDown(SDL_Scancode scanCode)
{
  return state.keyboardStates[scanCode];
}

void Update(Scene* scene, float deltaTime);
unsigned int LoadCubemap(dynlist_t* faces);
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

void mat3_to_mat4(mat3 src, mat4 dest)
{
  for(int i = 0; i < 3;  i++)
  {
    for(int j = 0; j < 3; j++)
    {
      dest[i][j] = src[i][j];
    }
  }
  for(int i = 0; i < 4; i++)
  {
    dest[3][i] = 0;
  }
}

static ImVec4 clearColor = {0.1f, 0.1f, 0.1f, 1.0f};
//static ImVec4 meshColor = { 1.0f, 1.0f, 1.0f, 1.0f};
static float f = 0.0f;
static PointLight pointLights[4];
static shaderStruct modelshader;
static shaderStruct skyboxShader;
void Render(Scene* scene, ImGuiIO* io, float time, unsigned int skyboxVAO, unsigned int cubemap)
{ if(!scene)
    return;
  cImGui_ImplOpenGL3_NewFrame();
  cImGui_ImplSDL2_NewFrame();
  ImGui_NewFrame();
  DrawSceneHierarchy(scene, &state.selected_node);
  DrawSceneInspector(&scene);
  if(state.selected_node)
  {
    DrawNodeInspector(state.selected_node);
  }
  ImGui_Render();
  glViewport(0, 0,WIDTH , HEIGHT);
  glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SceneRender(scene, &modelshader);
  glDepthFunc(GL_LEQUAL);
  UseShader(&skyboxShader);
  mat3 view3;
  glm_mat4_pick3(scene->activeCamera->view, view3);
  mat3_to_mat4(view3,scene->activeCamera->view);
  SetShaderMatrix4f(&skyboxShader, "view", scene->activeCamera->view);
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default 
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

//OpenGL
  state.glContext = SDL_GL_CreateContext(state.window);
  glewInit();
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
//ImGUI
   ImGui_CreateContext(NULL);
   ImGuiIO* ioptr = ImGui_GetIO();
   ioptr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
   cImGui_ImplSDL2_InitForOpenGL(state.window, state.glContext);
   cImGui_ImplOpenGL3_InitEx("#version 330");
  
//stbi
  stbi_set_flip_vertically_on_load(true); 

  state.loadedScene = SceneCreate(false);
  SceneDemoSetup(state.loadedScene); 
unsigned int VBO; 

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
  glGenBuffers(1,&VBO);


  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
  printf("kaif\n");
  dynlist_t* cubemap = dynlistInit(sizeof(char*), 4);
  char *tx1 = "E:\\projects\\deltaengine\\3dengine\\textures\\right.png\0";
  char *tx2 = "E:\\projects\\deltaengine\\3dengine\\textures\\left.png\0";
  char *tx3 = "E:\\projects\\deltaengine\\3dengine\\textures\\top.png\0";
  char *tx4 = "E:\\projects\\deltaengine\\3dengine\\textures\\bottom.png\0";
  char *tx5 = "E:\\projects\\deltaengine\\3dengine\\textures\\front.png\0";
  char *tx6 = "E:\\projects\\deltaengine\\3dengine\\textures\\back.png\0";
  dynlistPush(cubemap, &tx1);
  dynlistPush(cubemap, &tx2);
  dynlistPush(cubemap, &tx3);
  dynlistPush(cubemap, &tx4);
  dynlistPush(cubemap, &tx5);
  dynlistPush(cubemap, &tx6);

  glBindBuffer(GL_ARRAY_BUFFER,0);
  unsigned int skyboxVAO;
  glGenVertexArrays(1,&skyboxVAO);
  glBindVertexArray(skyboxVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  unsigned int cubemapTxt = LoadCubemap(cubemap);



  CreateShader(&modelshader, "E:\\projects\\deltaengine\\3dengine\\shaders\\modelvertex.vs", "E:\\projects\\deltaengine\\3dengine\\shaders\\modelfragment.fs");
  CreateShader(&skyboxShader, "E:\\projects\\deltaengine\\3dengine\\shaders\\skybox.vs", "E:\\projects\\deltaengine\\3dengine\\shaders\\skybox.fs");
  
  


  
  float lastTickTime = 0;
  state.deltaTime = 0;
  while(state.IsGameRuning)
  {
    int i = 0;
    float time = SDL_GetTicks();
    
    state.deltaTime = (time - lastTickTime) / 1000.0f;
    Update(state.loadedScene, state.deltaTime);  
    SetShaderFloat(&modelshader,"time", time);
    Render(state.loadedScene, ioptr,time,skyboxVAO, cubemapTxt);
    lastTickTime = time;
  }
  dynlistFree(cubemap);
  SceneDestroy(state.loadedScene);
  cImGui_ImplSDL2_Shutdown();
  cImGui_ImplOpenGL3_Shutdown();
  ImGui_DestroyContext(NULL);
  SDL_GL_DeleteContext(state.glContext);
  SDL_Quit();
  return 0;
}



void Update(Scene* scene, float deltaTime)
{   
  if(!scene)
    return;
      state.keyboardStates = SDL_GetKeyboardState(NULL);
      if(state.isMouseLocked)
      {
        if(isKeyDown(SDL_SCANCODE_W))
          CameraNodeHandleWASD(scene->activeCamera, deltaTime, CAMERA_EDIT_FORWARD);
        if(isKeyDown(SDL_SCANCODE_S))
          CameraNodeHandleWASD(scene->activeCamera, deltaTime, CAMERA_EDIT_BACKWARD);
        if(isKeyDown(SDL_SCANCODE_A))
          CameraNodeHandleWASD(scene->activeCamera, deltaTime, CAMERA_EDIT_LEFT);
        if(isKeyDown(SDL_SCANCODE_D))
          CameraNodeHandleWASD(scene->activeCamera, deltaTime, CAMERA_EDIT_RIGHT);
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
          CameraHandleMouse(scene->activeCamera, dx, dy, true); 
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
  CalcViewMatFromCamera(scene->activeCamera);
  UseShader(&skyboxShader);
  SetShaderMatrix4f(&skyboxShader, "projection", scene->activeCamera->projection);

  UseShader(&modelshader);
  SetShaderMatrix4f(&modelshader, "projection", scene->activeCamera->projection);
  SetShaderMatrix4f(&modelshader,"view",scene->activeCamera->view);
  SDL_SetRelativeMouseMode(state.isMouseLocked);
}
unsigned int LoadCubemap(dynlist_t* faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces->size; i++)
    { 
        char* texture = *(char**)dynlistAt(faces, i);
        unsigned char *data = stbi_load(texture, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            printf("Cubemap tex failed to load at path: %s", texture);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}  
