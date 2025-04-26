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


  //Lights

  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glGenBuffers(1, &lightUBO);
  glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
  glBufferData(GL_UNIFORM_BUFFER, 
             sizeof(GPUDirLight) + sizeof(GPUPointLight)*10 + sizeof(GPUSpotLight)*10, 
             NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightUBO);

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
  CreateShader(&nodeShader, "E:\\projects\\deltaengine\\3dengine\\shaders\\bilboard.vs", "E:\\projects\\deltaengine\\3dengine\\shaders\\bilboard.fs");
  
  GLuint lightsIndex = glGetUniformBlockIndex(modelshader.ID, "Lights");
  glUniformBlockBinding(modelshader.ID, lightsIndex, 0);

  
  float lastTickTime = 0;
  state.deltaTime = 0;
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
      if(state.isMouseLocked && !state.isPlayingModeOn)
      {
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
      if(state.event.type == SDL_MOUSEMOTION)
      {
        float dx, dy;
        dx = state.event.motion.xrel;
        dy = state.event.motion.yrel;
        if(state.isMouseLocked && !state.isPlayingModeOn) 
          CameraHandleMouse(state.loadedScene->activeCamera, dx, dy, true);
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
