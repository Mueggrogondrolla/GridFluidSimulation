#include "VectorFluidGrid.h"
#include "GridManager.h"
#include <assert.h>

using namespace std;
using namespace powidl;

VectorFluidGrid::VectorFluidGrid(size_t columns, size_t rows, float width, float height, Vector3 defaultValue, float offsetX, float offsetY, const std::string& keyPath, const std::string& timelineName)
	: UpdatableKeyPlum(keyPath), m_timelineName(timelineName), m_columns(columns), m_rows(rows), m_width(width), m_height(height), m_defaultValue(defaultValue), m_offsetX(offsetX), m_offsetY(offsetY)
{
	InitializeGrid();
}

void VectorFluidGrid::InitializeGrid()
{
	storedValues = std::vector<VectorDataPoint>();

	float cellWidth = m_width / (m_columns - 1);
	float cellHeight = m_height / (m_rows - 1);

	for (size_t y = 0; y < m_rows; y++)
	{
		for (size_t x = 0; x < m_columns; x++)
		{
			storedValues.push_back(VectorDataPoint(Vector3(cellWidth * x + m_offsetX, cellHeight * y+ m_offsetY, 0), m_defaultValue));
		}
	}
}

std::vector<VectorDataPoint> VectorFluidGrid::GetDataPoints()
{
	return storedValues;
}

VectorDataPoint& VectorFluidGrid::GetDataPoint(std::size_t indexX, std::size_t indexY)
{
	return storedValues[GetArrayPositionFromIndices(indexX, indexY)];
}

VectorDataPoint VectorFluidGrid::GetDataPointAtCoordinate(float x, float y)
{
	if (!isInside(x, y))
	{
		return VectorDataPoint(Vector2(x, y), m_defaultValue);
	}

	// Get the four closest data points
	size_t lowerIndexX = GetLowerXIndex(x), upperIndexX = GetUpperXIndex(x);
	size_t lowerIndexY = GetLowerYIndex(y), upperIndexY = GetUpperYIndex(y);

	VectorDataPoint leftUpperDataPoint = GetDataPoint(lowerIndexX, lowerIndexY);
	VectorDataPoint rightUpperDataPoint = GetDataPoint(upperIndexX, lowerIndexY);

	VectorDataPoint leftLowerDataPoint = GetDataPoint(lowerIndexX, upperIndexY);
	VectorDataPoint rightLowerDataPoint = GetDataPoint(upperIndexX, upperIndexY);

	// get the weights for the interpolation based on the coordinates
	float gX = 1.0f * (x - leftLowerDataPoint.GetCoordinates().x) * (m_columns - 1) / m_width;
	float gY = 1.0f * (y - leftLowerDataPoint.GetCoordinates().y) * (m_rows - 1) / m_height;

	// interpolate bilinear between those four vectors
	Vector3 interpolationUpper = Interpolate(leftUpperDataPoint.GetValue(), rightUpperDataPoint.GetValue(), gX);
	Vector3 interpolationLower = Interpolate(leftLowerDataPoint.GetValue(), rightLowerDataPoint.GetValue(), gX);

	return VectorDataPoint(x, y, Interpolate(interpolationLower, interpolationUpper, gY));
}

VectorDataPoint VectorFluidGrid::GetDataPointAtCoordinate(powidl::Vector3 position)
{
	return GetDataPointAtCoordinate(position.x, position.y);
}

void VectorFluidGrid::onFirstActivation()
{
	// TODO: Add child Plums here...
}

void VectorFluidGrid::onActivation()
{
	// Retrieve timeline used by this Plum
	m_timeline = usePlum<ITimekeeper>().getOrCreateTimeline(m_timelineName);
}

void VectorFluidGrid::onDeactivation()
{
	// Release timeline.
	m_timeline.reset();
}

void VectorFluidGrid::update() {
	// TODO: Place code which gets executed each update cycle...
}







Vector3 VectorFluidGrid::Interpolate(Vector3 value1, Vector3 value2, float g)
{
	return (1 - g) * value1 + g * value2;
}

bool VectorFluidGrid::isInside(float x, float y)
{
	return (x >= m_offsetX - m_width / 2 && x <= m_offsetX + m_width / 2) && (y >= m_offsetY - m_height / 2 && y <= m_offsetY + m_height / 2);
}

void VectorFluidGrid::AdvectValues(float deltaTime, GridManager* gridManager)
{
	StartAdvection();

	//Logger::logDebug("Advecting values");

	for (size_t y = 0; y < m_rows; y++)
	{
		for (size_t x = 0; x < m_columns; x++)
		{
			VectorDataPoint currentValue = storedValues[GetArrayPositionFromIndices(x, y)];
			Vector3 velocity = gridManager->GetVelocityAtCoordinate(currentValue.GetCoordinates());

			AdvectValue(x, y, velocity, deltaTime); // Forward advection
			//AdvectValue(x, y, velocity, -deltaTime); // Backwards advection

			//GetDataPoint(x, y).SetValue(Vector3(0, 0, 0));
		}
	}

	EndAdvection();
}

void VectorFluidGrid::AdvectValue(size_t indexX, size_t indexY, Vector3 advectionVector, float deltaTime)
{
	VectorDataPoint currentDataPoint = GetDataPoint(indexX, indexY);
	Vector3 newForwardCoordinates = currentDataPoint.GetCoordinates() + advectionVector * deltaTime;
	Vector3 currentValue = currentDataPoint.GetValue();
	currentDataPoint.SetValue(Vector3(0, 0, 0));

	float cellWidth = GetCellWidth();
	float cellHeight = GetCellHeight();

	size_t lowerNewXIndex = GetLowerXIndex(newForwardCoordinates.x);
	size_t lowerNewYIndex = GetLowerYIndex(newForwardCoordinates.y);

	float dx = (newForwardCoordinates.x - ((lowerNewXIndex * cellWidth) + m_offsetX)) / cellWidth;
	float dy = (newForwardCoordinates.y - ((lowerNewYIndex * cellHeight) + m_offsetY)) / cellHeight;

	assert(!(dx > 1 || dx < 0));
	assert(!(dy > 1 || dy < 0));

	advectionCopyValues[GetArrayPositionFromIndices(lowerNewXIndex, lowerNewYIndex)].AddToValue(currentValue * (dx * dy - dx - dy));
	advectionCopyValues[GetArrayPositionFromIndices(lowerNewXIndex + 1, lowerNewYIndex)].AddToValue(currentValue * dx * (1 - dy));
	advectionCopyValues[GetArrayPositionFromIndices(lowerNewXIndex, lowerNewYIndex + 1)].AddToValue(currentValue * (1 - dx) * dy);
	advectionCopyValues[GetArrayPositionFromIndices(lowerNewXIndex + 1, lowerNewYIndex + 1)].AddToValue(currentValue * dx * dy);
}

void VectorFluidGrid::StartAdvection()
{
	advectionCopyValues = storedValues;
}

void VectorFluidGrid::EndAdvection()
{
	for (size_t y = 0; y < m_rows; y++)
	{
		for (size_t x = 0; x < m_columns; x++)
		{
			GetDataPoint(x, y).SetValue(advectionCopyValues[GetArrayPositionFromIndices(x, y)].GetValue());
		}
	}
}




size_t VectorFluidGrid::normalizeIndexX(size_t index)
{
	if (index >= m_columns) { index = m_columns - 1; }
	if (index < 0) { index = 0; }

	return index;
}

size_t VectorFluidGrid::normalizeIndexY(size_t index)
{
	if (index >= m_rows) { index = m_rows - 1; }
	if (index < 0) { index = 0; }

	return index;
}

size_t VectorFluidGrid::GetLowerXIndex(float xCoordinate)
{
	return min((int)m_columns, max(0, (int)(((m_columns - 1) * (xCoordinate - m_offsetX)) / m_width)));
}

size_t VectorFluidGrid::GetUpperXIndex(float xCoordinate)
{
	return GetLowerXIndex(xCoordinate) + 1;
}

size_t VectorFluidGrid::GetLowerYIndex(float yCoordinate)
{
	return min((int)m_rows, max(0, (int)(((m_rows - 1) * (yCoordinate - m_offsetY)) / m_height)));
}

size_t VectorFluidGrid::GetUpperYIndex(float yCoordinate)
{
	return GetLowerYIndex(yCoordinate) + 1;
}

size_t VectorFluidGrid::GetArrayPositionFromIndices(size_t indexX, size_t indexY)
{
	return normalizeIndexX(indexX) + m_columns * normalizeIndexY(indexY);
}

float VectorFluidGrid::GetCellWidth()
{
	return m_width / (m_columns - 1);
}

float VectorFluidGrid::GetCellHeight()
{
	return m_height / (m_rows - 1);
}


/**
 *	Getters
*/

std::size_t VectorFluidGrid::getRows()
{
	return m_rows;
}

std::size_t VectorFluidGrid::getColumns()
{
	return m_columns;
}

float VectorFluidGrid::GetWidth()
{
	return m_width;
}

float VectorFluidGrid::GetHeight()
{
	return m_height;
}

float VectorFluidGrid::GetOffsetX()
{
	return m_offsetX;
}

float VectorFluidGrid::GetOffsetY()
{
	return m_offsetY;
}

