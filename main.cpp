/*
	-Blake Trahan
	-Cuber
	-2016
*/
#include "global_vars.cpp"
#include "sgl_functions.cpp"
#include "input.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void _RENDER_NORMAL(glm::mat4 &view, glm::mat4 &projection, vec3 scale)
{
	{
		glBindTexture(GL_TEXTURE_2D, texture_2);
		glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f, 0.0f));
		glUseProgram(basic_texture.program);
		glUniformMatrix4fv(basic_texture.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(basic_texture.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(basic_texture.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(basic_texture.uniform_scale,scale.x,scale.y,scale.z);

		glEnableVertexAttribArray(basic_texture.attribute_coord3d);
		glBindBuffer(GL_ARRAY_BUFFER, plane.verts);
		glVertexAttribPointer( basic_texture.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		glEnableVertexAttribArray(basic_texture.attribute_tex_coord2d);
		glBindBuffer(GL_ARRAY_BUFFER, plane.uv_coords);
		glVertexAttribPointer( basic_texture.attribute_tex_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0 );

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane.indices);
		int size; 
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(basic_texture.attribute_coord3d);
		glDisableVertexAttribArray(basic_texture.attribute_tex_coord2d);
	}
	{
		glBindTexture(GL_TEXTURE_2D, texture_apple);
		glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(2.5f,0.0f, 0.0f));
		glUseProgram(basic_texture.program);
		glUniformMatrix4fv(basic_texture.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(basic_texture.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(basic_texture.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(basic_texture.uniform_scale,scale.x,scale.y,scale.z);

		glEnableVertexAttribArray(basic_texture.attribute_coord3d);
		glBindBuffer(GL_ARRAY_BUFFER, cube.verts);
		glVertexAttribPointer( basic_texture.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		glEnableVertexAttribArray(basic_texture.attribute_tex_coord2d);
		glBindBuffer(GL_ARRAY_BUFFER, cube.uv_coords);
		glVertexAttribPointer( basic_texture.attribute_tex_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0 );

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.indices);
		int size; 
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(basic_texture.attribute_coord3d);
		glDisableVertexAttribArray(basic_texture.attribute_tex_coord2d);
	}
	{
		glUseProgram(color_verts.program);
		glm::mat4 model = glm::translate(glm::mat4(1.0f),glm::vec3(-2.5f,0.0f, 0.0f));
		glUniformMatrix4fv(color_verts.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(color_verts.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(color_verts.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3f(color_verts.uniform_scale,scale.x,scale.y,scale.z);

		glEnableVertexAttribArray(color_verts.attribute_coord3d);
		glBindBuffer(GL_ARRAY_BUFFER, pyramid.verts);
		glVertexAttribPointer( color_verts.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		glEnableVertexAttribArray(color_verts.attribute_v_color);
		glBindBuffer(GL_ARRAY_BUFFER, pyramid.colors);
		glVertexAttribPointer( color_verts.attribute_v_color, 3, GL_FLOAT, GL_FALSE, 0, 0 );

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid.indices);
		int size; 
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(color_verts.attribute_coord3d);
		glDisableVertexAttribArray(color_verts.attribute_v_color);
	}
}

int main(int argc, char* argv[]) { 
	initialize_memory(memory, 8); // 10 megabytes
	
	if (!create_sgl()) { cout << "ERROR: failed to create sdl or opengl" << endl; }
	create_plane();
	create_cube();
	create_pyramid();
	if (!create_basic_shader()) return false;
	if (!create_offscreen_shader()) return false;
	if (!create_color_verts_shader()) return false;
	if (!create_basic_texture_shader()) return false;

	if (!create_offscreen_texture()) return false;

	texture_2 = 0;
	image_2.data = stbi_load("test_2.png", &image_2.x, &image_2.y, &image_2.n, 0);
	texture_2 = my_create_texture(256,256,true,image_2.data,false);

	texture_apple = 0;
	image_apple.data = stbi_load("test_apple.png", &image_apple.x, &image_apple.y, &image_apple.n, 3);
	texture_apple = my_create_texture(256,256,false,image_apple.data,false);

	f4 prev_camera_angle = -1;
	b4 is_screen_dirty = true;
	const f4 RENDER_MS = 1.0f/60.0f;
	const f4 PHYSICS_MS = 1.0f/60.0f;
	f4 render_dt = 0.0f;
	f4 physics_dt = 0.0f;
	u4 time_physics_prev = SDL_GetTicks();

	while(!input.quit_app)
	{
		u4 time_physics_curr = SDL_GetTicks();
		f4 frameTime = ((f4)(time_physics_curr - time_physics_prev)) / 1000.0f;
		time_physics_prev = time_physics_curr;

		poll_events();

		physics_dt += frameTime;
		if (physics_dt >= PHYSICS_MS)
		{
			physics_dt -= PHYSICS_MS;

			input.mouse_clicked = false;
			input.mouse_right_clicked = false;
		}

		render_dt += frameTime;
		if (render_dt >= RENDER_MS )
		{
			render_dt = 0;
		
			glm::vec3 up_axis(0, 0, 1); 

			f4 posX = 7.0f * cos(camera_angle); 
    		f4 posY = 7.0f * sin(camera_angle); 
    		vec3 pp(posX, posY, 6.5f); // Create a positional vector with X=0
			
			glm::mat4 view = glm::lookAt(glm::vec3(pp.x,pp.y,pp.z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
			glm::mat4 projection = glm::perspective(45.0f, 1.0f*sgl.width/sgl.height, 0.1f, 100.0f);

			glBindFramebuffer(GL_FRAMEBUFFER, FBO);
			if (prev_camera_angle != camera_angle || is_screen_dirty)
			{
				prev_camera_angle = camera_angle;
				// _RENDER_OFFSCREEN();
				is_screen_dirty = false;
			}

			// get_mouse_state();

		   	glBindFramebuffer(GL_FRAMEBUFFER,0);
			glClearColor(0.23f,0.47f,0.58f,1.0f); 
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_RENDER_NORMAL(view,projection,vec3(1.0f,1.0f,1.0f));

			SDL_GL_SwapWindow(sgl.window);
		}

		memory.transient_current = 0;
	}
	stbi_image_free(image_2.data);
	stbi_image_free(image_apple.data);
	glDeleteTextures(1,&texture_2);
	glDeleteTextures(1,&texture_apple);
	empty_program();
	return 0;
}