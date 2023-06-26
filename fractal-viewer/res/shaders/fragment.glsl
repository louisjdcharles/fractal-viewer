#version 330 core
    
layout(location = 0) out vec4 colour;

in vec4 gl_FragCoord;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_TexCoord);
    colour = texColor;
}