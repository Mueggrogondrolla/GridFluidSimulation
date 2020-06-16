#include "GridManager.h"
#include "FluidGridBase.h"

using namespace std;
using namespace powidl;

GridManager::GridManager(size_t columns, size_t rows, float width, float height, float offsetX, float offsetY, const std::string& keyPath, const std::string& timelineName)
	: UpdatableKeyPlum(keyPath), m_timelineName(timelineName), m_isRunning(true)
{
	InitializeGrids(columns, rows, width, height, offsetX, offsetY);
}

void GridManager::InitializeGrids(size_t columns, size_t rows, float width, float height, float offsetX, float offsetY)
{
	float cellWidth = width / columns;
	float cellHeight = height / rows;

	m_floatValueGrids.push_back(make_shared<FluidGridBase<float>>(columns, rows, width - cellWidth, height - cellHeight, 0.0f, 0.0f, this, offsetX + cellWidth / 2, offsetY + cellHeight / 2));

	m_vector3ValueGrids.push_back(make_shared<FluidGridBase<powidl::Vector3>>(columns + 1, rows, width, height - cellHeight, Vector3(-100, 0, 0), Vector3(0, 0, 0), this, offsetX, offsetY + cellHeight / 2, true, powidl::Vector3(1, 0, 0))); // X velocities
	m_vector3ValueGrids.push_back(make_shared<FluidGridBase<powidl::Vector3>>(columns, rows + 1, width - cellWidth, height, Vector3(0, -100, 0), Vector3(0, 0, 0), this, offsetX + cellWidth / 2, offsetY, true, powidl::Vector3(0, 1, 0))); // Y velocities
}


void GridManager::onFirstActivation()
{
	// Add child Plums here...
}

void GridManager::onActivation()
{
	// Retrieve timeline used by this Plum
	m_timeline = usePlum<ITimekeeper>().getOrCreateTimeline(m_timelineName);
}

void GridManager::onDeactivation()
{
	// Release timeline.
	m_timeline.reset();
}

void GridManager::update()
{
	if (m_isRunning)
	{
		AdvectAll();
	}
}

void GridManager::AddDye(float x, float y, float amount)
{
	// TODO: if (x, y) is not inside the bounds, apply dye at the edge

	(*m_floatValueGrids.begin())->AddValue(x, y, amount);
}

void GridManager::AddVelocity(float x, float y, powidl::Vector3 velocityToAdd)
{
	// TODO: if (x, y) is not inside the bounds, apply velocity straight from the edge inwards

	(*m_vector3ValueGrids.begin())->AddValue(x, y, Vector3(velocityToAdd.x, 0, 0));
	(*(++m_vector3ValueGrids.begin()))->AddValue(x, y, Vector3(0, velocityToAdd.y, 0));
}

void GridManager::AdvectAll()
{
	float deltaTime = min(maxDeltaTime, getDeltaTime());

	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->StartUpdate(deltaTime); }
	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->Diffuse(); }
	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->AdvectValues(); }
	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->EndUpdate(); }

	for (vector<shared_ptr<FluidGridBase<float>>>::iterator iterator = m_floatValueGrids.begin(); iterator < m_floatValueGrids.end(); iterator++)
	{
		(*iterator)->StartUpdate(deltaTime);
		(*iterator)->Diffuse();
		(*iterator)->AdvectValues();
		(*iterator)->EndUpdate();
	}
}

bool GridManager::IsPaused()
{
	return !m_isRunning;
}

bool GridManager::Pause()
{
	return m_isRunning = false;
}

bool GridManager::Resume()
{
	return m_isRunning = true;;
}

void GridManager::Reset()
{
	bool wasRunning = m_isRunning;
	m_isRunning = false;

	size_t columns = getColumns();
	size_t rows = getRows();
	float width = GetWidth();
	float height = GetHeight();
	float offsetX = GetOffsetX();
	float offsetY = GetOffsetY();

	m_floatValueGrids.clear();
	m_vector3ValueGrids.clear();

	InitializeGrids(columns, rows, width, height, offsetX, offsetY);

	m_isRunning = wasRunning;
}






std::size_t GridManager::getRows()
{
	return (*m_floatValueGrids.begin())->getRows();
}

std::size_t GridManager::getColumns()
{
	return (*m_floatValueGrids.begin())->getColumns();
}

float GridManager::GetWidth()
{
	return (*m_vector3ValueGrids.begin())->GetWidth();
}

float GridManager::GetHeight()
{
	return m_vector3ValueGrids[1]->GetHeight();
}

float GridManager::GetOffsetX()
{
	return (*m_vector3ValueGrids.begin())->GetOffsetX();
}

float GridManager::GetOffsetY()
{
	return m_vector3ValueGrids[1]->GetOffsetY();
}

vector<shared_ptr<FluidGridBase<Vector3>>> GridManager::GetVector3ValueGrids()
{
	return m_vector3ValueGrids;
}

vector<EmptyDataPoint<float>> GridManager::GetPressureDataPoints()
{
	return (*m_floatValueGrids.begin())->GetDataPoints();
}




Vector3 GridManager::GetVelocityAtCoordinate(float x, float y, float z)
{
	return GetVelocityAtCoordinate(Vector3(x, y, z));
}

Vector3 GridManager::GetVelocityAtCoordinate(Vector2 coordinates)
{
	return GetVelocityAtCoordinate(Vector3(coordinates.x, coordinates.y, 0));
}

Vector3 GridManager::GetVelocityAtCoordinate(Vector3 coordinates)
{
	return Vector3(
		m_vector3ValueGrids.front()->GetDataPointAtCoordinate(coordinates).GetValue().x,
		m_vector3ValueGrids[1]->GetDataPointAtCoordinate(coordinates).GetValue().y,
		0);
}
