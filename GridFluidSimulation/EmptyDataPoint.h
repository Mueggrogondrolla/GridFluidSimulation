#pragma once
#include <Sge.h>

template<class T> class EmptyDataPoint
{
public:
	EmptyDataPoint(powidl::Vector3 coordinates, T defaultValue) : coordinates(coordinates), storedValue(defaultValue) { }
	EmptyDataPoint(powidl::Vector2 coordinates, T defaultValue) : EmptyDataPoint(powidl::Vector3(coordinates.x, coordinates.y, 0), defaultValue) { }
	EmptyDataPoint(float x, float y, float z, T defaultValue) : EmptyDataPoint(powidl::Vector3(x, y, z), defaultValue) { }
	EmptyDataPoint(float x, float y, T defaultValue) : EmptyDataPoint(powidl::Vector3(x, y, 0), defaultValue) { }
	virtual ~EmptyDataPoint() {};

	powidl::Vector3 GetCoordinates() { return coordinates; }

	T GetValue() { return storedValue; }
	EmptyDataPoint& SetValue(T newValue) { storedValue = newValue; return *this; }
	EmptyDataPoint& AddToValue(T valueToAdd) { storedValue += valueToAdd; return *this; }
	EmptyDataPoint& AddDifferenceToValue(T newValue) { storedValue += (storedValue - newValue); return *this; }
	//EmptyDataPoint SetValue(T newValue) { storedValue = (preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow) ? preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow(newValue) : newValue; return this; }

private:
	powidl::Vector3 coordinates;

	T storedValue;

	//auto preSetValueLambdaFunctionIReallyDontHaveABetterNameRightNow;
	// TODO: add a pre set value lambda function -> a function, that is executed always before the value is set to something (this would make it possible to e.g. multiply the value with a directions vector to "normalize" the direction of a velocity vector
};

