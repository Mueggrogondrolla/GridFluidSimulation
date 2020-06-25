#pragma once

#include <string>
#include <Sge.h>
#include "EmptyDataPoint.h"

template<class T> class FluidGridBase;

class GridManager : public powidl::UpdatableKeyPlum {
public:

	/**
	 * Constructor.
	 *
	 * @param keyPath	the path to the data depot
	 */
	GridManager(std::size_t columns, std::size_t rows, float width, float height, float offsetX = 0, float offsetY = 0, const std::string& keyPath = "", const std::string& timelineName = powidl::ITimekeeper::DEFAULT_TIMELINE_NAME);

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;
	virtual void update() override;


	std::size_t getRows();
	std::size_t getColumns();

	float GetWidth();
	float GetHeight();
	float GetOffsetX();
	float GetOffsetY();


	std::vector<std::shared_ptr<FluidGridBase<powidl::Vector3>>> GetVector3ValueGrids();
	std::vector<EmptyDataPoint<float>> GetPressureDataPoints();

	std::vector<float> GetDataToBeRendered();

	powidl::Vector3 GetVelocityAtCoordinate(float x, float y, float z);
	powidl::Vector3 GetVelocityAtCoordinate(powidl::Vector2 coordinates);
	powidl::Vector3 GetVelocityAtCoordinate(powidl::Vector3 coordinates);

	float GetDensityAtCoordinate(float x, float y, float z);
	float GetDensityAtCoordinate(powidl::Vector2 coordinates);
	float GetDensityAtCoordinate(powidl::Vector3 coordinates);

	void AdvectAll();

	void AddDye(float x, float y, float amount);
	void AddVelocity(float x, float y, powidl::Vector3 velocityToAdd);

	bool IsPaused();
	bool Pause();
	bool Resume();

	void Reset();

	const int SUBSTEP_SIZE = 5;

private:
	/** The name of the timeline used by this Plum. */
	std::string m_timelineName;

	/** The timeline used by this Plum. */
	std::shared_ptr<powidl::ITimeline> m_timeline;

	float maxDeltaTime = 0.2f;

	float m_time = 0;

	bool m_isRunning = false;


	/**
	 * Convenient method returning the current delta time.
	 *
	 * @return the elapsed time since the last update in seconds
	 */
	powidl::Real getDeltaTime() {
		return m_timeline->getDeltaTime();
	}

	//template<typename T> std::vector<FluidGridBase<T>> m_grids;

	std::vector<std::shared_ptr<FluidGridBase<float>>> m_floatValueGrids;
	std::vector<std::shared_ptr<FluidGridBase<powidl::Vector3>>> m_vector3ValueGrids;


	void InitializeGrids(std::size_t columns, std::size_t rows, float width, float height, float offsetX, float offsetY);

	void computeBoundaries();
};

