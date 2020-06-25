#include "GridRenderer.h"
#include "GridManager.h"
#include "FluidGridBase.h"

using namespace std;
using namespace powidl;

GridRenderer::GridRenderer(const std::string& keyPath, const std::string& timelineName)
	: UpdatableKeyPlum(keyPath)
	, m_timelineName(timelineName)
{
	// Intentionally left empty
}

void GridRenderer::onFirstActivation()
{
	auto& gridManager = usePlum<GridManager>();
	size_t columns = gridManager.getColumns();
	size_t rows = gridManager.getRows();

	addChild(usePlum<ILineRenderer2DFactory>("FAST").createLineRenderer(Priority::NORMAL, columns * rows + columns + rows + 100));
}

void GridRenderer::onActivation()
{
	// Retrieve timeline used by this Plum
	m_timeline = usePlum<ITimekeeper>().getOrCreateTimeline(m_timelineName);
}

void GridRenderer::onDeactivation()
{
	// TODO: Place cleanup code here...

	// Release timeline.
	m_timeline.reset();
}

void GridRenderer::update() {

	// get camera properties
	auto& cameraManager = usePlum<ICamera2DManager>();
	auto& cameraControl = usePlum<Camera2DControlPlum>();
	auto camera = cameraManager.getCamera();

	Vector2 viewSize = camera->getViewSize();
	Vector2 cameraPosition = camera->getPosition();
	cameraPosition.x -= viewSize.x / 2;
	cameraPosition.y -= viewSize.y / 2;

	bool cameraPropertiesChanged = viewSize != previousViewSize || cameraPosition != previousCameraPosition;

	// output the camera position and viewport properties, if they have changed
	if (cameraPropertiesChanged)
	{
		previousViewSize = viewSize;
		previousCameraPosition = cameraPosition;
	}




	// Prepare the line renderer
	auto& lineRenderer = usePlum<ILineRenderer2D>();
	lineRenderer.setLineWidth(1);

	// TODO: fix line renderer limitation (max 2^16 lines drawn or something like that)

	powidl::Color lineColor = powidl::StandardColors::WHITE;




	// get the grid properties
	auto& gridManager = usePlum<GridManager>();
	size_t columns = gridManager.getColumns();
	size_t rows = gridManager.getRows();
	float cellWidth = gridManager.GetWidth() / columns;
	float cellHeight = gridManager.GetHeight() / rows;

	float width = gridManager.GetWidth();
	float height = gridManager.GetHeight();

	float offsetX = gridManager.GetOffsetX();
	float offsetY = gridManager.GetOffsetY();


	//m_time += m_timeline->getDeltaTime();

	//lineColor = powidl::StandardColors::RED;
	//drawVector(Vector2(offsetX + width / 2, offsetY + height / 2), Vector2(1, 0).rotateDeg(m_time * 50), cameraPosition, viewSize, lineRenderer, lineColor);

	//lineColor = powidl::StandardColors::WHITE;

	// Draw a "coordinate system"
	if (m_drawCoordinateSystem)
	{
		drawVector(Vector2(0, -1000), Vector2(0, 1000), cameraPosition, viewSize, lineRenderer, StandardColors::BLUE);
		drawVector(Vector2(-1000, 0), Vector2(1000, 0), cameraPosition, viewSize, lineRenderer, StandardColors::RED);
	}

	// Draw the grid lines
	if (!m_drawGridLines && m_drawOutline)
	{
		drawVector(Vector2(offsetX, offsetY), Vector2(offsetX + width, offsetY), cameraPosition, viewSize, lineRenderer, lineColor);
		drawVector(Vector2(offsetX + width, offsetY), Vector2(offsetX + width, offsetY + height), cameraPosition, viewSize, lineRenderer, lineColor);
		drawVector(Vector2(offsetX + width, offsetY + height), Vector2(offsetX, offsetY + height), cameraPosition, viewSize, lineRenderer, lineColor);
		drawVector(Vector2(offsetX, offsetY + height), Vector2(offsetX, offsetY), cameraPosition, viewSize, lineRenderer, lineColor);
	}

	// Draw the grid lines
	if (m_drawGridLines)
	{
		for (size_t y = 0; y <= rows; y++)
		{
			drawVector(Vector2(offsetX, offsetY + y * cellHeight), Vector2(offsetX + width, offsetY + y * cellHeight), cameraPosition, viewSize, lineRenderer, lineColor);
		}
		for (size_t x = 0; x <= columns; x++)
		{
			drawVector(Vector2(offsetX + x * cellWidth, offsetY), Vector2(offsetX + x * cellWidth, offsetY + height), cameraPosition, viewSize, lineRenderer, lineColor);
		}
	}


	if (m_drawPressurePoints)
	{
		// Draw the pressure points
		vector<EmptyDataPoint<float>> pressurePoints = gridManager.GetPressureDataPoints();
		EmptyDataPoint<float>* pressurePointsData = pressurePoints.data();

		while (pressurePointsData < &pressurePoints.back())
		{
			EmptyDataPoint<float> currentDataPoint = *pressurePointsData++;

			Vector3 currentCoordinates = currentDataPoint.GetCoordinates();
			float pressure = currentDataPoint.GetValue();

			int saturation = min(255, max(0, (int)(255 * pressure)));

			lineColor.setRgb(saturation, saturation, saturation, 255);

			drawVector(Vector2(currentCoordinates.x - 1, currentCoordinates.y), Vector2(currentCoordinates.x + 1, currentCoordinates.y), cameraPosition, viewSize, lineRenderer, lineColor);
			drawVector(Vector2(currentCoordinates.x, currentCoordinates.y - 1), Vector2(currentCoordinates.x, currentCoordinates.y + 1), cameraPosition, viewSize, lineRenderer, lineColor);
		}
	}


	if (m_drawIntermediateVelocityVectors)
	{
		vector<EmptyDataPoint<float>> pressurePoints = gridManager.GetPressureDataPoints();
		EmptyDataPoint<float>* pressurePointsData = pressurePoints.data();

		while (pressurePointsData < &pressurePoints.back())
		{
			EmptyDataPoint<float> currentDataPoint = *pressurePointsData++;

			Vector3 currentCoordinates = currentDataPoint.GetCoordinates();
			Vector3 velocity = gridManager.GetVelocityAtCoordinate(currentCoordinates);

			drawVector(Vector2(currentCoordinates.x, currentCoordinates.y), Vector2(currentCoordinates.x + velocity.x, currentCoordinates.y + velocity.y), cameraPosition, viewSize, lineRenderer, lineColor);
		}
	}


	if (m_drawVelocityVectors)
	{
		// Draw the velocity vectors in x direction
		vector<EmptyDataPoint<Vector3>> velocityVectorsX = (*gridManager.GetVector3ValueGrids().begin())->GetDataPoints();
		EmptyDataPoint<Vector3>* velocityDataX = velocityVectorsX.data();

		lineColor = StandardColors::MEDIUM_PURPLE;

		while (velocityDataX < &velocityVectorsX.back())
		{
			EmptyDataPoint<Vector3> currentDataPoint = *velocityDataX++;

			Vector3 currentCoordinates = currentDataPoint.GetCoordinates();
			Vector3 velocity = currentDataPoint.GetValue();

			drawVector(Vector2(currentCoordinates.x, currentCoordinates.y), Vector2(currentCoordinates.x + velocity.x, currentCoordinates.y + velocity.y), cameraPosition, viewSize, lineRenderer, lineColor);
		}

		// Draw the velocity vectors in y direction
		vector<EmptyDataPoint<Vector3>> velocityVectorsY = gridManager.GetVector3ValueGrids()[1]->GetDataPoints();
		EmptyDataPoint<Vector3>* velocityDataY = velocityVectorsY.data();

		lineColor = StandardColors::AQUA;

		while (velocityDataY < &velocityVectorsY.back())
		{
			EmptyDataPoint<Vector3> currentDataPoint = *velocityDataY++;

			Vector3 currentCoordinates = currentDataPoint.GetCoordinates();
			Vector3 velocity = currentDataPoint.GetValue();

			drawVector(Vector2(currentCoordinates.x, currentCoordinates.y), Vector2(currentCoordinates.x + velocity.x, currentCoordinates.y + velocity.y), cameraPosition, viewSize, lineRenderer, lineColor);
		}
	}
}



/*
 *	Helper and convienience methods
 */

void GridRenderer::drawVector(powidl::Vector2 origin, powidl::Vector2 end, powidl::Vector2 rectanglePosition, powidl::Vector2 rectangleSize, powidl::ILineRenderer2D& lineRenderer, powidl::Color lineColor)
{
	lineRenderer.setColor(lineColor);

	if (LineIntersectsRectangle(origin, end, rectanglePosition, rectangleSize))
	{

		// Draw the vector
		lineRenderer.drawLine(origin, end);

		if (m_drawArrowHeads)
		{
			float arrowHeadLength = Vector2(0, 0).add(end).sub(origin).length() * 0.2f;
			float arrowHeadAngle = 15.0f;

			/*
			Calculating and drawing the arrow head
			*/
			Vector2 vectorArrowHead1 = end;
			vectorArrowHead1.sub(origin).normalize().rotateDeg(arrowHeadAngle).flip();
			vectorArrowHead1 *= arrowHeadLength;
			vectorArrowHead1.add(end);

			Vector2 vectorArrowHead2 = end;
			vectorArrowHead2.sub(origin).normalize().rotateDeg(-arrowHeadAngle).flip();
			vectorArrowHead2 *= arrowHeadLength;
			vectorArrowHead2.add(end);

			lineRenderer.drawLine(end, vectorArrowHead1);
			lineRenderer.drawLine(end, vectorArrowHead2);
		}
	}
}

std::string GridRenderer::powidlVector2ToString(powidl::Vector2 powidlVector)
{
	return "[x: " + to_string(powidlVector.x) + ", y: " + to_string(powidlVector.y) + "]";
}

bool GridRenderer::LineIntersectsRectangle(Vector2 lineStartPoint, Vector2 lineEndPoint, Vector2 rectanglePosition, Vector2 rectangleSize)
{
	return LineIntersectsLine(lineStartPoint, lineEndPoint, Vector2(rectanglePosition.x, rectanglePosition.y), Vector2(rectanglePosition.x + rectangleSize.x, rectanglePosition.y)) ||
		LineIntersectsLine(lineStartPoint, lineEndPoint, Vector2(rectanglePosition.x + rectangleSize.x, rectanglePosition.y), Vector2(rectanglePosition.x + rectangleSize.x, rectanglePosition.y + rectangleSize.y)) ||
		LineIntersectsLine(lineStartPoint, lineEndPoint, Vector2(rectanglePosition.x + rectangleSize.x, rectanglePosition.y + rectangleSize.y), Vector2(rectanglePosition.x, rectanglePosition.y + rectangleSize.y)) ||
		LineIntersectsLine(lineStartPoint, lineEndPoint, Vector2(rectanglePosition.x, rectanglePosition.y + rectangleSize.y), Vector2(rectanglePosition.x, rectanglePosition.y)) ||
		(RectangleContainsPoint(rectanglePosition, rectangleSize, lineStartPoint) && RectangleContainsPoint(rectanglePosition, rectangleSize, lineEndPoint));
}

bool GridRenderer::LineIntersectsLine(Vector2 line1StartPoint, Vector2 line1EndPoint, Vector2 line2StartPoint, Vector2 line2EndPoint)
{
	float q = (line1StartPoint.y - line2StartPoint.y) * (line2EndPoint.x - line2StartPoint.x) - (line1StartPoint.x - line2StartPoint.x) * (line2EndPoint.y - line2StartPoint.y);
	float d = (line1EndPoint.x - line1StartPoint.x) * (line2EndPoint.y - line2StartPoint.y) - (line1EndPoint.y - line1StartPoint.y) * (line2EndPoint.x - line2StartPoint.x);

	if (d == 0)
	{
		return false;
	}

	float r = q / d;

	q = (line1StartPoint.y - line2StartPoint.y) * (line1EndPoint.x - line1StartPoint.x) - (line1StartPoint.x - line2StartPoint.x) * (line1EndPoint.y - line1StartPoint.y);
	float s = q / d;

	if (r < 0 || r > 1 || s < 0 || s > 1)
	{
		return false;
	}

	return true;
}

bool GridRenderer::RectangleContainsPoint(Vector2 rectanglePosition, Vector2 rectangleSize, Vector2 point)
{
	return rectanglePosition.x <= point.x && rectanglePosition.x + rectangleSize.x > point.x &&
		rectanglePosition.y <= point.y && rectanglePosition.y + rectangleSize.y > point.y;
}