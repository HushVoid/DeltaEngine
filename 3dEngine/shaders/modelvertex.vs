#version 330
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform float time;


uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
void main()
{
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  TexCoords = aTexCoords;
  FragPos = vec3(model * vec4(aPos, 1.0));
  Normal = normalMatrix * aNormal;
}
