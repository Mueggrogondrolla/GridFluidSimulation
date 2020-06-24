#include "TestGridRenderState.h"
#include "GridManager.h"
#include "GridFluidRenderLayer.h"
#include "RenderData.h"

using namespace std;
using namespace powidl;

TestGridRenderState::TestGridRenderState(const std::string& keyPath)
	: KeyPlum(keyPath), mouseX(0), mouseY(0)
{
	// Intentionally left empty
}

void TestGridRenderState::onFirstActivation()
{
	addChild(usePlum<ISpriteSceneGraph2DFactory>().createSpriteSceneGraph2D());
	addChild(usePlum<ICamera2DManagerFactory>().createCamera2DManager());
	addChild(make_shared<Camera2DControlPlum>());

	//addChild(make_shared<GridManager>(10, 10, 800.0f, 800.0f, -400.0f, -400.0f));
	//addChild(make_shared<GridManager>(384, 216, 1920.0f, 1080.0f, -960.0f, -540.0f));
	addChild(make_shared<GridManager>(384, 216, 19.2f, 10.8f, -9.6f, -5.4f));

	addChild(make_shared<RenderData>());

	addChild(make_shared<GridFluidRenderLayer>());
}

void TestGridRenderState::onActivation()
{
	usePlum<IKeyboard>().addKeyboardListener(this);
	usePlum<IMouse>().addMouseListener(this);

	//usePlum<Camera2DControlPlum>().setHomeZoomLevel(5);
	//usePlum<Camera2DControlPlum>().setZoomLevel(5);
	usePlum<Camera2DControlPlum>().setZoom(80);
}

void TestGridRenderState::onDeactivation()
{

}

bool TestGridRenderState::onKeyDown(powidl::Keycode code)
{
	auto& gridManager = usePlum<GridManager>();
	if (code == powidl::Keycode::K_A)
	{
		if (gridManager.IsPaused()) { usePlum<GridManager>().AdvectAll(); }
	}
	if (code == powidl::Keycode::K_P)
	{
		if (gridManager.IsPaused()) { gridManager.Resume(); }
		else { gridManager.Pause(); }
	}
	if (code == powidl::Keycode::K_R)
	{
		usePlum<GridManager>().Reset();
	}

	return true;
}

bool TestGridRenderState::onMouseButtonDown(int x, int y, int button)
{
	if (button == 0)
	{
		mouseX = x;
		mouseY = y;

		auto cam = usePlum<ICamera2DManager>().getCamera();
		auto worldPoint = cam->inverseTransformPoint(x, y);

		usePlum<GridManager>().AddVelocity(worldPoint.x, worldPoint.y, Vector3(0.0f, 0.0f, 0.0f));
		usePlum<GridManager>().AddDye(worldPoint.x, worldPoint.y, 10.0f);

		addingDye = true;
	}

	return true;
}

bool TestGridRenderState::onMouseButtonUp(int x, int y, int button)
{
	if (button == 0)
	{
		addingDye = false;
	}

	return true;
}

bool TestGridRenderState::onMouseMove(int x, int y)
{
	if (addingDye)
	{
		auto cam = usePlum<ICamera2DManager>().getCamera();
		auto worldPointPreviously = cam->inverseTransformPoint(mouseX, mouseY);
		auto worldPoint = cam->inverseTransformPoint(x, y);

		Vector2 dragDirection = worldPoint - worldPointPreviously;

		usePlum<GridManager>().AddVelocity(worldPoint.x, worldPoint.y, Vector3(dragDirection.x, dragDirection.y, 0) * 5.0f);
		usePlum<GridManager>().AddDye(worldPoint.x, worldPoint.y, 1.0f);

		mouseX = x;
		mouseY = y;
	}

	return true;
}
