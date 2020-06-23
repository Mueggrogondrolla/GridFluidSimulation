#pragma once

#include <vector>
#include <Sge.h>
#include "SimpleVertex.h"
#include "SimpleTriangle.h"

class SimplePlane2DMesh
{
public:
	SimplePlane2DMesh();

	void AddTriangle(SimpleTriangle triangle);
	void AddTriangle(powidl::Vector2 positionVertex1, powidl::Vector2 positionVertex2, powidl::Vector2 positionVertex3);
	void AddTriangle(float vertex1X, float vertex1Y, float vertex2X, float vertex2Y, float vertex3X, float vertex3Y);

	void RemoveTriangle(std::size_t index);

	std::vector<SimpleTriangle> GetTriangles();

	std::vector<float> GetFloatValues();
	std::vector<uint32_t> GetIndices();

	void setColor(powidl::Color color);

private:
	std::vector<SimpleTriangle> m_triangles;
};

