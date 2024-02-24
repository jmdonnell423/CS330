#pragma once
#include "Vertex.h"
#include <glm/glm.hpp>
#include <cmath>
#include <glad/glad.h>



struct ShapeData
{
	ShapeData() :
		vertices(0), numVertices(0),
		indices(0), numIndices(0) {}
	Vertex* vertices;
	GLuint numVertices;
	GLushort* indices;
	GLuint numIndices;
	GLsizeiptr vertexBufferSize() const
	{
		return numVertices * sizeof(Vertex);
	}
	GLsizeiptr indexBufferSize() const
	{
		return numIndices * sizeof(GLushort);
	}
	void cleanup()
	{
		delete[] vertices;
		delete[] indices;
		numVertices = numIndices = 0;
	}
};