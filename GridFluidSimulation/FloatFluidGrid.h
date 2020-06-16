#pragma once
#include "FloatDataPoint.h"


class GridManager;


class FloatFluidGrid : public powidl::UpdatableKeyPlum
{
public:
	/**
	 * Constructor.
	 *
	 * @param keyPath		the path to the data depot
	 * @param timelineName	the name of the timeline to use
	 */
	FloatFluidGrid(std::size_t columns, std::size_t rows, float width, float height, float defaultValue, float offsetX = 0, float offsetY = 0, const std::string& keyPath = "", const std::string& timelineName = powidl::ITimekeeper::DEFAULT_TIMELINE_NAME);

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;
	virtual void update() override;

	void InitializeGrid();

	std::vector<FloatDataPoint> GetDataPoints();

	FloatDataPoint GetDataPoint(std::size_t indexX, std::size_t indexY);

	FloatDataPoint GetDataPointAtCoordinate(float x, float y);
	FloatDataPoint GetDataPointAtCoordinate(powidl::Vector3 position);


	std::size_t getRows();
	std::size_t getColumns();

	float GetWidth();
	float GetHeight();
	float GetOffsetX();
	float GetOffsetY();

	void StartAdvection();

	void AdvectValues(float deltaTime, GridManager* gridManager);

private:
	/** The name of the timeline used by this Plum. */
	std::string m_timelineName;

	/** The timeline used by this Plum. */
	std::shared_ptr<powidl::ITimeline> m_timeline;


	/**
	 * Convenient method returning the current delta time.
	 *
	 * @return the elapsed time since the last update in seconds
	 */
	powidl::Real getDeltaTime() {
		m_timeline->getDeltaTime();
	}

	float Interpolate(float value1, float value2, float g);
	bool isInside(float x, float y);

	std::size_t normalizeIndexX(std::size_t index);
	std::size_t normalizeIndexY(std::size_t index);

	std::size_t GetLowerXIndex(float xCoordinate);
	std::size_t GetUpperXIndex(float xCoordinate);
	std::size_t GetLowerYIndex(float yCoordinate);
	std::size_t GetUpperYIndex(float yCoordinate);

	std::size_t GetArrayPositionFromIndices(std::size_t indexX, std::size_t indexY);

	void AdvectValue(size_t indexX, size_t indexY, powidl::Vector3 advectionVector, float deltaTime);

	float GetCellWidth();
	float GetCellHeight();


	std::size_t m_rows;
	std::size_t m_columns;

	float m_height;
	float m_width;

	float m_offsetX;
	float m_offsetY;

	float m_defaultValue;

	std::vector<FloatDataPoint> storedValues;
	std::vector<FloatDataPoint> advectionCopyValues;
};

