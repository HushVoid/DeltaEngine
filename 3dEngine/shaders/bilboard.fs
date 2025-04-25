#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D iconTexture;
uniform vec4 tintColor;

void main() 
{
    vec4 texColor = texture(iconTexture, TexCoord);
    if(texColor.a < 0.1)
      discard;
    FragColor = texColor * tintColor;
}
