#version 330
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;


struct Material
{
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct DirLight
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

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
struct SpotLight
{
  vec3 position;
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float cutOff;
  float outerCutOff;
};

uniform Material material;
uniform SpotLight light;

uniform float time;

uniform float mixValue;
uniform vec3 viewPos;

void main()
{

  vec3 lightDir = normalize(light.position - FragPos);
  float  theta = dot(lightDir, normalize(-light.direction));
  float  epsilon = light.cutOff - light.outerCutOff;
  float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
  //ambient
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

  //diffuse;
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm,lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

  //specular
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * vec3(texture(material.specular, TexCoords)) * spec;

  diffuse*= intensity;
  specular *= intensity;
 
  vec3 resColor = ambient + diffuse + specular;
  FragColor = vec4(resColor, 1.0);

  //FragColor = texture(texture1, TexCoords);
}

