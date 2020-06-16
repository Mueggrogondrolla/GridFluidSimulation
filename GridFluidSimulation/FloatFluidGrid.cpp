#include "FloatFluidGrid.h"
#include "GridManager.h"

using namespace std;
using namespace powidl;

FloatFluidGrid::FloatFluidGrid(std::size_t columns, std::size_t rows, float width, float height, float defaultValue, float offsetX, float offsetY, const std::string& keyPath, const std::string& timelineName)
	: UpdatableKeyPlum(keyPath), m_timelineName(timelineName), m_columns(columns), m_rows(rows), m_width(width), m_height(height), m_defaultValue(defaultValue), m_offsetX(offsetX), m_offsetY(offsetY)
{
	InitializeGrid();
}

void FloatFluidGrid::InitializeGrid()
{
	storedValues = std::vector<FloatDataPoint>();

	float cellWidth = m_width / (m_columns - 1);
	float cellHeight = m_height / (m_rows - 1);

	for (size_t y = 0; y < m_rows; y++)
	{
		for (size_t x = 0; x < m_columns; x++)
		{
			storedValues.push_back(FloatDataPoint(Vector3(cellWidth * x+ m_offsetX, cellHeight * y+ m_offsetY, 0), m_defaultValue));
		}
	}
}

std::vector<FloatDataPoint> FloatFluidGrid::GetDataPoints()
{
	return storedValues;
}

FloatDataPoint FloatFluidGrid::GetDataPoint(std::size_t indexX, std::size_t indexY)
{
	return storedValues[GetArrayPositionFromIndices(indexX, indexY)];
}

FloatDataPoint FloatFluidGrid::GetDataPointAtCoordinate(float x, float y)
{
	if (!isInside(x, y))
	{
		return FloatDataPoint(Vector2(x, y), m_defaultValue);
	}

	// Get the four closest data points
	size_t lowerIndexX = GetLowerXIndex(x), upperIndexX = GetUpperXIndex(x);
	size_t lowerIndexY = GetLowerYIndex(y), upperIndexY = GetUpperYIndex(y);

	FloatDataPoint leftUpperDataPoint = GetDataPoint(lowerIndexX, lowerIndexY);
	FloatDataPoint rightUpperDataPoint = GetDataPoint(upperIndexX, lowerIndexY);

	FloatDataPoint leftLowerDataPoint = GetDataPoint(lowerIndexX, upperIndexY);
	FloatDataPoint rightLowerDataPoint = GetDataPoint(upperIndexX, upperIndexY);

	// get the weights for the interpolation based on the coordinates
	float gX = 1.0f * (x - leftLowerDataPoint.GetCoordinates().x) * (m_columns - 1) / m_width;
	float gY = 1.0f * (y - leftLowerDataPoint.GetCoordinates().y) * (m_rows - 1) / m_height;

	// interpolate bilinear between those four vectors
	float interpolationUpper = Interpolate(leftUpperDataPoint.GetValue(), rightUpperDataPoint.GetValue(), gX);
	float interpolationLower = Interpolate(leftLowerDataPoint.GetValue(), rightLowerDataPoint.GetValue(), gX);

	return FloatDataPoint(x, y, Interpolate(interpolationLower, interpolationUpper, gY));
}

FloatDataPoint FloatFluidGrid::GetDataPointAtCoordinate(powidl::Vector3 position)
{
	return GetDataPointAtCoordinate(position.x, position.y);
}

void FloatFluidGrid::onFirstActivation()
{
	// TODO: Add child Plums here...
}

void FloatFluidGrid::onActivation()
{
	// Retrieve timeline used by this Plum
	m_timeline = usePlum<ITimekeeper>().getOrCreateTimeline(m_timelineName);
}

void FloatFluidGrid::onDeactivation()
{
	// Release timeline.
	m_timeline.reset();
}

void FloatFluidGrid::update() {
	// TODO: Place code which gets executed each update cycle...
}







float FloatFluidGrid::Interpolate(float value1, float value2, float g)
{
	return (1 - g) * value1 + g * value2;
}

bool FloatFluidGrid::isInside(float x, float y)
{
	return (x >= m_offsetX - m_width / 2 && x <= m_offsetX + m_width / 2) && (y >= m_offsetY - m_height / 2 && y <= m_offsetY + m_height / 2);
}

void FloatFluidGrid::AdvectValues(float deltaTime, GridManager* gridManager)
{
	StartAdvection();

	for (size_t y = 0; y < m_rows; y++)
	{
		for (size_t x = 0; x < m_columns; x++)
		{
			FloatDataPoint currentValue = storedValues[GetArrayPositionFromIndices(x, y)];
			AdvectValue(x, y, gridManager->GetVelocityAtCoordinate(currentValue.GetCoordinates()), deltaTime); // Forward advection
			//AdvectValue(x, y, velocity, -deltaTime); // Backwards advection
		}
	}
}

void FloatFluidGrid::AdvectValue(size_t indexX, size_t indexY, Vector3 advectionVector, float deltaTime)
{
	FloatDataPoint currentDataPoint = GetDataPoint(indexX, indexY);
	Vector3 newForwardCoordinates = currentDataPoint.GetCoordinates() + advectionVector * deltaTime;
	Vector3 newBackwardCoordinates = currentDataPoint.GetCoordinates() - advectionVector * deltaTime;
	float currentValue = currentDataPoint.GetValue();
	currentDataPoint.SetValue(0);

	float cellWidth = GetCellWidth();
	float cellHeight = GetCellHeight();

	size_t lowerNewXIndex = (size_t)(newForwardCoordinates.x / cellWidth);
	size_t lowerNewYIndex = (size_t)(newForwardCoordinates.y / cellHeight);

	float dx = newForwardCoordinates.x - (lowerNewXIndex * cellWidth);
	float dy = newForwardCoordinates.y - (lowerNewYIndex * cellHeight);

	GetDataPoint(lowerNewXIndex, lowerNewYIndex).AddDifferenceToValue(currentValue * dx * dy);
	GetDataPoint(lowerNewXIndex + 1, lowerNewYIndex).AddDifferenceToValue(currentValue * (1 - dx) * dy);
	GetDataPoint(lowerNewXIndex, lowerNewYIndex + 1).AddDifferenceToValue(currentValue * dx * (1 - dy));
	GetDataPoint(lowerNewXIndex + 1, lowerNewYIndex + 1).AddDifferenceToValue(currentValue * (1 - dx) * (1 - dy));
}

void FloatFluidGrid::StartAdvection()
{
	advectionCopyValues = storedValues; // <-- TDB: does this work? Or do I need to iterate over the list and copy the values by hand?
}




size_t FloatFluidGrid::normalizeIndexX(size_t index)
{
	if (index >= m_columns) { index = m_columns - 1; }
	if (index < 0) { index = 0; }

	return index;
}

size_t FloatFluidGrid::normalizeIndexY(size_t index)
{
	if (index >= m_rows) { index = m_rows - 1; }
	if (index < 0) { index = 0; }

	return index;
}

size_t FloatFluidGrid::GetLowerXIndex(float xCoordinate)
{
	return min((int)m_rows, max(0, (int)(((m_columns - 1) * (xCoordinate + m_width / 2)) / m_width)));
}

size_t FloatFluidGrid::GetUpperXIndex(float xCoordinate)
{
	return GetLowerXIndex(xCoordinate) + 1;
}

size_t FloatFluidGrid::GetLowerYIndex(float yCoordinate)
{
	return min((int)m_rows, max(0, (int)(((m_rows - 1) * (-yCoordinate + m_height / 2)) / m_height)));
}

size_t FloatFluidGrid::GetUpperYIndex(float yCoordinate)
{
	return GetLowerYIndex(yCoordinate) + 1;
}

size_t FloatFluidGrid::GetArrayPositionFromIndices(size_t indexX, size_t indexY)
{
	return normalizeIndexX(indexX) + m_columns * normalizeIndexY(indexY);
}

float FloatFluidGrid::GetCellWidth()
{
	return m_width / m_columns;
}

float FloatFluidGrid::GetCellHeight()
{
	return m_height / m_rows;
}


/**
 *	Getters
*/

std::size_t FloatFluidGrid::getRows()
{
	return m_rows;
}

std::size_t FloatFluidGrid::getColumns()
{
	return m_columns;
}

float FloatFluidGrid::GetWidth()
{
	return m_width;
}

float FloatFluidGrid::GetHeight()
{
	return m_height;
}

float FloatFluidGrid::GetOffsetX()
{
	return m_offsetX;
}

float FloatFluidGrid::GetOffsetY()
{
	return m_offsetY;
}

