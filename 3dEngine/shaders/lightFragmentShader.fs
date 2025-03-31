#version 330
out vec4 FragColor;

uniform float time;
struct PointLight
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};
uniform int pointLightIndex;
uniform PointLight pointLights[4];
void main()
{
  vec3 lightColor = vec3(0.0);
  lightColor =  pointLights[pointLightIndex].diffuse;
  FragColor = vec4(lightColor, 1.0);
}

