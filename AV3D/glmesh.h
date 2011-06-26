#pragma once

#ifndef LMTZ_GLMESH_H
#define LMTZ_GLMESH_H

#include "glvertex.h"

class GLMesh
{
	public:
	GLMesh();
	virtual ~GLMesh();

	virtual void Draw();

	void SetVertexBuffer(GLVertex* vertices, size_t count = 0);
	void SetIndexBuffer(short* indices, size_t count = 0);
	void CalculateNormals();
	void CalculateTangents();
	void Update();

	private:
	GLVertex* _vertices;
	short*	_indices;
	size_t	_vCount, _iCount;
	GLuint	_vbo, _ibo;	
};

#endif