#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

struct DirLight {
    vec3 direction;
    float intensity;
    vec3 color;
    float padding;
};

struct PointLight {
    vec3 position;      // 12 bytes
    float intensity;    // 4 bytes
    vec3 color; // 12 bytes
    float padding;  // 4 bytes
    float constant;     // 4 bytes
    float linear;       // 4 bytes
    float quadratic;    // 4 bytes
    float padding2;
    vec3 paddingV;// 8 bytes
    int id;             // 4 bytes
    }; 
    
struct SpotLight {
    vec3 direction;     // 12 bytes
    float innerCutOff;  // 4  (16)bytes
    vec3 color;         // 12 bytes
    float outerCutOff;  // 4  (16)bytes
    vec3 position;      // 12 bytes
    float intensity;    // 4 bytes
    vec3 paddingV;    // 12 bytes
    int id;           // 4 bytes
};  

layout (std140) uniform Lights {
    DirLight dirLight;
    PointLight pointLights[10];
    SpotLight spotLights[10];
};

uniform vec3 viewPos;
uniform int activePointLights;
uniform int activeSpotLights;
uniform bool lightEnabled;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
  if(lightEnabled)
  {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.1) * texture(texture_diffuse1, TexCoords).rgb;
    
    result += CalcDirLight(dirLight, norm, viewDir);
    
    for(int i = 0; i < activePointLights && i < 10; i++) {
        if(pointLights[i].intensity > 0.0)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }
    
    for(int i = 0; i < activeSpotLights && i < 10; i++)
    {
        if(spotLights[i].intensity > 0.0)
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0);
    
  }
  else
  {
    FragColor = texture(texture_diffuse1, TexCoords);
  }
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(texture_diffuse1, TexCoords).rgb;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.color * spec * texture(texture_specular1, TexCoords).rgb;
    
    return (diffuse + specular) * light.intensity;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(texture_diffuse1, TexCoords).rgb;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.color * spec * texture(texture_specular1, TexCoords).rgb;
    
    // Затухание
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                             light.quadratic * (distance * distance));
    
    return (diffuse + specular) * attenuation * light.intensity;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.innerCutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texture(texture_diffuse1, TexCoords).rgb;
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = light.color * spec * texture(texture_specular1, TexCoords).rgb;
    
    return (diffuse + specular) * intensity * light.intensity;
}
