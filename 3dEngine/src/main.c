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

void Update(float deltaTime);
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
static PointLight pointLights[4];
dynlist_t * models;
static shaderStruct modelshader;
static vec3 modelPos;
static vec3 modelPos1;
void Render(ImGuiIO* io, float time)
{ 
  cImGui_ImplOpenGL3_NewFrame();
  cImGui_ImplSDL2_NewFrame();
  ImGui_NewFrame();
  {
    ImGui_Begin("salem", NULL, ImGuiWindowFlags_None);
    ImGui_Text("eto text");
    ImGui_End();
  }
  ImGui_Render();

  glViewport(0, 0,WIDTH , HEIGHT);
  glClearColor(clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4 modelMat;
  mat3 normalMatrix;
  glm_mat4_identity(modelMat);
  glm_translate(modelMat, modelPos); 
  glm_mat4_pick3(modelMat, normalMatrix);
  glm_mat3_inv(normalMatrix, normalMatrix);
  glm_mat3_transpose(normalMatrix);
  UseShader(&modelshader);
  SetShaderMatrix4f(&modelshader, "model", modelMat);
  SetShaderMatrix3f(&modelshader,"normalMatrix", normalMatrix);
  DrawModel((Model*)(dynlistAt(models, 0)), &modelshader);
  glm_translate(modelMat, modelPos1); 
  glm_mat4_pick3(modelMat, normalMatrix);
  glm_mat3_inv(normalMatrix, normalMatrix);
  glm_mat3_transpose(normalMatrix);
  UseShader(&modelshader);
  SetShaderMatrix4f(&modelshader, "model", modelMat);
  SetShaderMatrix3f(&modelshader,"normalMatrix", normalMatrix);
  DrawModel((Model*)(dynlistAt(models, 1)), &modelshader);
  cImGui_ImplOpenGL3_RenderDrawData(ImGui_GetDrawData());
  SDL_GL_SwapWindow(state.window);
}


int main(int argc, char** argv)
{

  models = dynlistInit(sizeof(Model), DEFAULT_INIT_CAPACITY);
  Model backpack;
  Model cube; 
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


  Node* myNode = NodeCreate("dobro node");
  printf("node created named: %s\n", myNode->name);
  SpatialNode* spNode = SpatialNodeCreate("spatial");
  CameraNode* camera = CameraNodeCreate("cam1", 70, (vec3){0, 1, 0}, 0.125f, 100.0f, WIDTH/HEIGHT);
  NodeAddChild(myNode, (Node*)spNode);
  NodeAddChild((Node*)spNode, (Node*)camera);
  char* jsonNode = NodeToJSON(myNode);
  WriteToFile("nodes.json", jsonNode);
  free(jsonNode);

  material.shininess = 2.0f;

  shaderStruct objectshader;
  shaderStruct lightShader;
unsigned int VBO; 

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
  glGenBuffers(1,&VBO);


  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW);



  glBindBuffer(GL_ARRAY_BUFFER,0);
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



  CreateShader(&modelshader, "E:\\projects\\deltaengine\\3dengine\\shaders\\modelvertex.vs", "E:\\projects\\deltaengine\\3dengine\\shaders\\modelfragment.fs");
  
  ModelInit(&backpack, "E:\\projects\\deltaengine\\3dengine\\resources\\models\\backpack\\backpack.obj");
  ModelInit(&cube, "E:\\projects\\deltaengine\\3dengine\\resources\\models\\Cube\\cube.obj");
  dynlistPush(models, &backpack);
  dynlistPush(models, &cube);
  
  stbi_image_free(data);
  UseShader(&objectshader);
  SetShaderInt(&objectshader, "material.diffuse", 0);
  SetShaderInt(&objectshader, "material.specular", 1);
  SetShaderMatrix4f(&objectshader, "projection", camera->projection);
  UseShader(&lightShader);
  SetShaderMatrix4f(&lightShader, "projection", camera->projection);
  UseShader(&modelshader);
  SetShaderMatrix4f(&modelshader, "projection", camera->projection);


  float lastTickTime = 0;
  state.deltaTime = 0;
  while(state.IsGameRuning)
  {
    int i = 0;
    float time = SDL_GetTicks();
    CalcViewMatFromCamera(camera);
    state.deltaTime = (time - lastTickTime) / 1000.0f;
    Update(state.deltaTime);
    SetShaderMatrix4f(&lightShader,"view", camera->view);
    UseShader(&modelshader);
    SetShaderFloat(&modelshader,"time", time);
    SetShaderMatrix4f(&modelshader,"view", camera->view);
    Render(ioptr,time);
   // SDL_Delay(1);
    lastTickTime = time;
  }
  for(int i = 0; i < models->size; i++)
  {
    DeleteModel(dynlistAt(models,i));
  }
  dynlistFree(models);
  NodeDestroy(myNode);
  cImGui_ImplSDL2_Shutdown();
  cImGui_ImplOpenGL3_Shutdown();
  ImGui_DestroyContext(NULL);
  SDL_GL_DeleteContext(state.glContext);
  SDL_Quit();
  return 0;
}



void Update(float deltaTime)
{
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
}

