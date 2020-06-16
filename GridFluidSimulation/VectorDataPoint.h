#pragma once
#include <Sge.h>

class VectorDataPoint
{
public:
	VectorDataPoint(powidl::Vector3 coordinates, powidl::Vector3 defaultValue) : coordinates(coordinates), storedValue(defaultValue) { }
	VectorDataPoint(powidl::Vector2 coordinates, powidl::Vector3 defaultValue) : VectorDataPoint(powidl::Vector3(coordinates.x, coordinates.y, 0), defaultValue) { }
	VectorDataPoint(float x, float y, float z, powidl::Vector3 defaultValue) : VectorDataPoint(powidl::Vector3(x, y, z), defaultValue) { }
	VectorDataPoint(float x, float y, powidl::Vector3 defaultValue) : VectorDataPoint(powidl::Vector3(x, y, 0), defaultValue) { }
	virtual ~VectorDataPoint() {};

	powidl::Vector3 GetCoordinates() { return coordinates; }

	powidl::Vector3 GetValue() { return storedValue; }
	VectorDataPoint& SetValue(powidl::Vector3 newValue) { storedValue = newValue; return *this; }
	VectorDataPoint& AddToValue(powidl::Vector3 valueToAdd) { storedValue += valueToAdd; return *this; }
	VectorDataPoint& AddDifferenceToValue(powidl::Vector3 newValue) { storedValue += (storedValue - newValue); return *this; }
	//VectorDataPoint SetValue(powidl::Vector3 newValue) { storedValue = (preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow) ? preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow(newValue) : newValue; return this; }

private:
	powidl::Vector3 coordinates;

	powidl::Vector3 storedValue;

	//auto preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow;
	// TODO: add a pre set value lambda function -> a function, that is executed always before the value is set to something (this would make it possible to e.g. multiply the value with a directions vector to "normalize" the direction of a velocity vector
};

