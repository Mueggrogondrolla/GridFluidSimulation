#pragma once

#include <vector>
#include <Sge.h>
#include "SimpleVertex.h"

class SimpleTriangle
{
public:
	SimpleTriangle(SimpleVertex vertex1, SimpleVertex vertex2, SimpleVertex vertex3) : m_vertices{ vertex1, vertex2, vertex3 } {}
	SimpleTriangle(powidl::Vector2 vertexPosition1, powidl::Vector2 vertexPosition2, powidl::Vector2 vertexPosition3) : SimpleTriangle(SimpleVertex(vertexPosition1), SimpleVertex(vertexPosition2), SimpleVertex(vertexPosition3)) {}
	SimpleTriangle(float vertex1X, float vertex1Y, float vertex2X, float vertex2Y, float vertex3X, float vertex3Y) : SimpleTriangle(SimpleVertex(vertex1X, vertex1Y), SimpleVertex(vertex2X, vertex2Y), SimpleVertex(vertex3X, vertex3Y)) {}

	SimpleTriangle& setColor(powidl::Color color)
	{
		m_vertices[0].setColor(color);
		m_vertices[1].setColor(color);
		m_vertices[2].setColor(color);

		return *this;
	}

	SimpleVertex* getVertices()
	{
		doWindingSort();
		return m_vertices;
	}

private:
	SimpleVertex m_vertices[3];
	bool windingOrder = true; // true -> clockwise; false -> counter clockwise

	void doWindingSort()
	{
		SimpleVertex vertexCopies[] = { m_vertices[0], m_vertices[1], m_vertices[2] };

		powidl::Vector2 center = getCenter();

		float angles[3] = { 
			powidl::Vector2(1, 0).angle(m_vertices[0].getPosition() - center), 
			powidl::Vector2(1, 0).angle(m_vertices[1].getPosition() - center), 
			powidl::Vector2(1, 0).angle(m_vertices[2].getPosition() - center) };
		bool vertexTaken[3] = { false };

		for (size_t i = 0; i < 3; i++)
		{
			int currentIndex = !vertexTaken[0] ? 0 : (!vertexTaken[1] ? 1 : 2);

			for (size_t j = 0; j < 3; j++)
			{
				// if the winding order is clockwise, take the highest unused angle, if it is counter clockwise, take the lowest unused angle
				if ((!windingOrder ? angles[j] < angles[currentIndex] : angles[j] > angles[currentIndex]) && !vertexTaken[j])
				{
					currentIndex = j;
				}
			}

			vertexTaken[currentIndex] = true;
			m_vertices[i] = vertexCopies[currentIndex];
		}
	}

	powidl::Vector2 getCenter()
	{
		return powidl::Vector2(
			(m_vertices[0].getPosition().x + m_vertices[1].getPosition().x + m_vertices[2].getPosition().x) / 3,
			(m_vertices[0].getPosition().y + m_vertices[1].getPosition().y + m_vertices[2].getPosition().y) / 3);
	}
};

