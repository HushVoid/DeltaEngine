#version 330
out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform float time;

uniform sampler2D texture1;
uniform vec4 myColor;
uniform vec3 lightColor;
uniform float mixValue;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
  vec4 objectColor = mix(texture(texture1, TexCoords), myColor, mixValue); 
  //ambient
  float ambientStrength = 0.1;
  vec3 ambient = ambientStrength * lightColor;

  //diffuse;
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(norm,lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  //specular
  float specularStrength = 0.5f;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor; 
  
  vec4 res = objectColor * vec4(ambient + diffuse + specular, 1.0);
  FragColor = res;
  //FragColor = texture(texture1, TexCoords);
}

