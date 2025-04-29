#include "allincludes.h"
#include <minwindef.h>
#include <string.h>
#include <winnt.h>

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
    bool isGameRuning;
    bool isMouseLocked;
    bool isPlayingModeOn;
} state;
static struct 
{
  CameraNode* playModeCamera;
  PlayerNode* playModePlayer;
  Transform playerTransform;
  Transform cameraTransform;
} playModeState;


bool isKeyDown(SDL_Scancode scanCode)
{
  return state.keyboardStates[scanCode];
}

void Update(float deltaTime, ImGuiIO* io);
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
static GLuint lightUBO; 
static shaderStruct modelshader;
static shaderStruct skyboxShader;
static shaderStruct nodeShader;
void Render(ImGuiIO* io, float time, unsigned int skyboxVAO, unsigned int cubemap)
{ if(!state.loadedScene)
    return;
  CameraNode* currentCamera = state.isPlayingModeOn ? playModeState.playModeCamera : state.loadedScene->activeCamera;
  if(!state.isPlayingModeOn)
  {
    cImGui_ImplOpenGL3_NewFrame();
    cImGui_ImplSDL2_NewFrame();
    ImGui_NewFrame();
    DrawSceneHierarchy(state.loadedScene, &state.selected_node, lightUBO);
    DrawSceneInspector(&state.loadedScene);
    if(state.selected_node)
    {
      DrawNodeInspector(state.selected_node, state.loadedScene);
    }
    ImGui_Render();
  }
  glViewport(0, 0,WIDTH , HEIGHT);
  glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  vec3 CamGlobalPos;
  SpatialGetGlobalPos((SpatialNode*)currentCamera, CamGlobalPos);
  SetShaderFloat3(&modelshader, "viewPos", CamGlobalPos);
  SceneRender(state.loadedScene, lightUBO, &modelshader, &nodeShader);
  glDepthFunc(GL_LEQUAL);
  UseShader(&skyboxShader);
  mat3 view3;
  glm_mat4_pick3(currentCamera->view, view3);
  mat3_to_mat4(view3,currentCamera->view);
  SetShaderMatrix4f(&skyboxShader, "view", currentCamera->view);
  glBindVertexArray(skyboxVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
  glDepthFunc(GL_LESS); // set depth function back to default
  if(!state.isPlayingModeOn)
  {
    glDisable(GL_DEPTH_TEST);
    if(state.loadedScene->root)
    {
      NodeDrawEditor(state.loadedScene->root, &nodeShader, state.loadedScene->billboard.texture, state.loadedScene->billboard.VAO);
    }
    glEnable(GL_DEPTH_TEST);
  }
  if(!state.isPlayingModeOn)
  {
    cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());
  }
  SDL_GL_SwapWindow(state.window);
}


int main(int argc, char** argv)
{

  //SDL
  SDL_Init(SDL_INIT_EVERYTHING);
  state.window = SDL_CreateWindow("Delta", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,SDL_WINDOW_OPENGL);

  //Logic
  state.isMouseLocked = true;
  state.isGameRuning = true;
  state.isPlayingModeOn = false;
  playModeState.playModeCamera = NULL;
  playModeState.playModePlayer = NULL;

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

//loading test scene;
  state.loadedScene = SceneLoad("scenes\\test.scn");
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


  //skybox buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glGenBuffers(1, &lightUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
  glBufferData(GL_UNIFORM_BUFFER, 
             sizeof(GPUDirLight) + sizeof(GPUPointLight)*10 + sizeof(GPUSpotLight)*10, 
             NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO);

  //loading skybox texures
  char directory[MAX_PATH];
  if(GetModuleFileName(NULL,directory, MAX_PATH) != 0)
  {
    char* lastSlash = strrchr(directory, '\\');
    if(lastSlash != NULL) *lastSlash = '\0';        
  }
  glBufferData(GL_ARRAY_BUFFER,sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
  dynlist_t* cubemap = dynlistInit(sizeof(char[MAX_PATH]), 4);
  char cubemappath1[MAX_PATH];
  strcpy_s(cubemappath1, MAX_PATH, directory);
  strcat_s(cubemappath1, MAX_PATH, "\\textures\\right.png");
  char cubemappath2[MAX_PATH];
  strcpy_s(cubemappath2, MAX_PATH, directory);
  strcat_s(cubemappath2, MAX_PATH, "\\textures\\left.png");
  char cubemappath3[MAX_PATH];
  strcpy_s(cubemappath3, MAX_PATH, directory);
  strcat_s(cubemappath3, MAX_PATH, "\\textures\\top.png");
  char cubemappath4[MAX_PATH];
  strcpy_s(cubemappath4, MAX_PATH, directory);
  strcat_s(cubemappath4, MAX_PATH, "\\textures\\bottom.png");
  char cubemappath5[MAX_PATH];
  strcpy_s(cubemappath5, MAX_PATH, directory);
  strcat_s(cubemappath5, MAX_PATH, "\\textures\\front.png");
  char cubemappath6[MAX_PATH];
  strcpy_s(cubemappath6, MAX_PATH, directory);
  strcat_s(cubemappath6, MAX_PATH, "\\textures\\back.png");
  dynlistPush(cubemap, cubemappath1);
  dynlistPush(cubemap, cubemappath2);
  dynlistPush(cubemap, cubemappath3);
  dynlistPush(cubemap, cubemappath4);
  dynlistPush(cubemap, cubemappath5);
  dynlistPush(cubemap, cubemappath6);

  glBindBuffer(GL_ARRAY_BUFFER,0);
  unsigned int skyboxVAO;
  glGenVertexArrays(1,&skyboxVAO);
  glBindVertexArray(skyboxVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  unsigned int cubemapTxt = LoadCubemap(cubemap);

  //loading shaders 
  char modelShaderPathFS[256];
  char modelShaderPathVS[256];
  char skyboxShaderPathFS[256];
  char skyboxShaderPathVS[256];
  char nodeShaderPathFS[256];
  char nodeShaderPathVS[256];
  strcpy_s(modelShaderPathFS, 256, directory);
  strcpy_s(modelShaderPathVS, 256, directory);
  strcpy_s(skyboxShaderPathFS, 256, directory);
  strcpy_s(skyboxShaderPathVS, 256, directory);
  strcpy_s(nodeShaderPathFS, 256, directory);
  strcpy_s(nodeShaderPathVS, 256, directory);
  strcat_s(modelShaderPathVS, 256, "\\shaders\\modelvertex.vs");
  strcat_s(modelShaderPathFS, 256, "\\shaders\\modelfragment.fs");
  strcat_s(skyboxShaderPathVS, 256, "\\shaders\\skybox.vs");
  strcat_s(skyboxShaderPathFS, 256, "\\shaders\\skybox.fs");
  strcat_s(nodeShaderPathVS, 256, "\\shaders\\bilboard.vs");
  strcat_s(nodeShaderPathFS, 256, "\\shaders\\bilboard.fs");
  CreateShader(&modelshader, modelShaderPathVS, modelShaderPathFS);
  CreateShader(&skyboxShader, skyboxShaderPathVS, skyboxShaderPathFS);
  CreateShader(&nodeShader, nodeShaderPathVS, nodeShaderPathFS);
  
  GLuint lightsIndex = glGetUniformBlockIndex(modelshader.ID, "Lights");
  glUniformBlockBinding(modelshader.ID, lightsIndex, 0);

  
  float lastTickTime = 0;
  state.deltaTime = 0;
  //application loop
  while(state.isGameRuning)
  {
    int i = 0;
    float time = SDL_GetTicks();
    
    state.deltaTime = (time - lastTickTime) / 1000.0f;
    Update(state.deltaTime, ioptr);  
    SetShaderFloat(&modelshader,"time", time);
    Render(ioptr,time,skyboxVAO, cubemapTxt);
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



void Update(float deltaTime, ImGuiIO* io)
{   
  if(!state.loadedScene)
    return;
      state.keyboardStates = SDL_GetKeyboardState(NULL);
      //Editing mode camera
      if(state.isMouseLocked && !state.isPlayingModeOn)
      {
        //wasd movement
        if(isKeyDown(SDL_SCANCODE_W))
          CameraNodeHandleWASD(state.loadedScene->activeCamera, deltaTime, CAMERA_EDIT_FORWARD);
        if(isKeyDown(SDL_SCANCODE_S))
          CameraNodeHandleWASD(state.loadedScene->activeCamera, deltaTime, CAMERA_EDIT_BACKWARD);
        if(isKeyDown(SDL_SCANCODE_A))
          CameraNodeHandleWASD(state.loadedScene->activeCamera, deltaTime, CAMERA_EDIT_LEFT);
        if(isKeyDown(SDL_SCANCODE_D))
          CameraNodeHandleWASD(state.loadedScene->activeCamera, deltaTime, CAMERA_EDIT_RIGHT);
        if(isKeyDown(SDL_SCANCODE_LSHIFT))
          state.loadedScene->activeCamera->speed = CAM_DEFAULT_SPEED * 3.5;
        else
          state.loadedScene->activeCamera->speed = CAM_DEFAULT_SPEED;
      }
    while(SDL_PollEvent(&state.event) != 0)
    {
      cImGui_ImplSDL2_ProcessEvent(&state.event);
      if(state.event.type == SDL_QUIT)
        state.isGameRuning = false;
      //Handling mouse motion
      if(state.event.type == SDL_MOUSEMOTION)
      {
        float dx, dy;
        dx = state.event.motion.xrel;
        dy = state.event.motion.yrel;
        //editing
        if(state.isMouseLocked && !state.isPlayingModeOn) 
          CameraHandleMouse(state.loadedScene->activeCamera, dx, dy, true);
        //playing
        else if(state.isPlayingModeOn && playModeState.playModePlayer)
        {
          PlayerNodeHandleMouse(playModeState.playModePlayer, playModeState.playModeCamera, dx, dy); 
        }
      }
      if(state.event.type == SDL_KEYDOWN && !io->WantCaptureKeyboard)
      {
        switch(state.event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            state.isGameRuning= false;
          case SDLK_f:
            if(state.isMouseLocked)
            {
              state.isMouseLocked = false;
              break;
            }
            state.isMouseLocked = true;
            break;
         case SDLK_p:
          {
            if(!state.isPlayingModeOn && state.isMouseLocked)
            {
              CameraNode* activeCam = SceneFindActiveCamera(state.loadedScene); 
              if(!activeCam)
                printf("No active camera found, can't enter play mode \n");
              else
              {
                TransformCopyTo(&activeCam->base.transform, &playModeState.cameraTransform);
                playModeState.playModeCamera = activeCam;
                PlayerNode* player = SceneFindActivePlayer(state.loadedScene);
                if(player)
                {
                  TransformCopyTo(&player->base.transform, &playModeState.playerTransform);
                  playModeState.playModePlayer = player;
                }
                state.isPlayingModeOn = true;
              }
            }
            else 
            {
              CameraNode* activeCam = SceneFindActiveCamera(state.loadedScene); 
              state.isPlayingModeOn = false;
              if(playModeState.playModePlayer)
              {
                PlayerNode* player = SceneFindActivePlayer(state.loadedScene);
                TransformCopyTo(&playModeState.playerTransform, &player->base.transform);
                TransformCopyTo(&playModeState.cameraTransform, &activeCam->base.transform);
                glm_vec3_fill(player->velocity, 0);
                SpatialNodeUpdateGlobalTransform((SpatialNode*) player);
                playModeState.playModePlayer = NULL;
                playModeState.playModeCamera = NULL;
              }
              else if(playModeState.playModeCamera)
              {
                TransformCopyTo(&playModeState.cameraTransform, &activeCam->base.transform);
                SpatialNodeUpdateGlobalTransform((SpatialNode*) activeCam);
                playModeState.playModeCamera = NULL;
              }
            }
            break;
          }
        
        }
      }

    }

     
  if(state.isPlayingModeOn)
  {
    if(playModeState.playModePlayer)
    {
      if(isKeyDown(SDL_SCANCODE_W))
        PlayerNodeUpdate(playModeState.playModePlayer, deltaTime, PLAYER_FORWARD);
      if(isKeyDown(SDL_SCANCODE_S))
        PlayerNodeUpdate(playModeState.playModePlayer, deltaTime, PLAYER_BACKWARD);
      if(isKeyDown(SDL_SCANCODE_A))
        PlayerNodeUpdate(playModeState.playModePlayer, deltaTime, PLAYER_LEFT);
      if(isKeyDown(SDL_SCANCODE_D))
        PlayerNodeUpdate(playModeState.playModePlayer, deltaTime, PLAYER_RIGHT);
      if(isKeyDown(SDL_SCANCODE_SPACE))
        PlayerNodeUpdate(playModeState.playModePlayer, deltaTime, PLAYER_JUMP);
      IsPlayerGrounded(playModeState.playModePlayer, state.loadedScene, 0.2f);
      PlayerNodePhysicsStep(playModeState.playModePlayer, deltaTime);
    }
      CalcViewMatFromCamera(playModeState.playModeCamera);
      UseShader(&skyboxShader);
      SetShaderMatrix4f(&skyboxShader, "projection", playModeState.playModeCamera->projection);

      UseShader(&modelshader);
      SetShaderMatrix4f(&modelshader, "projection",playModeState.playModeCamera->projection);
      SetShaderMatrix4f(&modelshader,"view", playModeState.playModeCamera->view);
      
    ScenePhysicsUpdateCollisions(state.loadedScene);
    SDL_SetRelativeMouseMode(true);
  }
  else 
  {
    CalcViewMatFromCamera(state.loadedScene->activeCamera);
    UseShader(&skyboxShader);
    SetShaderMatrix4f(&skyboxShader, "projection", state.loadedScene->activeCamera->projection);

    UseShader(&modelshader);
    SetShaderMatrix4f(&modelshader, "projection", state.loadedScene->activeCamera->projection);
    SetShaderMatrix4f(&modelshader,"view",state.loadedScene->activeCamera->view);
    UseShader(&nodeShader);
    SetShaderMatrix4f(&nodeShader, "view", state.loadedScene->activeCamera->view);
    SetShaderMatrix4f(&nodeShader, "projection", state.loadedScene->activeCamera->projection);
    SetShaderFloat3(&nodeShader, "viewPos", state.loadedScene->activeCamera->base.transform.position);
    SDL_SetRelativeMouseMode(state.isMouseLocked);
  }
}
unsigned int LoadCubemap(dynlist_t* faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces->size; i++)
    { 
        const char* texture = dynlistAt(faces, i);
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
