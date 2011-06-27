#pragma once

/* glvertex.h
 * ---------- 
 * Author: E. Pronk
 * Descr.: Our basic vertex declaration
 */

#ifndef LMTZ_GLVERTEX_H
#define LMTZ_GLVERTEX_H

#include "vector.h"
#include "glinclude.h"

enum VertexAttribute
{
	VA_POSITION,
	VA_NORMAL,
	VA_TANGENT,
	VA_BITANGENT,
	VA_TEXCOORD,
	VA_MAX
};

class GLVertex
{
public:
	Vector3D Position;
	Vector3D Normal;
	Vector3D Tangent;
	Vector3D Bitangent;
	Vector2D TexCoord;
	
	GLVertex(float x, float y, float z, float u, float v)
	{
		Position.X() = x;
		Position.Y() = y;
		Position.Z() = z;
		TexCoord.X() = u;
		TexCoord.Y() = v;
	}

	static unsigned int Stride() { return sizeof(GLVertex); }
	
	static void Declare()
	{
		glVertexAttribPointer(VA_POSITION, 3, GL_FLOAT, GL_FALSE, Stride(), (const void*)0);
		glVertexAttribPointer(VA_NORMAL, 3, GL_FLOAT, GL_FALSE, Stride(), (const void*)(3 * sizeof(float)));
		glVertexAttribPointer(VA_TANGENT, 3, GL_FLOAT, GL_FALSE, Stride(), (const void*)(6 * sizeof(float)));
		glVertexAttribPointer(VA_BITANGENT, 3, GL_FLOAT, GL_FALSE, Stride(), (const void*)(9 * sizeof(float)));
		glVertexAttribPointer(VA_TEXCOORD, 2, GL_FLOAT, GL_FALSE, Stride(),(const void*)(12 * sizeof(float)));

		glEnableVertexAttribArray(VA_POSITION);
		glEnableVertexAttribArray(VA_NORMAL);
		glEnableVertexAttribArray(VA_TEXCOORD);
	}

	static const char* GetAttributeName(VertexAttribute attribute)
	{
		static const char* vertexAttributeNames[] = 
		{
			"_inVertex",
			"_inNormal",
			"_inTangent",
			"_inBitangent",
			"_inTexCoord"
		};
		return vertexAttributeNames[(int)attribute];
	}
};

#endif