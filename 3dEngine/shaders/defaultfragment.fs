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

#define NR_POINT_LIGHTS 4 
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;

uniform float time;

uniform float mixValue;
uniform vec3 viewPos;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
  vec3 lightDir = normalize(-light.direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);
  // specular shading
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  // combine results
  vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
  vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
  return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
      vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
  vec3 lightDir = normalize(light.position - fragPos);
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
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * vec3(texture(material.specular, TexCoords)) * spec;

  diffuse*= intensity;
  specular*= intensity;
 
  vec3 resColor = ambient + diffuse + specular;
  return resColor;
}

void main()
{

  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 result = vec3(0.0);
 // vec3 result  = CalcDirLight(dirLight, norm, viewDir);
 
 for(int i = 0; i < NR_POINT_LIGHTS; i++)
 {
   result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
 }
 result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
  FragColor = vec4(result, 1.0);

  //FragColor = texture(texture1, TexCoords);
}

