#pragma once

#include <Sge.h>
#include "FluidGridBase.h"

template<class T> class GridBuilder
{
public:
	GridBuilder(powidl::Plum& context);
	~GridBuilder();

	// Dimension of grid to build.
	GridBuilder& width(float width) { m_width = width; return *this; }
	float getWidth() const { return m_width; }
	GridBuilder& height(float height) { m_height = height; return *this; }
	float getHeight() const { return m_height; }

	GridBuilder& offsetX(float offsetX) { m_offsetX = offsetX; return *this; }
	float getOffsetX() const { return m_offsetX; }
	GridBuilder& offsetY(float offsetY) { m_offsetY = offsetY; return *this; }
	float getOffsetY() const { return m_offsetY; }

	// Number of Elements in the grid to build.
	GridBuilder& columns(int columns) { m_columns = columns; return *this; }
	int getColumns() const { return m_columns; }
	GridBuilder& rows(int rows) { m_rows = rows; return *this; }
	int getRows() const { return m_rows; }

	// Actually builds the grid.
	std::shared_ptr<FluidGridBase<T>> build() const;

private:
	// The context for the creation of the grid to find other plums, nodes, ...
	powidl::Plum& m_context;

	// the width of the grid from the most left to the most right point
	float m_width;
	// the height of the grid from the most upper to the most down point
	float m_height;

	// the columns and rows of the grid in x and y
	int m_columns;
	int m_rows;

	float m_offsetX;
	float m_offsetY;
};

