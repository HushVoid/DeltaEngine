#version 330
out vec4 FragColor;
in vec2 TexCoords;

uniform float time;

uniform sampler2D texture1;

void main()
{
  vec3 color = vec3(sin(time / 100), cos(time/100), sin(time / 100) * cos(time / 100));
  FragColor = mix(texture(texture1, TexCoords), vec4(color,1.0), 0.5);
  //FragColor = texture(texture1, TexCoords);
}

