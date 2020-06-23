#pragma once

#include <Sge.h>

class SimpleVertex
{
public:
	SimpleVertex(powidl::Vector2 position, powidl::Color color = powidl::Color::fromRGB(0, 0, 0)) : position(position), color(color) {}
	SimpleVertex(float positionX, float positionY, powidl::Color color = powidl::Color::fromRGB(0, 0, 0)) : SimpleVertex(powidl::Vector2(positionX, positionY), color) {}

	SimpleVertex& setPosition(powidl::Vector2 position) {
		this->position = position; return *this;
	};
	SimpleVertex& setPosition(float x, float y) { return setPosition(powidl::Vector2(x, y)); };
	powidl::Vector2 getPosition() { return position; }

	SimpleVertex& setColor(powidl::Color color) {
		this->color = color; return *this;
	};
	powidl::Color getColor() { return color; }

private:
	powidl::Vector2 position;
	powidl::Color color;
};

