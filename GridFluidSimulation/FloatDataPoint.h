#pragma once
#include <Sge.h>

class FloatDataPoint
{
public:
	FloatDataPoint(powidl::Vector3 coordinates, float defaultValue) : coordinates(coordinates), storedValue(defaultValue) { }
	FloatDataPoint(powidl::Vector2 coordinates, float defaultValue) : FloatDataPoint(powidl::Vector3(coordinates.x, coordinates.y, 0), defaultValue) { }
	FloatDataPoint(float x, float y, float z, float defaultValue) : FloatDataPoint(powidl::Vector3(x, y, z), defaultValue) { }
	FloatDataPoint(float x, float y, float defaultValue) : FloatDataPoint(powidl::Vector3(x, y, 0), defaultValue) { }
	virtual ~FloatDataPoint() {};

	powidl::Vector3 GetCoordinates() { return coordinates; }

	float GetValue() { return storedValue; }
	FloatDataPoint& SetValue(float newValue) { storedValue = newValue; return *this; }
	FloatDataPoint& AddToValue(float newValue) { storedValue += newValue; return *this; }
	FloatDataPoint& AddDifferenceToValue(float newValue) { storedValue += (storedValue - newValue); return *this; }
	//EmptyDataPoint SetValue(T newValue) { storedValue = (preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow) ? preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow(newValue) : newValue; return this; }

private:
	powidl::Vector3 coordinates;

	float storedValue;

	//auto preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow;
	// TODO: add a pre set value lambda function -> a function, that is executed always before the value is set to something (this would make it possible to e.g. multiply the value with a directions vector to "normalize" the direction of a velocity vector
};

