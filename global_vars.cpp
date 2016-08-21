#include <iostream>

// eventually remove this
// only using it for shader creation
// switch out vector with a blist
#include <vector> // shader creation
#include <algorithm> // file saving
#include <fstream> // file saving
#include <cstdlib> // i dont know

#include <SDL2/SDL.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "b_memory.h" 
#include "b_list.h"
#include "b_vec.h"

#define DEBUG_BUILD

using namespace std;

inline void b_max(f4 max, f4 &value) {
	if (value > max) {
		value = max;
	}
}

typedef GLuint gu;
typedef GLint gi;

struct OBJ {
	gu verts;
	gu indices;
};

struct BASIC_SHADER {
	gu program;
	gi uniform_model;
	gi uniform_view;
	gi uniform_proj;
	gi uniform_scale;
	gi uniform_color;
	gi uniform_alpha;
	gi attribute_coord3d;
} basic;

struct BASIC_TEXTURE_SHADER {
	gu program;
	gi uniform_model;
	gi uniform_view;
	gi uniform_proj;
	gi uniform_scale;
	gi uniform_tex_source;
	gi attribute_coord3d;
	gi attribute_tex_coord2d;
} basic_texture;

struct COLOR_VERTS_SHADER {
	gu program;
	gi uniform_model;
	gi uniform_view;
	gi uniform_proj;
	gi uniform_scale;
	gi attribute_coord3d;
	gi attribute_v_color;
} color_verts;

struct OFFSCREEN_SHADER {
	gu program;
	gi uniform_model;
	gi uniform_view;
	gi uniform_proj;
	gi uniform_scale;
	gi uniform_color;
	gi attribute_coord3d;
} offscreen;

struct PRIMITIVE
{
	gu verts;
	gu colors;
	gu indices;
	gu uv_coords;
} plane, cube, pyramid; 

struct SGL
{
	SDL_Window* window;
	s4 width;
	s4 height;
} sgl;

struct IMAGE {
	unsigned char* data;
	int x;
	int y;
	int n;
} image_2, image_apple;

gu texture_2;
gu texture_apple;

gu offscreen_texture;
gu FBO;

f4 camera_angle = M_PI32/-2.0f;