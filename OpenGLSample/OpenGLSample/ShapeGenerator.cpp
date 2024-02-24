#include "ShapeData.h"
#include "ShapeGenerator.h"
#include "Vertex.h"
#include <vector>

#define PI 3.14159265359
using glm::vec3;
using glm::mat4;
using glm::mat3;
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

glm::vec3 randomColor()
{
	glm::vec3 ret;
	ret.x = rand() / (float)RAND_MAX;
	ret.y = rand() / (float)RAND_MAX;
	ret.z = rand() / (float)RAND_MAX;
	return ret;
}


ShapeData ShapeGenerator::makePlaneVerts(uint dimensions)
{
	ShapeData ret;
	ret.numVertices = dimensions * dimensions;
	int half = dimensions / 2;
	ret.vertices = new Vertex[ret.numVertices];
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			Vertex& thisVert = ret.vertices[i * dimensions + j];
			thisVert.position.x = j - half;
			thisVert.position.z = i - half;
			thisVert.position.y = 0;
			thisVert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			thisVert.color = randomColor();
		}
	}
	return ret;
}

ShapeData ShapeGenerator::makePlaneIndices(uint dimensions)
{
	ShapeData ret;
	ret.numIndices = (dimensions - 1) * (dimensions - 1) * 2 * 3; // 2 triangles per square, 3 indices per triangle
	ret.indices = new unsigned short[ret.numIndices];
	int runner = 0;
	for (int row = 0; row < dimensions - 1; row++)
	{
		for (int col = 0; col < dimensions - 1; col++)
		{
			ret.indices[runner++] = dimensions * row + col;
			ret.indices[runner++] = dimensions * row + col + dimensions;
			ret.indices[runner++] = dimensions * row + col + dimensions + 1;

			ret.indices[runner++] = dimensions * row + col;
			ret.indices[runner++] = dimensions * row + col + dimensions + 1;
			ret.indices[runner++] = dimensions * row + col + 1;
		}
	}
	assert(runner = ret.numIndices);
	return ret;
}


ShapeData ShapeGenerator::makePlane(uint dimensions)
{
	ShapeData ret = makePlaneVerts(dimensions);
	ShapeData ret2 = makePlaneIndices(dimensions);
	ret.numIndices = ret2.numIndices;
	ret.indices = ret2.indices;
	return ret;
}

ShapeData ShapeGenerator::makeSphere(uint tesselation)
{
	ShapeData ret = makePlaneVerts(tesselation);
	ShapeData ret2 = makePlaneIndices(tesselation);
	ret.indices = ret2.indices;
	ret.numIndices = ret2.numIndices;

	uint dimensions = tesselation;
	const float RADIUS = 1.0f;
	const double CIRCLE = PI * 2;
	const double SLICE_ANGLE = CIRCLE / (dimensions - 1);
	for (size_t col = 0; col < dimensions; col++)
	{
		double phi = -SLICE_ANGLE * col;
		for (size_t row = 0; row < dimensions; row++)
		{
			double theta = -(SLICE_ANGLE / 2.0) * row;
			size_t vertIndex = col * dimensions + row;
			Vertex& v = ret.vertices[vertIndex];
			v.position.x = RADIUS * cos(phi) * sin(theta);
			v.position.y = RADIUS * sin(phi) * sin(theta);
			v.position.z = RADIUS * cos(theta);
			v.normal = glm::normalize(v.position);
		}
	}
	return ret;
}

ShapeData ShapeGenerator::makeCylinder(uint dimensions)
{
	ShapeData ret;

	// Calculate the number of vertices
	ret.numVertices = dimensions * 2 + 2; // Two additional vertices for the top and bottom centers
	ret.vertices = new Vertex[ret.numVertices];

	// Calculate the number of indices
	ret.numIndices = dimensions * 12; // Each segment has 2 triangles for the side and 1 for top and bottom caps
	ret.indices = new GLushort[ret.numIndices];

	// Check if memory allocation succeeded
	if (!ret.vertices || !ret.indices) {
		// Handle allocation failure
		// For simplicity, you can throw an exception or return an empty ShapeData
		throw std::bad_alloc(); // Example: throw an exception for memory allocation failure
	}

	// Calculate the radius and height of the cylinder
	const float RADIUS = 0.5f;
	const float HEIGHT = 1.0f;

	// Calculate the angle between each segment
	float angleIncrement = 2 * PI / dimensions;

	// Generate vertices for the side of the cylinder
	for (uint i = 0; i < dimensions; ++i)
	{
		float x = RADIUS * cos(i * angleIncrement);
		float z = RADIUS * sin(i * angleIncrement);

		// Bottom vertex
		ret.vertices[i * 2].position = glm::vec3(x, -HEIGHT / 2.0f, z);
		ret.vertices[i * 2].normal = glm::normalize(glm::vec3(x, 0, z));
		ret.vertices[i * 2].color = randomColor();

		// Top vertex
		ret.vertices[i * 2 + 1].position = glm::vec3(x, HEIGHT / 2.0f, z);
		ret.vertices[i * 2 + 1].normal = glm::normalize(glm::vec3(x, 0, z));
		ret.vertices[i * 2 + 1].color = randomColor();
	}

	// Add vertices for the top and bottom center points
	ret.vertices[dimensions * 2] = { glm::vec3(0.0f, -HEIGHT / 2.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), randomColor() };
	ret.vertices[dimensions * 2 + 1] = { glm::vec3(0.0f, HEIGHT / 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), randomColor() };

	// Generate indices for the side of the cylinder
	for (uint i = 0; i < dimensions; ++i)
	{
		// Generate indices for the side triangles
		ret.indices[i * 6] = i * 2;
		ret.indices[i * 6 + 1] = (i * 2 + 1) % (dimensions * 2);
		ret.indices[i * 6 + 2] = (i * 2 + 2) % (dimensions * 2);

		ret.indices[i * 6 + 3] = (i * 2 + 2) % (dimensions * 2);
		ret.indices[i * 6 + 4] = (i * 2 + 1) % (dimensions * 2);
		ret.indices[i * 6 + 5] = (i * 2 + 3) % (dimensions * 2);

		// Generate indices for the top cap
		ret.indices[dimensions * 6 + i * 3] = dimensions * 2;
		ret.indices[dimensions * 6 + i * 3 + 1] = (i * 2 + 2) % (dimensions * 2);
		ret.indices[dimensions * 6 + i * 3 + 2] = (i * 2) % (dimensions * 2);

		// Generate indices for the bottom cap
		ret.indices[dimensions * 9 + i * 3] = dimensions * 2 + 1;
		ret.indices[dimensions * 9 + i * 3 + 1] = (i * 2 + 1) % (dimensions * 2);
		ret.indices[dimensions * 9 + i * 3 + 2] = (i * 2 + 3) % (dimensions * 2);
	}

	return ret;
}





