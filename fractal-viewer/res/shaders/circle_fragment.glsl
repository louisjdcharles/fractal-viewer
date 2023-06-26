#version 330 core
    
layout(location = 0) out vec4 colour;

in vec4 gl_FragCoord;
in vec2 v_TexCoord;

uniform vec4 u_CircleColour;

vec4 bordercolour = vec4(0, 0, 0, 1);

void main()
{
    float x = 2 * (v_TexCoord.x - 0.5);
    float y = 2 * (v_TexCoord.y - 0.5);

    float r2 = abs(x * x + y * y);

    if (r2 <= 0.6) {
        colour = u_CircleColour;
    } else if (r2 <= 0.7) {
        colour = mix(u_CircleColour, bordercolour, 10 * (r2 - 0.6));
    } else if (r2 <= 0.9) {
        colour = bordercolour;
    } else if (r2 <= 1.0) {
        colour = mix(bordercolour, vec4(0,0,0,0), 10 * (r2 - 0.9));
    } else {
        colour = vec4(0, 0, 0, 0);
    }
}