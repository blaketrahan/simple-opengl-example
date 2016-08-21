/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */

char* file_read(const char* filename, int* size);
void print_log(GLuint object);
GLuint create_shader(const char* filename, GLenum type);
GLuint create_program(const char* vertexfile, const char *fragmentfile);
GLint get_attrib(GLuint program, const char *name);
GLint get_uniform(GLuint program, const char *name);
void print_opengl_info();

/**
 * Store all the file's contents in memory, useful to pass shaders
 * source code to OpenGL.  Using SDL_RWops for Android asset support.
 */
char* file_read(const char* filename, int* size) {
	SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
	if (rw == NULL) return NULL;
	
	Sint64 res_size = SDL_RWsize(rw);
	char* res = (char*)malloc(res_size + 1);

	Sint64 nb_read_total = 0, nb_read = 1;
	char* buf = res;
	while (nb_read_total < res_size && nb_read != 0) {
		nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
		nb_read_total += nb_read;
		buf += nb_read;
	}
	SDL_RWclose(rw);
	if (nb_read_total != res_size) {
		free(res);
		return NULL;
	}
	
	res[nb_read_total] = '\0';
	if (size != NULL)
		*size = nb_read_total;
	return res;
}

/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object) {
	GLint log_length = 0;
	if (glIsShader(object)) {
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else if (glIsProgram(object)) {
		glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
	} else {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "printlog: Not a shader or a program");
		return;
	}

	char* log = (char*)malloc(log_length);
	
	if (glIsShader(object))
		glGetShaderInfoLog(object, log_length, NULL, log);
	else if (glIsProgram(object))
		glGetProgramInfoLog(object, log_length, NULL, log);
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s\n", log);
	free(log);
}

/**
 * Compile the shader from file 'filename', with error handling
 */
GLuint create_shader(const char* filename, GLenum type) {
	const GLchar* source = file_read(filename, NULL);
	if (source == NULL) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "Error opening %s: %s", filename, SDL_GetError());
		return 0;
	}
	GLuint res = glCreateShader(type);

	// GLSL version
	const char* version;
	int profile;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
	if (profile == SDL_GL_CONTEXT_PROFILE_ES)
		version = "#version 100\n";  // OpenGL ES 2.0
	else
		version = "#version 120\n";  // OpenGL 2.1

	// GLES2 precision specifiers
	const char* precision;
	precision =
		"#ifdef GL_ES                        \n"
		"#  ifdef GL_FRAGMENT_PRECISION_HIGH \n"
		"     precision highp float;         \n"
		"#  else                             \n"
		"     precision mediump float;       \n"
		"#  endif                            \n"
		"#else                               \n"
		// Ignore unsupported precision specifiers
		"#  define lowp                      \n"
		"#  define mediump                   \n"
		"#  define highp                     \n"
		"#endif                              \n";

	const GLchar* sources[] = {
		version,
		precision,
		source
	};
	glShaderSource(res, 3, sources, NULL);
	free((void*)source);
	
	glCompileShader(res);
	GLint compile_ok = GL_FALSE;
	glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
	if (compile_ok == GL_FALSE) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s:\n", filename);
		print_log(res);
		glDeleteShader(res);
		return 0;
	}
	
	return res;
}

GLuint create_program(const char *vertexfile, const char *fragmentfile) {
	GLuint program = glCreateProgram();
	GLuint shader;

	if(vertexfile) {
		shader = create_shader(vertexfile, GL_VERTEX_SHADER);
		if(!shader)
			return 0;
		glAttachShader(program, shader);
	}

	if(fragmentfile) {
		shader = create_shader(fragmentfile, GL_FRAGMENT_SHADER);
		if(!shader)
			return 0;
		glAttachShader(program, shader);
	}

	glLinkProgram(program);
	GLint link_ok = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "glLinkProgram:");
		print_log(program);
		glDeleteProgram(program);
		return 0;
	}

	return program;
}

GLint get_attrib(GLuint program, const char *name) {
	GLint attribute = glGetAttribLocation(program, name);
	if(attribute == -1)
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "Could not bind attribute %s", name);
	return attribute;
}

GLint get_uniform(GLuint program, const char *name) {
	GLint uniform = glGetUniformLocation(program, name);
	if(uniform == -1)
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR,
					   "Could not bind uniform %s", name);
	return uniform;
}

void print_opengl_info() {
	int major, minor, profile;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &profile);
	const char* profile_str = "";
	if (profile & SDL_GL_CONTEXT_PROFILE_CORE)
		profile_str = "CORE";
	if (profile & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY)
		profile_str = "COMPATIBILITY";
	if (profile & SDL_GL_CONTEXT_PROFILE_ES)
		profile_str = "ES";

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO,
				   "OpenGL %d.%d %s", major, minor, profile_str);
}

b4 create_sgl()
{
	sgl.width = 800;
	sgl.height = 600;
	SDL_Init(SDL_INIT_VIDEO);
	sgl.window = SDL_CreateWindow("Chess",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		sgl.width, sgl.height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (sgl.window == NULL) {
		cerr << "Error: can't create window: " << SDL_GetError() << endl;
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	if (SDL_GL_CreateContext(sgl.window) == NULL) {
		cerr << "Error: SDL_GL_CreateContext: " << SDL_GetError() << endl;
		return false;
	}

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return false;
	}
	if (!GLEW_VERSION_2_0) {
		cerr << "Error: your graphic card does not support OpenGL 2.0" << endl;
		return false;
	}

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void create_cube() {
	GLfloat cube_vertices[] = {
		 -1.0f, -1.0f, 1.0f,//VO - 0
    1.0f, -1.0f, 1.0f,//V1 - 1
    -1.0f, 1.0f, 1.0f,//V2 - 2
    1.0f, 1.0f, 1.0f,//V3 - 3

    -1.0f, -1.0f, -1.0f,//V4 - 4
    1.0f, -1.0f, -1.0f,//V5 - 5
    -1.0f, 1.0f, -1.0f,//V6 - 6
    1.0f, 1.0f, -1.0f,//V7 - 7

    -1.0f, 1.0f, 1.0f,//V2 - 8
    1.0f, 1.0f, 1.0f,//V3 - 9
    -1.0f, 1.0f, -1.0f,//V6 - 10
    1.0f, 1.0f, -1.0f,//V7 - 11

    -1.0f, -1.0f, 1.0f,//VO - 12
    1.0f, -1.0f, 1.0f,//V1 - 13
    -1.0f, -1.0f, -1.0f,//V4 - 14
    1.0f, -1.0f, -1.0f,//V5 - 15

    -1.0f, -1.0f, 1.0f,//VO - 16
    -1.0f, 1.0f, 1.0f,//V2 - 17
    -1.0f, -1.0f, -1.0f,//V4 - 18
    -1.0f, 1.0f, -1.0f,//V6 - 19

    1.0f, -1.0f, 1.0f,//V1 - 20
    1.0f, 1.0f, 1.0f,//V3 - 21
    1.0f, -1.0f, -1.0f,//V5 - 22
    1.0f, 1.0f, -1.0f,//V7 - 23
	};
	glGenBuffers(1, &cube.verts);
	glBindBuffer(GL_ARRAY_BUFFER, cube.verts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	
	GLfloat cube_colors_black[] = {
		// top colors
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		// bottom colors
		0.06, 0.12, 0.15,
		0.06, 0.12, 0.15,
		0.06, 0.12, 0.15,
		0.06, 0.12, 0.15,

		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,

		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,

		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15, };
	glGenBuffers(1, &cube.colors);
	glBindBuffer(GL_ARRAY_BUFFER, cube.colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors_black), cube_colors_black, GL_STATIC_DRAW);
	
	GLushort cube_elements[] = {
    0,1,2, 1,3,2,
    6,7,5, 6,5,4,
    8,9,10, 9,10,11,
    12,13,14, 13,14,15,
    17,16,18, 17,18,19,
    20,21,22, 21,22,23
	};
	glGenBuffers(1, &cube.indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

	GLfloat cube_uv[] = {
		    0.0f, 1.0f,//0
    1.0f, 1.0f,//1
    0.0f, 0.0f,//2
    1.0f, 0.0f,//3

    0.0f, 1.0f,//0
    1.0f, 1.0f,//1
    0.0f, 0.0f,//2
    1.0f, 0.0f,//3

    0.0f, 1.0f,//0
    1.0f, 1.0f,//1
    0.0f, 0.0f,//2
    1.0f, 0.0f,//3

    0.0f, 1.0f,//0
    1.0f, 1.0f,//1
    0.0f, 0.0f,//2
    1.0f, 0.0f,//3

    0.0f, 1.0f,//0
    1.0f, 1.0f,//1
    0.0f, 0.0f,//2
    1.0f, 0.0f,//3

    0.0f, 1.0f,//0
    1.0f, 1.0f,//1
    0.0f, 0.0f,//2
    1.0f, 0.0f,//3
	};
	glGenBuffers(1, &cube.uv_coords);
	glBindBuffer(GL_ARRAY_BUFFER, cube.uv_coords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uv), cube_uv, GL_STATIC_DRAW);
}

void create_plane() {
	GLfloat plane_vertices[] = {
		 -1.0f, -1.0f, 1.0f,//VO - 0
	    1.0f, -1.0f, 1.0f,//V1 - 1
	    -1.0f, 1.0f, 1.0f,//V2 - 2
	    1.0f, 1.0f, 1.0f,//V3 - 3
	};
	glGenBuffers(1, &plane.verts);
	glBindBuffer(GL_ARRAY_BUFFER, plane.verts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
	
	GLfloat plane_colors_black[] = {
		// top colors
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
		0.06, 0.52, 0.15,
	};
	glGenBuffers(1, &plane.colors);
	glBindBuffer(GL_ARRAY_BUFFER, plane.colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_colors_black), plane_colors_black, GL_STATIC_DRAW);
	
	GLushort plane_elements[] = {
    	0,1,2, 1,3,2,
	};
	glGenBuffers(1, &plane.indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_elements), plane_elements, GL_STATIC_DRAW);

	GLfloat plane_uv[] = {
		0.0f, 1.0f,//0
    	1.0f, 1.0f,//1
    	0.0f, 0.0f,//2
    	1.0f, 0.0f,//3
	};
	glGenBuffers(1, &plane.uv_coords);
	glBindBuffer(GL_ARRAY_BUFFER, plane.uv_coords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uv), plane_uv, GL_STATIC_DRAW);
}

void create_pyramid()
{
	GLfloat pyramid_vertices[] = {
		// top
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-0.1, -0.1, -1.0,
		0.1, -0.1, -1.0,
		0.1,  0.1, -1.0,
		-0.1,  0.1, -1.0,
	};

	glGenBuffers(1, &pyramid.verts);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid.verts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);
	
	GLfloat pyramid_colors[] = {
		// top colors
		0.1, 0.85, 0.815,
		0.1, 0.85, 0.815,
		0.1, 0.85, 0.815,
		0.1, 0.85, 0.815,
		// bottom colors
		0.6, 0.99, 0.95,
		0.6, 0.99, 0.95,
		0.6, 0.99, 0.95,
		0.6, 0.99, 0.95 };
	glGenBuffers(1, &pyramid.colors);
	glBindBuffer(GL_ARRAY_BUFFER, pyramid.colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_colors), pyramid_colors, GL_STATIC_DRAW);
	
	GLushort pyramid_elements[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};
	glGenBuffers(1, &pyramid.indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid.indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_elements), pyramid_elements, GL_STATIC_DRAW);

	// GLfloat pyramid_uv[] = {

	// };
	// glGenBuffers(1, &pyramid.uv_coords);
	// glBindBuffer(GL_ARRAY_BUFFER, pyramid.uv_coords);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_uv), pyramid_uv, GL_STATIC_DRAW);
}

b4 create_basic_shader()
{
	GLint link_ok = GL_FALSE;
	
	GLuint vs, fs;
	if ((vs = create_shader("basic.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
	if ((fs = create_shader("basic.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;
	
	basic.program = glCreateProgram();
	glAttachShader(basic.program, vs);
	glAttachShader(basic.program, fs);
	glLinkProgram(basic.program);
	glGetProgramiv(basic.program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "glLinkProgram:";
		print_log(basic.program);
		return false;
	}
	
	basic.attribute_coord3d = get_attrib(basic.program, "coord3d"); 
	
	basic.uniform_scale = get_uniform(basic.program, "scale");
	basic.uniform_model = get_uniform(basic.program, "model");
	basic.uniform_view = get_uniform(basic.program, "view");
	basic.uniform_proj = get_uniform(basic.program, "proj");
	basic.uniform_color = get_uniform(basic.program, "in_color");
	basic.uniform_alpha = get_uniform(basic.program, "in_alpha");
	
	return true;
}

b4 create_basic_texture_shader()
{
	GLint link_ok = GL_FALSE;
	
	GLuint vs, fs;
	if ((vs = create_shader("basic_texture.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
	if ((fs = create_shader("basic_texture.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;
	
	basic_texture.program = glCreateProgram();
	glAttachShader(basic_texture.program, vs);
	glAttachShader(basic_texture.program, fs);
	glLinkProgram(basic_texture.program);
	glGetProgramiv(basic_texture.program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "glLinkProgram:";
		print_log(basic_texture.program);
		return false;
	}
	
	basic_texture.attribute_coord3d = get_attrib(basic_texture.program, "coord3d"); 
	basic_texture.attribute_tex_coord2d = get_attrib(basic_texture.program, "tex_coord2d"); 
	
	basic_texture.uniform_scale = get_uniform(basic_texture.program, "scale");
	basic_texture.uniform_model = get_uniform(basic_texture.program, "model");
	basic_texture.uniform_view = get_uniform(basic_texture.program, "view");
	basic_texture.uniform_proj = get_uniform(basic_texture.program, "proj");
	basic_texture.uniform_tex_source = get_uniform(basic_texture.program, "tex_source");
	
	
	return true;
}

b4 create_color_verts_shader()
{
	GLint link_ok = GL_FALSE;
	
	GLuint vs, fs;
	if ((vs = create_shader("color_verts.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
	if ((fs = create_shader("color_verts.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;
	
	color_verts.program = glCreateProgram();
	glAttachShader(color_verts.program, vs);
	glAttachShader(color_verts.program, fs);
	glLinkProgram(color_verts.program);
	glGetProgramiv(color_verts.program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "glLinkProgram:";
		print_log(color_verts.program);
		return false;
	}
	
	color_verts.attribute_coord3d = get_attrib(color_verts.program, "coord3d"); 
	color_verts.attribute_v_color = get_attrib(color_verts.program, "v_color"); 
	
	color_verts.uniform_scale = get_uniform(color_verts.program, "scale");
	color_verts.uniform_model = get_uniform(color_verts.program, "model");
	color_verts.uniform_view = get_uniform(color_verts.program, "view");
	color_verts.uniform_proj = get_uniform(color_verts.program, "proj");
	
	return true;
}

b4 create_offscreen_shader()
{
	GLint link_ok = GL_FALSE;
	
	GLuint vs, fs;
	if ((vs = create_shader("offscreen.v.glsl", GL_VERTEX_SHADER))   == 0) return false;
	if ((fs = create_shader("offscreen.f.glsl", GL_FRAGMENT_SHADER)) == 0) return false;
	
	offscreen.program = glCreateProgram();
	glAttachShader(offscreen.program, vs);
	glAttachShader(offscreen.program, fs);
	glLinkProgram(offscreen.program);
	glGetProgramiv(offscreen.program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "glLinkProgram:";
		print_log(offscreen.program);
		return false;
	}
	
	offscreen.attribute_coord3d = get_attrib(offscreen.program, "coord3d"); 
	
	offscreen.uniform_scale = get_uniform(offscreen.program, "scale");
	offscreen.uniform_model = get_uniform(offscreen.program, "model");
	offscreen.uniform_view = get_uniform(offscreen.program, "view");
	offscreen.uniform_proj = get_uniform(offscreen.program, "proj");
	offscreen.uniform_color = get_uniform(offscreen.program, "in_color");
	
	return true;
}

GLuint my_create_texture(s4 sw, s4 sh, b4 alpha, unsigned char* image_data = 0, b4 is_render_target = false)
{
	// TODO: figure texture size
	unsigned int width = sw;
	unsigned int height = sh;

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// TODO: are these misplaced?  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (is_render_target)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		return textureID;
	}

	// Give the image to OpenGL
	if (image_data == 0)
	{
		if (alpha) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		}
		else { 
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0); // TODO: test if necessary
		}
	}
	else
	{ 
		if (alpha) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
		}
	}
	return textureID;
}

b4 create_offscreen_texture()
{
	offscreen_texture = 0;
	offscreen_texture = my_create_texture(sgl.width, sgl.height, false, 0, true);
	if (offscreen_texture == 0)
	{
		std::cout << "ERROR: Could not load offscreen_texture." << std::endl;
		return false;
	}

	// int w, h;
	// int miplevel = 0;
	// glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &w);
	// glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &h);

	// generate a framebuffer 
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	
	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offscreen_texture, 0);
	// Set the list of draw buffers.

	// glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	// glDrawBuffers(1, DrawBuffers);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cout << "glCheckFramebufferStatus() ERROR: " << glGetError() << " and " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Set the list of draw buffers.
	// GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	// glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


	return true;
}

void onResize(int width, int height) {
	sgl.width = width;
	sgl.height = height;
	glViewport(0, 0, sgl.width, sgl.height);
}

void empty_program()
{
	glDeleteTextures(1,&offscreen_texture);
	glDeleteTextures(1,&texture_2);
	glDeleteTextures(1,&texture_apple);
	glDeleteProgram(basic.program);
	glDeleteProgram(basic_texture.program);
	glDeleteProgram(color_verts.program);
	glDeleteProgram(offscreen.program);
	glDeleteBuffers(1, &cube.verts);
	glDeleteBuffers(1, &cube.colors);
	glDeleteBuffers(1, &cube.indices);
	glDeleteBuffers(1, &cube.uv_coords);
	glDeleteBuffers(1, &plane.verts);
	glDeleteBuffers(1, &plane.colors);
	glDeleteBuffers(1, &plane.indices);
	glDeleteBuffers(1, &plane.uv_coords);
	glDeleteBuffers(1, &pyramid.verts);
	glDeleteBuffers(1, &pyramid.colors);
	glDeleteBuffers(1, &pyramid.indices);
	// glDeleteBuffers(1, &pyramid.uv_coords);
	SDL_DestroyWindow( sgl.window );
	SDL_Quit();

	free(memory.TransientStorage);
	free(memory.PermanentStorage);
}