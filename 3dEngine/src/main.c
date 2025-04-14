#include "allincludes.h"

#define WIDTH 1600
#define HEIGHT 900

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
static PointLight pointLights[4];
dynlist_t * models;
static shaderStruct modelshader;
static vec3 modelPos;
static vec3 modelPos1;
void Render(shaderStruct*lightShader,ImGuiIO* io, GLuint lightVAO, GLuint* textures, float time)
{ 
  cImGui_ImplOpenGL3_NewFrame();
  cImGui_ImplSDL2_NewFrame();
  ImGui_NewFrame();
 {
      static int counter = 0;
      static int lightIndex = 0;

      ImGui_Begin("Inspecta!", NULL, ImGuiWindowFlags_None);                          // Create a window called "Hello, world!" and append into it.

      ImGui_Text("This is some useful text.");               // Display some text (you can use a format strings too)
      ImGui_Checkbox("Demo Window", &state.showDemoWindow);      // Edit bools storing our window open/close state
      ImGui_Checkbox("Material Window", &state.showAnotherWindow);
      //ImGui_SliderFloat("Mix value", &f, 0, 1);
      ImGui_ColorEdit3("clear color", (float*)&clearColor , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
 //     ImGui_ColorEdit3("cube color", (float*)&meshColor , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      if(ImGui_ArrowButton("plus", ImGuiDir_Up))
      {
        if(lightIndex < 3)
          lightIndex++;
      }
      if(ImGui_ArrowButton("minus", ImGuiDir_Down))
      {
        if(lightIndex > 0)
          lightIndex--;
      }
      ImGui_Text("point light number = %d", lightIndex);
      
      ImGui_InputFloat3("point light position", pointLights[lightIndex].position);
      ImGui_ColorEdit3("point light ambient color", pointLights[lightIndex].ambient , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_ColorEdit3("point light diffuse color", pointLights[lightIndex].diffuse , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_ColorEdit3("point light specular color", pointLights[lightIndex].specular , ImGuiColorEditFlags_None); // Edit 3 floats representing a color
      ImGui_InputFloat3("model position", modelPos);
      ImGui_InputFloat3("model position1", modelPos1);
      ImGui_Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
      ImGui_End();
  }
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

  for(int i = 0; i < 4; i++)
  {  
  InitDefaultPointLight(&pointLights[i], (vec3){0.0, -1 + i,  -i * 2}, 50.0f);
  }


  material.shininess = 2.0f;

  Camera mainCamera;
  shaderStruct objectshader;
  shaderStruct lightShader;
  InitializeCamera(&mainCamera, (vec3){0.0f,0.0f,-3.0f}, (vec3){0.0f, 1.0f, 0.0f}, DEFAULT_CAM_YAW, DEFAULT_CAM_PITCH, 0.1f, 100.f);
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

  CreateShader(&objectshader, "e:\\projects\\deltaengine\\3dengine\\shaders\\defaultvertex.vs","e:\\projects\\deltaengine\\3dengine\\shaders\\defaultfragment.fs");
  CreateShader(&lightShader, "e:\\projects\\deltaengine\\3dengine\\shaders\\lightCubeVertex.vs","e:\\projects\\deltaengine\\3dengine\\shaders\\lightFragmentShader.fs");
  CreateShader(&modelshader, "E:\\projects\\deltaengine\\3dengine\\shaders\\modelvertex.vs", "E:\\projects\\deltaengine\\3dengine\\shaders\\modelfragment.fs");
  
  ModelInit(&backpack, "E:\\projects\\deltaengine\\3dengine\\resources\\models\\backpack\\backpack.obj");
  printf("cubik\n");
  ModelInit(&cube, "E:\\projects\\deltaengine\\3dengine\\resources\\models\\Cube\\cube.obj");
  dynlistPush(models, &backpack);
  dynlistPush(models, &cube);

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
  UseShader(&modelshader);
  SetShaderMatrix4f(&modelshader, "projection", projection);


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
    UseShader(&modelshader);
    SetShaderFloat(&modelshader,"time", time);
    SetShaderMatrix4f(&modelshader,"view", view);
    Render(&lightShader,ioptr,lightVAO,textures,time);
   // SDL_Delay(1);
    lastTickTime = time;
  }
  for(int i = 0; i < models->size; i++)
  {
    DeleteModel(dynlistAt(models,i));
  }
  dynlistFree(models);
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
}

