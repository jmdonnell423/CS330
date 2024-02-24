#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include "ShapeData.h"
#include "Vertex.h"


typedef unsigned int uint;

class ShapeGenerator
{
private:
    static ShapeData makePlaneVerts(uint dimensions);
    static ShapeData makePlaneIndices(uint dimensions);

public:
    static ShapeData makePlane(uint dimensions = 10);
    static ShapeData makeSphere(uint tesselation = 20);
    static ShapeData makeCylinder(uint dimensions = 10); // New method for creating a cylinder
};

