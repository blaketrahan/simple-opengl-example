attribute vec3 coord3d;
uniform vec3 in_color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 scale;
varying vec3 f_color;

attribute vec2 tex_coord2d;
uniform sampler2D tex_source;

varying vec2 UV;

void main(void) {
	vec3 scaled_vertex = coord3d * scale;
  	
  	gl_Position = proj * view * model * vec4(scaled_vertex, 1.0);

  	UV = tex_coord2d;
}
