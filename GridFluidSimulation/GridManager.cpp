#include "GridManager.h"
#include "FluidGridBase.h"

using namespace std;
using namespace powidl;

GridManager::GridManager(size_t columns, size_t rows, float width, float height, float offsetX, float offsetY, const std::string& keyPath, const std::string& timelineName)
	: UpdatableKeyPlum(keyPath), m_timelineName(timelineName)
{
	InitializeGrids(columns, rows, width, height, offsetX, offsetY);
}

void GridManager::InitializeGrids(size_t columns, size_t rows, float width, float height, float offsetX, float offsetY)
{
	float cellWidth = width / columns;
	float cellHeight = height / rows;

	m_floatValueGrids.push_back(make_shared<FluidGridBase<float>>(columns, rows, width - cellWidth, height - cellHeight, 0.9f, 0.0f, this, offsetX + cellWidth / 2, offsetY + cellHeight / 2));

	m_vector3ValueGrids.push_back(make_shared<FluidGridBase<powidl::Vector3>>(columns + 1, rows, width, height - cellHeight, Vector3(1, 0, 0), Vector3(0, 0, 0), this, offsetX, offsetY + cellHeight / 2, true, powidl::Vector3(1, 0, 0))); // X velocities
	m_vector3ValueGrids.push_back(make_shared<FluidGridBase<powidl::Vector3>>(columns, rows + 1, width - cellWidth, height, Vector3(0, 1, 0), Vector3(0, 0, 0), this, offsetX + cellWidth / 2, offsetY, true, powidl::Vector3(0, 1, 0))); // Y velocities
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
	m_time += m_timeline->getDeltaTime();
	Vector3 shootDirection = powidl::Vector3(2, 0, 0).rotateZDeg(m_time * 50);

	if (m_isRunning)
	{
		Vector2 m_center = Vector2(GetOffsetX() + GetWidth() / 2, GetOffsetY() + GetHeight() / 2);

		AddVelocity(m_center.x, m_center.y, shootDirection);
		AddDye(m_center.x, m_center.y, 2);

		AdvectAll();
	}
}

void GridManager::AddDye(float x, float y, float amount)
{
	float width = GetWidth(), height = GetHeight();
	float offsetX = GetOffsetX(), offsetY = GetOffsetY();

	// if the point, where dye shall be added is outside the actual grid, redefine the point to be on the nearest edge
	if (x < offsetX) { x = offsetX; }
	if (x > offsetX + width) { x = offsetX + width; }

	if (y < offsetY) { y = offsetY; }
	if (y > offsetY + height) { y = offsetY + height; }

	// apply the dye
	(*m_floatValueGrids.begin())->AddValue(x, y, amount);
}

void GridManager::AddVelocity(float x, float y, powidl::Vector3 velocityToAdd)
{
	float width = GetWidth(), height = GetHeight();
	float offsetX = GetOffsetX(), offsetY = GetOffsetY();

	// if the point, where velocity shall be added is outside the actual grid, redefine the velocity vector to the nearest edge facing inwards
	if (x < offsetX || x > offsetX + width || y < offsetY || y > offsetY + height)
	{
		velocityToAdd = Vector3(0, 0, 0);
	}

	if (x < offsetX)
	{
		x = offsetX;
		velocityToAdd.x = 5;
	}
	if (x > offsetX + width)
	{
		x = offsetX + width;
		velocityToAdd.x = -5;
	}

	if (y < offsetY)
	{
		y = offsetY;
		velocityToAdd.y = 5;
	}
	if (y > offsetY + height)
	{
		y = offsetY + height;
		velocityToAdd.y = -5;
	}


	// add the x component to the velocities
	(*m_vector3ValueGrids.begin())->AddValue(x, y, Vector3(velocityToAdd.x, 0, 0));

	// add the y component to the velocities
	(*(++m_vector3ValueGrids.begin()))->AddValue(x, y, Vector3(0, velocityToAdd.y, 0));
}

void GridManager::AdvectAll()
{
	float deltaTime = min(maxDeltaTime, getDeltaTime());

	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->StartUpdate(deltaTime); }
	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->Diffuse(); }
	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->AdvectValues(); }
	for (vector<shared_ptr<FluidGridBase<Vector3>>>::iterator iterator = m_vector3ValueGrids.begin(); iterator < m_vector3ValueGrids.end(); iterator++) { (*iterator)->EndUpdate(); }

	computeBoundaries();

	for (vector<shared_ptr<FluidGridBase<float>>>::iterator iterator = m_floatValueGrids.begin(); iterator < m_floatValueGrids.end(); iterator++)
	{
		(*iterator)->StartUpdate(deltaTime);
		(*iterator)->Diffuse();
		(*iterator)->AdvectValues();
		(*iterator)->EndUpdate();
	}
}

void GridManager::computeBoundaries()
{
	size_t columns = (*(++m_vector3ValueGrids.begin()))->getColumns();
	size_t rows = (*(++m_vector3ValueGrids.begin()))->getRows();

	vector<EmptyDataPoint<Vector3>> velocitiesToAdd = vector<EmptyDataPoint<Vector3>>();

	float cellWidth = (*(++m_vector3ValueGrids.begin()))->GetWidth() / (columns - 1);

	// looking at the top and bottom wall
	for (std::size_t x = 0; x < columns; x++)
	{
		EmptyDataPoint<powidl::Vector3> currentValue = (*(++m_vector3ValueGrids.begin()))->GetDataPoint(x, 0);
		Vector3 currentCoordinates = currentValue.GetCoordinates();
		bool isCorner = x == 0 || x == columns - 1;

		if (currentValue.GetValue().y != 0)
		{
			// this helper variable defines, whether the fluid pushes against the wall in this location or not
			bool isPushing = currentValue.GetValue().y < 0;

			float impulseFactor = (isCorner ? 0.5f : 0.25f) * (isPushing ? 1 : -1);
			impulseFactor *= 0.99f;

			if (x > 0)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x - cellWidth / 2, currentCoordinates.y, powidl::Vector3(currentValue.GetValue().y * impulseFactor, 0, 0)));
			}
			if (x < columns - 1)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x + cellWidth / 2, currentCoordinates.y, powidl::Vector3(-currentValue.GetValue().y * impulseFactor, 0, 0)));
			}
			velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates, -currentValue.GetValue()));
		}


		currentValue = (*(++m_vector3ValueGrids.begin()))->GetDataPoint(x, rows - 1);
		currentCoordinates = currentValue.GetCoordinates();

		if (currentValue.GetValue().y != 0)
		{
			bool isPushing = currentValue.GetValue().y > 0;
			float impulseFactor = (isCorner ? 0.5f : 0.25f) * (isPushing ? 1 : -1);
			impulseFactor *= 0.99f;

			if (x > 0)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x - cellWidth / 2, currentCoordinates.y, powidl::Vector3(-currentValue.GetValue().y * impulseFactor, 0, 0)));
			}
			if (x < columns - 1)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x + cellWidth / 2, currentCoordinates.y, powidl::Vector3(currentValue.GetValue().y * impulseFactor, 0, 0)));
			}
			velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates, -currentValue.GetValue()));
		}
	}


	columns = (*m_vector3ValueGrids.begin())->getColumns();
	rows = (*m_vector3ValueGrids.begin())->getRows();

	float cellHeight = (*m_vector3ValueGrids.begin())->GetHeight() / (rows - 1);

	// looking at the right and left wall
	for (std::size_t y = 0; y < rows; y++)
	{
		EmptyDataPoint<powidl::Vector3> currentValue = (*m_vector3ValueGrids.begin())->GetDataPoint(0, y);
		Vector3 currentCoordinates = currentValue.GetCoordinates();
		bool isCorner = y == 0 || y == rows - 1;

		if (currentValue.GetValue().x != 0)
		{
			bool isPushing = currentValue.GetValue().x < 0;

			float impulseFactor = (isCorner ? 0.5f : 0.25f) * (isPushing ? 1 : -1);
			impulseFactor *= 0.99f;

			if (y > 0)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x, currentCoordinates.y - cellHeight / 2, powidl::Vector3(0, currentValue.GetValue().x * impulseFactor, 0)));
			}
			if (y < rows - 1)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x, currentCoordinates.y + cellHeight / 2, powidl::Vector3(0, -currentValue.GetValue().x * impulseFactor, 0)));
			}
			velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates, -currentValue.GetValue()));
		}


		currentValue = (*m_vector3ValueGrids.begin())->GetDataPoint(columns - 1, y);
		currentCoordinates = currentValue.GetCoordinates();

		if (currentValue.GetValue().x != 0)
		{
			bool isPushing = currentValue.GetValue().x > 0;
			float impulseFactor = (isCorner ? 0.5f : 0.25f) * (isPushing ? 1 : -1);
			impulseFactor *= 0.99f;

			if (y > 0)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x, currentCoordinates.y - cellHeight / 2, powidl::Vector3(0, -currentValue.GetValue().x * impulseFactor, 0)));
			}
			if (y < rows - 1)
			{
				velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates.x, currentCoordinates.y + cellHeight / 2, powidl::Vector3(0, currentValue.GetValue().x * impulseFactor, 0)));
			}
			velocitiesToAdd.push_back(EmptyDataPoint<Vector3>(currentCoordinates, -currentValue.GetValue()));
		}
	}

	//TODO: compute corners


	for (vector<EmptyDataPoint<Vector3>>::iterator iterator = velocitiesToAdd.begin(); iterator < velocitiesToAdd.end(); iterator++)
	{
		AddVelocity(iterator->GetCoordinates().x, iterator->GetCoordinates().y, iterator->GetValue());
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

std::vector<float> GridManager::GetDataToBeRendered()
{
	std::vector<float> data = vector<float>();

	vector<EmptyDataPoint<float>> dataPoints = (*m_floatValueGrids.begin())->GetDataPoints();
	vector<EmptyDataPoint<float>>::iterator iterator = dataPoints.begin();

	while (iterator != dataPoints.end())
	{
		data.push_back((*iterator).GetValue());
		iterator++;
	}

	return data;
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

float GridManager::GetDensityAtCoordinate(float x, float y, float z)
{
	return GetDensityAtCoordinate(Vector3(x, y, z));
}

float GridManager::GetDensityAtCoordinate(powidl::Vector2 coordinates)
{
	return GetDensityAtCoordinate(Vector3(coordinates.x, coordinates.y, 0));
}

float GridManager::GetDensityAtCoordinate(powidl::Vector3 coordinates)
{
	return m_floatValueGrids.front()->GetDataPointAtCoordinate(coordinates).GetValue();
}
