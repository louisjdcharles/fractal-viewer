#version 430
layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform uint u_MaxIter = 20;
uniform float u_Scale = 10;
uniform float u_ConvThreshold = 0.1;
uniform vec2 u_Focus = vec2(0, 0);

uniform int u_OutWidth;
uniform int u_OutHeight;

uniform vec2 u_Sol1;
uniform vec2 u_Sol2;
uniform vec2 u_Sol3;

// aliases for brevity
vec2 a = u_Sol1;
vec2 b = u_Sol2;
vec2 c = u_Sol3;

float cubed(float n) {
	return n * n * n;
}

float squared(float n) {
	return n * n;
}

// (a + bi) / (c + di) = (ac + bd) / (c^2 + d^2) + i (bc - ad)/(c^2 + d^2)
vec2 ComplexDivide(vec2 n, vec2 m){
	float denom = m.x * m.x + m.y * m.y;
	float re = (n.x * m.x + n.y * m.y) / denom;
	float im = (n.y * m.x - n.x * m.y) / denom;
	
	return vec2(re, im);
}

// (a + bi)^2 = a^2 - b^2 + 2abi
vec2 ComplexSquare(vec2 n){
	return vec2(n.x * n.x - n.y * n.y, 2 * n.x * n.y);
}

// (a + bi)^3 = a^3 - 3ab^2 + i(3a^2b - b^3)
vec2 ComplexCube(vec2 n){
	return vec2(cubed(n.x) - 3 * n.x * squared(n.y), 3 * squared(n.x) * n.y - cubed(n.y));
}

// (a + bi)(c + di) = ac - bd + i(ad + bc)
vec2 ComplexMul(vec2 n, vec2 m){
	return vec2(n.x * m.x - n.y * m.y, n.x * m.y + n.y * m.x);
}

// derivative of cubic
vec2 deriv(vec2 n) {
	return 3 * ComplexSquare(n) - 2 * ComplexMul(a + b + c, n) + ComplexMul(a, b) + ComplexMul(b, c) + ComplexMul(a, c);
}

vec2 func(vec2 n) {
	return ComplexCube(n) - ComplexMul(a + b + c, ComplexSquare(n)) + ComplexMul(ComplexMul(a, b) + ComplexMul(b, c) + ComplexMul(a, c), n) - ComplexMul(ComplexMul(a, b), c);
}

void main(){
	vec4 pixel = vec4(1, 1, 1, 1);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	if (pixel_coords.x < u_OutWidth && pixel_coords.y < u_OutHeight) {

		ivec2 dims = imageSize(img_output);
		vec2 pos;

		float ratio = float(u_OutWidth) / u_OutHeight;

		pos.x = float(pixel_coords.x * 2 - u_OutWidth) * ratio * u_Scale / u_OutWidth + u_Focus.x;
		pos.y = float(pixel_coords.y * 2 - u_OutHeight) * u_Scale  / u_OutHeight + u_Focus.y;

		int i;

		for (i = 0; i < u_MaxIter; i++)
		{
			pos -= ComplexDivide(func(pos), deriv(pos));

			if (abs(pos.x - a.x) < u_ConvThreshold && abs(pos.y - a.y) < u_ConvThreshold)
			{
				pixel = vec4(0.961, 0.498, 0.733, 1);
			} else if (abs(pos.x - b.x) < u_ConvThreshold && abs(pos.y - b.y) < u_ConvThreshold)
			{
				pixel = vec4(0.961, 0.918, 0.592, 1);
			} else if (abs(pos.x - c.x) < u_ConvThreshold && abs(pos.y - c.y) < u_ConvThreshold)
			{
				pixel = vec4(0.690, 0.929, 0.961, 1);
			}
		}
	}

	imageStore(img_output, pixel_coords, pixel);
}