#include "glmesh.h"

GLMesh::GLMesh()
{
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);
}

GLMesh::~GLMesh()
{
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
}

void GLMesh::Draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glDrawElements(GL_TRIANGLE_STRIP, _iCount, GL_UNSIGNED_SHORT, 0);
}

void GLMesh::SetVertexBuffer(GLVertex* vertices, size_t count)
{
	_vertices = vertices;
	_vCount = count;
}
void GLMesh::SetIndexBuffer(short* indices, size_t count)
{
	_indices = indices;
	_iCount = count;
}

void GLMesh::CalculateNormals()
{
	for (unsigned int i = 0; i < _iCount - 2; i++)
	{
		GLVertex p1 = _vertices[_indices[i+0]];
		GLVertex p2 = _vertices[_indices[i+1]];
		GLVertex p3 = _vertices[_indices[i+2]];
		Vector3D normal = (p2.Position - p1.Position) * (p3.Position - p1.Position);
		normal.Normalize();
		if (i%2) normal *= -1;

		_vertices[_indices[i+0]].Normal += normal;
		_vertices[_indices[i+1]].Normal += normal;
		_vertices[_indices[i+2]].Normal += normal;
	}

	for (unsigned int v=0; v< _vCount - 2; v++)
	{
		_vertices[v].Normal.Normalize();
	}
}

void GLMesh::Update()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _iCount * sizeof(short), _indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vCount * GLVertex::Stride(), _vertices, GL_STATIC_DRAW);
	GLVertex::Declare();
}
