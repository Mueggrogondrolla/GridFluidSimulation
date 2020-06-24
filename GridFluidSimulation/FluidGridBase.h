#pragma once

#include <string>
#include <memory>
#include <assert.h>
#include <Sge.h>
#include "EmptyDataPoint.h"
#include "GridManager.h"

using namespace std;

template<class T> class FluidGridBase : public powidl::UpdatableKeyPlum {
public:

#define ValueAtPosition(arrayToLookFor, x, y) (arrayToLookFor[GetArrayPositionFromIndices(x, y)])

	/**
	 * Constructor.
	 *
	 * @param keyPath		the path to the data depot
	 * @param timelineName	the name of the timeline to use
	 */
	FluidGridBase(std::size_t columns, std::size_t rows, float width, float height, T defaultValue, T zeroValue, GridManager* gridManager, float offsetX = 0, float offsetY = 0, bool isVeloctiyGrid = false, powidl::Vector3 velocityDirection = powidl::Vector3(0, 0, 0), const std::string& keyPath = "", const std::string& timelineName = powidl::ITimekeeper::DEFAULT_TIMELINE_NAME)
		: UpdatableKeyPlum(keyPath), m_timelineName(timelineName), m_columns(columns), m_rows(rows), m_width(width), m_height(height), m_defaultValue(defaultValue), m_zeroValue(zeroValue), m_offsetX(offsetX), m_offsetY(offsetY), gridManager(gridManager), isVeloctiyGrid(isVeloctiyGrid), velocityDirection(velocityDirection)
	{
		InitializeGrid();
	}

	// Inherited via Plum
	virtual void onFirstActivation() override {}
	virtual void onActivation() override { m_timeline = usePlum<powidl::ITimekeeper>().getOrCreateTimeline(m_timelineName); }
	virtual void onDeactivation() override { m_timeline.reset(); }
	virtual void update() override {}

	void InitializeGrid()
	{
		storedValues = std::vector<EmptyDataPoint<T>>();

		float cellWidth = m_width / (m_columns - 1);
		float cellHeight = m_height / (m_rows - 1);

		for (size_t y = 0; y < m_rows; y++)
		{
			for (size_t x = 0; x < m_columns; x++)
			{
				storedValues.push_back(EmptyDataPoint<T>(powidl::Vector3(x * cellWidth + m_offsetX, y * cellHeight + m_offsetY, 0), ((x == m_columns / 2 && y == m_rows / 2) || true) ? m_defaultValue : m_zeroValue));
			}
		}
	}

	std::vector<EmptyDataPoint<T>> GetDataPoints() { return storedValues; }

	EmptyDataPoint<T>& GetDataPoint(std::size_t indexX, std::size_t indexY) { return ValueAtPosition(storedValues, indexX, indexY); }

	EmptyDataPoint<T> GetDataPointAtCoordinate(float x, float y)
	{
		if (!isInside(x, y))
		{
			return EmptyDataPoint<T>(powidl::Vector2(x, y), m_defaultValue);
		}

		// Get the four closest data points
		size_t lowerIndexX = GetLowerXIndex(x), upperIndexX = lowerIndexX + 1;
		size_t lowerIndexY = GetLowerYIndex(y), upperIndexY = lowerIndexY + 1;

		EmptyDataPoint<T> leftLowerDataPoint = GetDataPoint(lowerIndexX, lowerIndexY);
		EmptyDataPoint<T> rightLowerDataPoint = GetDataPoint(upperIndexX, lowerIndexY);

		EmptyDataPoint<T> leftUpperDataPoint = GetDataPoint(lowerIndexX, upperIndexY);
		EmptyDataPoint<T> rightUpperDataPoint = GetDataPoint(upperIndexX, upperIndexY);

		// get the weights for the interpolation based on the coordinates
		float gX = 1.0f * (x - leftLowerDataPoint.GetCoordinates().x) * (m_columns - 1) / m_width;
		float gY = 1.0f * (y - leftLowerDataPoint.GetCoordinates().y) * (m_rows - 1) / m_height;

		// interpolate bilinear between those four vectors
		T interpolationUpper = Interpolate(leftUpperDataPoint.GetValue(), rightUpperDataPoint.GetValue(), gX);
		T interpolationLower = Interpolate(leftLowerDataPoint.GetValue(), rightLowerDataPoint.GetValue(), gX);

		return EmptyDataPoint<T>(x, y, Interpolate(interpolationLower, interpolationUpper, gY));
	}
	EmptyDataPoint<T> GetDataPointAtCoordinate(powidl::Vector3 position) { return GetDataPointAtCoordinate(position.x, position.y); }


	std::size_t getRows() { return m_rows; }
	std::size_t getColumns() { return m_columns; }

	float GetWidth() { return m_width; }
	float GetHeight() { return m_height; }
	float GetOffsetX() { return m_offsetX; }
	float GetOffsetY() { return m_offsetY; }

	void StartUpdate(float deltaTime) { newValues = storedValues; lastDeltaTime = deltaTime; }
	void EndUpdate()
	{
		for (std::size_t y = 0; y < m_rows; y++)
		{
			for (std::size_t x = 0; x < m_columns; x++)
			{
				bool isBorderCell = x == 0 || x == m_columns - 1 || y == 0 || y == m_rows - 1;
				GetDataPoint(x, y).SetValue(ValueAtPosition(newValues, x, y).GetValue() * (1 - m_dissipationFactor) * (isBorderCell ? (1 - m_dissipationFactor) : 1));
			}
		}
	}

	void AdvectValues()
	{
		for (std::size_t y = 0; y < m_rows; y++)
		{
			for (std::size_t x = 0; x < m_columns; x++)
			{
				EmptyDataPoint<T> currentValue = ValueAtPosition(storedValues, x, y);
				powidl::Vector3 velocity = gridManager->GetVelocityAtCoordinate(currentValue.GetCoordinates());

				AdvectValue(x, y, velocity); // Forward advection
			}
		}
	}

	void Diffuse()
	{
		float a = lastDeltaTime * m_diffusionFactor;

		for (int y = 0; y < m_rows; y++)
		{
			for (int x = 0; x < m_columns; x++)
			{
				if (x < m_columns - 1)
				{
					T currentValue = ValueAtPosition(newValues, x + 1, y).GetValue() * a;
					ValueAtPosition(newValues, x, y).AddToValue(currentValue);
					ValueAtPosition(newValues, x + 1, y).AddToValue(-currentValue);
				}
				if (x > 0)
				{
					T currentValue = ValueAtPosition(newValues, x - 1, y).GetValue() * a;
					ValueAtPosition(newValues, x, y).AddToValue(currentValue);
					ValueAtPosition(newValues, x - 1, y).AddToValue(-currentValue);
				}


				if (y < m_rows - 1)
				{
					T currentValue = ValueAtPosition(newValues, x, y + 1).GetValue() * a;
					ValueAtPosition(newValues, x, y).AddToValue(currentValue);
					ValueAtPosition(newValues, x, y + 1).AddToValue(-currentValue);
				}
				if (y > 0)
				{
					T currentValue = ValueAtPosition(newValues, x, y - 1).GetValue() * a;
					ValueAtPosition(newValues, x, y).AddToValue(currentValue);
					ValueAtPosition(newValues, x, y - 1).AddToValue(-currentValue);
				}
			}
		}
	}

	void AddValue(float x, float y, T valueToAdd)
	{
		ValueAtPosition(storedValues, GetLowerXIndex(x), GetLowerYIndex(y)).AddToValue(valueToAdd);
	}

private:
	/** The name of the timeline used by this Plum. */
	std::string m_timelineName;

	/** The timeline used by this Plum. */
	std::shared_ptr<powidl::ITimeline> m_timeline;

	std::size_t m_rows;
	std::size_t m_columns;

	float m_height;
	float m_width;

	float m_offsetX;
	float m_offsetY;

	T m_defaultValue;
	T m_zeroValue;

	std::vector<EmptyDataPoint<T>> storedValues;
	std::vector<EmptyDataPoint<T>> newValues;

	GridManager* gridManager;

	bool isVeloctiyGrid;
	powidl::Vector3 velocityDirection;

	// Defines, how much of a cell diffuses into neighbour cells per second
	float m_diffusionFactor = 0.15f;

	// Defines, how much per cell dissapates away per tick
	float m_dissipationFactor = 0.0005f;

	float lastDeltaTime;



	void AdvectValue(std::size_t indexX, std::size_t indexY, powidl::Vector3 advectionVector)
	{
		EmptyDataPoint<T>& currentDataPoint = ValueAtPosition(newValues, indexX, indexY);
		powidl::Vector3 newForwardCoordinates = currentDataPoint.GetCoordinates() + advectionVector * lastDeltaTime;
		T currentValue = currentDataPoint.GetValue();
		currentDataPoint.SetValue(m_zeroValue);

		float cellWidth = GetCellWidth();
		float cellHeight = GetCellHeight();

		std::size_t lowerNewXIndex = GetLowerXIndex(newForwardCoordinates.x);
		std::size_t lowerNewYIndex = GetLowerYIndex(newForwardCoordinates.y);

		float dx = 1.0f - min(1.0f, max(0.0f, (newForwardCoordinates.x - (lowerNewXIndex * cellWidth + m_offsetX)) / cellWidth));
		float dy = 1.0f - min(1.0f, max(0.0f, (newForwardCoordinates.y - (lowerNewYIndex * cellHeight + m_offsetY)) / cellHeight));

		//if (lowerNewXIndex >= indexX) { dx = 1 - dx; }
		//if (lowerNewYIndex >= indexY) { dy = 1 - dy; }

		assert(!(dx > 1 || dx < 0));
		assert(!(dy > 1 || dy < 0));

		ValueAtPosition(newValues, lowerNewXIndex, lowerNewYIndex).AddToValue(currentValue * (dx * dy));
		ValueAtPosition(newValues, lowerNewXIndex + 1, lowerNewYIndex).AddToValue(currentValue * (1 - dx) * dy);
		ValueAtPosition(newValues, lowerNewXIndex, lowerNewYIndex + 1).AddToValue(currentValue * dx * (1 - dy));
		ValueAtPosition(newValues, lowerNewXIndex + 1, lowerNewYIndex + 1).AddToValue(currentValue * ((1 - dx) * (1 - dy)));
	}




	/*
		Getters and convenience methods
	*/

	T Interpolate(T value1, T value2, float g) { return (1 - g) * value1 + g * value2; }
	bool isInside(float x, float y) { return (x >= m_offsetX && x <= m_offsetX + m_width) && (y >= m_offsetY && y <= m_offsetY + m_height); }

	/**
	 * Convenient method returning the current delta time.
	 *
	 * @return the elapsed time since the last update in seconds
	 */
	powidl::Real getDeltaTime() { m_timeline->getDeltaTime(); }

	std::size_t normalizeIndexX(std::size_t index)
	{
		if (index >= m_columns) { index = m_columns - 1; }
		if (index < 0) { index = 0; }

		return index;
	}
	std::size_t normalizeIndexY(std::size_t index)
	{
		if (index >= m_rows) { index = m_rows - 1; }
		if (index < 0) { index = 0; }

		return index;
	}

	std::size_t GetLowerXIndex(float xCoordinate) { return min((int)m_columns, max(0, (int)(((m_columns - 1) * (xCoordinate - m_offsetX)) / m_width))); }
	std::size_t GetUpperXIndex(float xCoordinate) { return GetLowerXIndex(xCoordinate) + 1; }
	std::size_t GetLowerYIndex(float yCoordinate) { return min((int)m_rows, max(0, (int)(((m_rows - 1) * (yCoordinate - m_offsetY)) / m_height))); }
	std::size_t GetUpperYIndex(float yCoordinate) { return GetLowerYIndex(yCoordinate) + 1; }

	std::size_t GetArrayPositionFromIndices(std::size_t indexX, std::size_t indexY) { return normalizeIndexX(indexX) + m_columns * normalizeIndexY(indexY); }

	float GetCellWidth() { return m_width / (m_columns - 1); }
	float GetCellHeight() { return m_height / (m_rows - 1); }
};

