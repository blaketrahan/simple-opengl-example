void render_hand(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection, vec3 scale)
{
	glUseProgram(color_verts.program);
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

void render_chessboard(glm::mat4 &view, glm::mat4 &projection)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, cube.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,TILE_WIDTH,TILE_WIDTH,TILE_DEPTH);

	s4 index = 0;
	for (s4 i = 0; i < board->num_down; i++)
	{
		for (s4 k = 0; k < board->num_across ; k++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f),
				glm::vec3(	x(k) + board->render_info[index].offset.x,
							y(i) + board->render_info[index].offset.y,
							board->render_info[index].offset.z)
				);
			glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
			glUniform3f(basic.uniform_color,board->render_info[index].color.r,board->render_info[index].color.g,board->render_info[index].color.b);
			glUniform1f(basic.uniform_alpha, board->render_info[index].color.a);

			glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
			
			index++;
		}
	}

	glDisableVertexAttribArray(basic.attribute_coord3d);
}

void render_chessboard_offscreen(glm::mat4 &view, glm::mat4 &projection)
{
	/* render offscreen */
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);
	glClearColor(0.0f,0.0f,0.0f,1.0f); 
	glClear(GL_COLOR_BUFFER_BIT); // no depth included in texture

	glEnableVertexAttribArray(offscreen.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, cube.verts);
	glVertexAttribPointer( offscreen.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(offscreen.program);
	glUniformMatrix4fv(offscreen.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(offscreen.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(offscreen.uniform_scale,TILE_WIDTH,TILE_WIDTH,0.0001f); // 0.0001f so its flat, bc no depth testing yet

	f4 color = 0.0f;
	for (s4 i = 0; i < board->num_down; i++)
	{
		for (s4 k = 0; k < board->num_across ; k++)
		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x(k),y(i),TILE_DEPTH));
			color += 1.0f/256.0f;
			
			glUniformMatrix4fv(offscreen.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1f(offscreen.uniform_color,color);

			glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		} 
	}

	glDisableVertexAttribArray(offscreen.attribute_coord3d);
}

void render_player_offscreen(glm::mat4 &view, glm::mat4 &projection, OBJ &obj, f4 x, f4 y, s4 id)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, obj.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,0.3f,0.3f,0.3f);
	glUniform1f(basic.uniform_alpha, 1.0f);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,TILE_DEPTH));

	glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
	f4 green = (1.0f/256.0f) * id;
	glUniform3f(basic.uniform_color,0.0f,green, 1.0f);

	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(basic.attribute_coord3d);
}


void render_piece(glm::mat4 &view, glm::mat4 &projection, OBJ &obj, f4 x, f4 y)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, obj.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,0.3f,0.3f,0.3f);
	glUniform1f(basic.uniform_alpha, 1.0f);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,TILE_DEPTH));

	glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(basic.uniform_color,1.0f, 1.0f, 1.0f);

	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(basic.attribute_coord3d);
}

void render_player(glm::mat4 &view, glm::mat4 &projection, OBJ &obj, f4 x, f4 y, s4 id, b4 alive)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, obj.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,0.3f,0.3f,0.3f);

	glm::mat4 model;
	if (alive) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,TILE_DEPTH));
	} else {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4,0,TILE_DEPTH));
	}

	glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
	
	if (user.active == 1 && user.active_id == id)
		glUniform3f(basic.uniform_color,0.3f, 0.9f, 0.9f);
	else
		glUniform3f(basic.uniform_color,0.0f, 0.0f, 0.0f);

	glUniform1f(basic.uniform_alpha, 1.0f);

	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(basic.attribute_coord3d);
}

void render_art(glm::mat4 &view, glm::mat4 &projection, ART_INFO art)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, art.mesh.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, art.mesh.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,0.3f,0.3f,0.3f);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x(art.pos.x),y(art.pos.y),TILE_DEPTH));

	glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(basic.uniform_color,art.color.r, art.color.g, art.color.b); 
	glUniform1f(basic.uniform_alpha, art.color.a);

	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(basic.attribute_coord3d);
}

void render_highlights(glm::mat4 &view, glm::mat4 &projection, vec2* available_pts, s4 num_pts)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, cube.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,TILE_WIDTH,TILE_WIDTH,TILE_DEPTH/2.0f);
 	glUniform1f(basic.uniform_alpha, 0.7f);
 	glUniform3f(basic.uniform_color,0.9f, 0.12f, 0.25f);
	for (s4 i = 0; i < num_pts; i++)
	{ 
		if (available_pts[i].x < 0 || available_pts[i].y < 0) { continue; }

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x(available_pts[i].x),y(available_pts[i].y),0.2));

		glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));

		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	} 
}

void render_board_state(glm::mat4 &view, glm::mat4 &projection, f4 x, f4 y, f4 r, f4 g, f4 b, f4 offset = 0.0f)
{
	glEnableVertexAttribArray(basic.attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, cube.verts);
	glVertexAttribPointer( basic.attribute_coord3d, 3, GL_FLOAT, GL_FALSE, 0, 0 );

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.indices);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

	glUseProgram(basic.program);
	glUniformMatrix4fv(basic.uniform_view, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(basic.uniform_proj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3f(basic.uniform_scale,TILE_WIDTH,TILE_WIDTH,TILE_DEPTH/2.0f);
 	glUniform1f(basic.uniform_alpha, 0.7f);
 	glUniform3f(basic.uniform_color,r,g,b);
	
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x,y,0.4 + offset));

	glUniformMatrix4fv(basic.uniform_model, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

}