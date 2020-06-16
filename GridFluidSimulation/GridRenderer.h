#pragma once

#include <string>
#include <memory>
#include <Sge.h>

class GridRenderer : public powidl::UpdatableKeyPlum {
public:

	/**
	 * Constructor.
	 * 
	 * @param keyPath		the path to the data depot
	 * @param timelineName	the name of the timeline to use
	 */
	GridRenderer(const std::string & keyPath = "", const std::string & timelineName = powidl::ITimekeeper::DEFAULT_TIMELINE_NAME);

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;
	virtual void update() override;

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

	std::string powidlVector2ToString(powidl::Vector2 powidlVector);


	bool m_drawArrowHeads = false;

	bool m_drawGridLines = false;
	bool m_drawPressurePoints = true;
	bool m_drawVelocityVectors = true;
	bool m_drawCoordinateSystem = false;

	powidl::Vector2 previousViewSize = powidl::Vector2(0, 0);
	powidl::Vector2 previousCameraPosition = powidl::Vector2(0, 0);

	std::shared_ptr<powidl::SpriteNode2D> cellContentsSpriteNode;

	void drawVector(powidl::Vector2 origin, powidl::Vector2 end, powidl::Vector2 rectanglePosition, powidl::Vector2 rectangleSize, powidl::ILineRenderer2D& lineRenderer, powidl::Color lineColor = powidl::StandardColors::BLACK);

	static bool LineIntersectsRectangle(powidl::Vector2 lineStartPoint, powidl::Vector2 lineEndPoint, powidl::Vector2 rectanglePosition, powidl::Vector2 rectangleSize);
	static bool LineIntersectsLine(powidl::Vector2 line1StartPoint, powidl::Vector2 line1EndPoint, powidl::Vector2 line2StartPoint, powidl::Vector2 line2EndPoint);
	static bool RectangleContainsPoint(powidl::Vector2 rectanglePosition, powidl::Vector2 rectangleSize, powidl::Vector2 point);
};

