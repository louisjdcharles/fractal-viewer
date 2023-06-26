#version 430
layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform uint u_MaxIter = 20;
uniform float u_Scale = 10;
uniform vec2 u_Focus = vec2(0, 0);

uniform int u_OutWidth;
uniform int u_OutHeight;

vec2 squared(vec2 n) {
	return vec2(n.x * n.x - n.y * n.y, 2 * n.x * n.y);
}

vec4 getColour(float iter) {
	float t = mod(iter / u_MaxIter, 0.1) * 10.0;

	vec4 c1 = vec4(0.941, 0.537, 0.102, 1);
	vec4 c2 = vec4(0.082, 0.451, 0.929, 1);
	vec4 c3 = vec4(0, 0, 0, 1);

	vec4 col = (1-t)*(1-t)*(1-t)*c1 + 3.0*(1-t)*(1-t) * t * c2 + t*t*t * c3;

	return col;
}

void main(){
	vec4 pixel = vec4(0, 0, 0, 1);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	if (pixel_coords.x < u_OutWidth && pixel_coords.y < u_OutHeight) {

		ivec2 dims = imageSize(img_output);
		vec2 pos;

		float ratio = float(u_OutWidth) / u_OutHeight;

		pos.x = float(pixel_coords.x * 2 - u_OutWidth) * ratio * u_Scale / u_OutWidth + u_Focus.x;
		pos.y = float(pixel_coords.y * 2 - u_OutHeight) * u_Scale  / u_OutHeight + u_Focus.y;

		int i = 0;

		vec2 z = vec2(0, 0);

		while (i < u_MaxIter && z.x * z.y + z.y * z.y < float(1 << 16))
		{
			z = squared(z) + pos;

			if (abs(z.x) > 2 || abs(z.y) > 2) {
				pixel = vec4(0,0,0,1);
				break;
			}

			i+=1;
		}

		if (i < u_MaxIter) {
			float log_zn = log(z.x*z.x + z.y*z.y) / 2.0;
			float nu = log(log_zn / log(2.0)) / log(2.0);
			float iter = float(i) + 1.0 - nu;

			vec4 p1 = getColour(iter);
			vec4 p2 = getColour(iter+1.0);
			pixel = mix(p1, p2, mod(iter, 1.0));
		}
	}

	imageStore(img_output, pixel_coords, pixel);
}