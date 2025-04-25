#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 position;
uniform vec3 viewPos;
uniform float size;    
void main() {
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up = vec3(view[0][1], view[1][1], view[2][1]);
    float sizeD = size * (length(position - viewPos) / 5) ;
    vec3 pos = position + right * aPos.x * sizeD + up * aPos.y * sizeD;
    
    gl_Position = projection * view * vec4(pos, 1.0);
    TexCoord = aTexCoord;
}
