attribute vec3 coord3d;
attribute vec3 v_color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform vec3 scale;
varying vec3 f_color;

void main(void) {
	vec3 scaled_vertex = coord3d * scale;
  	gl_Position = proj * view * model * vec4(scaled_vertex, 1.0);
  	f_color = v_color;
}
