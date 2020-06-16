#pragma once

#include <Sge.h>
#include "VectorDataPoint.h"

class GridManager;

class VectorFluidGrid : public powidl::UpdatableKeyPlum {
public:

	/**
	 * Constructor.
	 *
	 * @param keyPath		the path to the data depot
	 * @param timelineName	the name of the timeline to use
	 */
	VectorFluidGrid(std::size_t columns, std::size_t rows, float width, float height, powidl::Vector3 defaultValue, float offsetX = 0, float offsetY = 0, const std::string& keyPath = "", const std::string& timelineName = powidl::ITimekeeper::DEFAULT_TIMELINE_NAME);

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;
	virtual void update() override;

	void InitializeGrid();

	std::vector<VectorDataPoint> GetDataPoints();

	VectorDataPoint& GetDataPoint(std::size_t indexX, std::size_t indexY);

	VectorDataPoint GetDataPointAtCoordinate(float x, float y);
	VectorDataPoint GetDataPointAtCoordinate(powidl::Vector3 position);


	std::size_t getRows();
	std::size_t getColumns();

	float GetWidth();
	float GetHeight();
	float GetOffsetX();
	float GetOffsetY();

	void StartAdvection();
	void EndAdvection();

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

	powidl::Vector3 Interpolate(powidl::Vector3 value1, powidl::Vector3 value2, float g);
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

	powidl::Vector3 m_defaultValue;

	std::vector<VectorDataPoint> storedValues;
	std::vector<VectorDataPoint> advectionCopyValues;
};

