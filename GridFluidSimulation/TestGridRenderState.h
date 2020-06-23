#pragma once

#include <string>
#include <Sge.h>

class TestGridRenderState : public powidl::KeyPlum, public powidl::IKeyboardListener, public powidl::IMouseListener {
public:

	/**
	 * Constructor.
	 * 
	 * @param keyPath	the path to the data depot
	 */
	TestGridRenderState(const std::string & keyPath = "GridRenderTest");

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;

private:
	// Add private members here.

	bool addingDye = false;
	int mouseX;
	int mouseY;


	virtual bool onKeyDown(powidl::Keycode code) override;


	virtual bool onMouseButtonDown(int x, int y, int button) override;
	virtual bool onMouseButtonUp(int x, int y, int button) override;
	virtual bool onMouseMove(int x, int y) override;
};

