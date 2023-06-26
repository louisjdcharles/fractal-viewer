#version 330 core
    
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_Mvp;
uniform float u_XScale;
uniform float u_YScale;

void main()
{
    gl_Position = u_Mvp * vec4(position.x, position.y, 0, 1.0);
    v_TexCoord = vec2(u_XScale * texCoord.x, u_YScale * texCoord.y);
}