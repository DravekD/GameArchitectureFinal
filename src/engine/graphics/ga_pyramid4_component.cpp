/*
** RPI Game Architecture Engine
**
** Portions adapted from:
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** This file is distributed under the MIT License. See LICENSE.txt.
*/

#include "ga_pyramid4_component.h"
#include "ga_material.h"

#include "entity/ga_entity.h"

#define GLEW_STATIC
#include <GL/glew.h>

ga_pyramid4_component::ga_pyramid4_component(ga_entity* ent, const char* texture_file, GLfloat length, GLfloat height, GLfloat depth) : ga_component(ent)
{
	_material = new ga_unlit_texture_material(texture_file);
	_material->init();


	static GLfloat color[] =
	{
		// Front
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		// Back
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		// Left
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		// Right
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		// Bottom
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
	};
	GLfloat vertices[] = {
		// Front
		(1.0f)*(length),  (-1.0f)*(height), (-1.0f)*(depth),
		(-1.0f)*(length), (-1.0f)*(height), (-1.0f)*(depth),
		0.0f,             (1.0f)*(height),  0.0f,
		// Back
		(1.0f)*(length),  (-1.0f)*(height), (1.0f)*(depth),
		(-1.0f)*(length), (-1.0f)*(height), (1.0f)*(depth),
		0.0f,             (1.0f)*(height),  0.0f,
		// Left
		(-1.0f)*(length), (-1.0f)*(height), (1.0f)*(depth),
		(-1.0f)*(length), (-1.0f)*(height), (-1.0f)*(depth),
		0.0f,             (1.0f)*(height),  0.0f,
		// Right
		(1.0f)*(length),  (-1.0f)*(height), (1.0f)*(depth),
		(1.0f)*(length),  (-1.0f)*(height), (-1.0f)*(depth),
		0.0f,             (1.0f)*(height),  0.0f,
		// Bottom
		(-1.0f)*(length), (-1.0f)*(height), (-1.0f)*(depth),
		(length),         (-1.0f)*(height), (-1.0f)*(depth),
		(length),         (-1.0f)*(height), (depth),
		(-1.0f)*(length), (-1.0f)*(height), (depth),
	};
	static GLfloat texcoords[] = {
		// Front
		0.0, 1.0,
		1.0, 1.0,
		0.5, 0.0,
		// Back
		0.0, 1.0,
		1.0, 1.0,
		0.5, 0.0,
		// Left
		0.0, 1.0,
		1.0, 1.0,
		0.5, 0.0,
		// Right
		0.0, 1.0,
		1.0, 1.0,
		0.5, 0.0,
		// Bottom
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};
	static GLushort indices[] = {
		// Front
		0,  1,  2,
		// Back
		3,  4,  5,
		// Left
		6,  7,  8,
		// Right
		9,  10, 11,
		// Bottom
		12, 13, 14,
		14, 15, 12,
	};

	_index_count = uint32_t(sizeof(indices) / sizeof(*indices));

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(4, _vbos);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, _vbos[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbos[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

ga_pyramid4_component::~ga_pyramid4_component()
{
	glDeleteBuffers(4, _vbos);
	glDeleteVertexArrays(1, &_vao);

	delete _material;
}

void ga_pyramid4_component::update(ga_frame_params* params)
{
	float dt = std::chrono::duration_cast<std::chrono::duration<float>>(params->_delta_time).count();
	ga_quatf axis_angle;
	axis_angle.make_axis_angle(ga_vec3f::y_vector(), ga_degrees_to_radians(60.0f) * dt);
	get_entity()->rotate(axis_angle);

	ga_static_drawcall draw;
	draw._name = "ga_pyramid4_component";
	draw._vao = _vao;
	draw._index_count = _index_count;
	draw._transform = get_entity()->get_transform();
	draw._draw_mode = GL_TRIANGLES;
	draw._material = _material;

	while (params->_static_drawcall_lock.test_and_set(std::memory_order_acquire)) {}
	params->_static_drawcalls.push_back(draw);
	params->_static_drawcall_lock.clear(std::memory_order_release);
}
