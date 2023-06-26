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

	vec4 col;
	vec4 p0 = vec4(0.082, 0.451, 0.929, 1);
	vec4 p1 = vec4(0, 0, 0, 1);
	vec4 p2 = vec4(0.941, 0.537, 0.102, 1);
	vec4 p3 = vec4(1, 1, 1, 1);

	if (0 <= t && t < 0.33) {
		col = mix(p0, p1, 4.0 * t);
	} else if (0.33 <= t && t < 0.66) {
		col = mix(p1, p2, 4.0 * (t-0.33));
	} else {
		col = mix(p2, p3, 4.0 * (t-0.66));
	}

	return col;
}

void main(){
	vec4 pixel = vec4(0, 0, 0, 1);
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	if (pixel_coords.x < u_OutWidth && pixel_coords.y < u_OutHeight) {

		// ivec2 dims = imageSize(img_output);
		vec2 pos;

		float ratio = float(u_OutWidth) / u_OutHeight;

		pos.x = float(pixel_coords.x * 2 - u_OutWidth) * ratio * u_Scale / u_OutWidth + u_Focus.x;
		pos.y = float(pixel_coords.y * 2 - u_OutHeight) * u_Scale  / u_OutHeight + u_Focus.y;

		int i = 0;

		vec2 z = vec2(0, 0);

		while (i < u_MaxIter && z.x * z.x + z.y * z.y < float(1 << 16))
		{
			float new_x = z.x * z.x - z.y * z.y + pos.x;
			z.y = 2 * z.x * z.y + pos.y;
			z.x = new_x;

			// z = squared(z) + pos;

			i+=1;
		}

		if (i < u_MaxIter) {
			// adjust iteration value to mitigate floating point errors
			// source: https://en.wikipedia.org/wiki/Plotting_algorithms_for_the_Mandelbrot_set#Continuous_(smooth)_coloring
			float log_zn = log(z.x*z.x + z.y*z.y) / 2.0;
			float nu = log(log_zn / log(2.0)) / log(2.0);
			float iter = float(i) + 1.0 - nu;

			// sample colours twice to remove banding
			vec4 p1 = getColour(floor(iter));
			vec4 p2 = getColour(floor(iter)+1.0);
			pixel = mix(p1, p2, mod(iter, 1.0));
		}
	}

	imageStore(img_output, pixel_coords, pixel);
}