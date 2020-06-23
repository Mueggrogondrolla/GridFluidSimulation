#include "SimplePlane2DMesh.h"

using namespace std;
using namespace powidl;

SimplePlane2DMesh::SimplePlane2DMesh() : m_triangles(vector<SimpleTriangle>())
{
}

void SimplePlane2DMesh::AddTriangle(SimpleTriangle triangle)
{
	m_triangles.push_back(triangle);
}

void SimplePlane2DMesh::AddTriangle(powidl::Vector2 positionVertex1, powidl::Vector2 positionVertex2, powidl::Vector2 positionVertex3)
{
	AddTriangle(SimpleTriangle(positionVertex1, positionVertex2, positionVertex3));
}

void SimplePlane2DMesh::AddTriangle(float vertex1X, float vertex1Y, float vertex2X, float vertex2Y, float vertex3X, float vertex3Y)
{
	AddTriangle(SimpleTriangle(vertex1X, vertex1Y, vertex2X, vertex2Y, vertex3X, vertex3Y));
}

void SimplePlane2DMesh::RemoveTriangle(std::size_t index)
{

}

std::vector<SimpleTriangle> SimplePlane2DMesh::GetTriangles()
{
	return m_triangles;
}

std::vector<float> SimplePlane2DMesh::GetFloatValues()
{
	auto dataBuilder = FloatDataBuilder();

	vector<SimpleTriangle>::iterator iterator = m_triangles.begin();

	while (iterator != m_triangles.end())
	{
		SimpleVertex* vertices = (*iterator).getVertices();

		dataBuilder.add(vertices[0].getPosition().x);
		dataBuilder.add(vertices[0].getPosition().y);
		dataBuilder.add(vertices[0].getColor().r);
		dataBuilder.add(vertices[0].getColor().g);
		dataBuilder.add(vertices[0].getColor().b);
		dataBuilder.add(vertices[0].getColor().a);

		dataBuilder.add(vertices[1].getPosition().x);
		dataBuilder.add(vertices[1].getPosition().y);
		dataBuilder.add(vertices[1].getColor().r);
		dataBuilder.add(vertices[1].getColor().g);
		dataBuilder.add(vertices[1].getColor().b);
		dataBuilder.add(vertices[1].getColor().a);

		dataBuilder.add(vertices[2].getPosition().x);
		dataBuilder.add(vertices[2].getPosition().y);
		dataBuilder.add(vertices[2].getColor().r);
		dataBuilder.add(vertices[2].getColor().g);
		dataBuilder.add(vertices[2].getColor().b);
		dataBuilder.add(vertices[2].getColor().a);

		iterator++;
	}

	return dataBuilder.getData();
}

std::vector<uint32_t> SimplePlane2DMesh::GetIndices()
{
	auto dataBuilder = DataBuilder<uint32_t>();

	// because of the winding sorting thing, the triangles should already be added in the right order and we just need to return a continouos counter as indices
	for (size_t i = 0; i < m_triangles.size() * 3; i++) { dataBuilder.add(i); }

	return dataBuilder.getData();
}

void SimplePlane2DMesh::setColor(powidl::Color color)
{
	std::vector<SimpleTriangle>::iterator iterator = m_triangles.begin();

	while (iterator != m_triangles.end())
	{
		(*iterator).setColor(color);
		iterator++;
	}
}
