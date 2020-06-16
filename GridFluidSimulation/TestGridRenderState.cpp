#include "TestGridRenderState.h"
#include "GridRenderer.h"
#include "GridManager.h"

using namespace std;
using namespace powidl;

TestGridRenderState::TestGridRenderState(const std::string & keyPath)
	: KeyPlum(keyPath)
{
	// Intentionally left empty
}

void TestGridRenderState::onFirstActivation()
{
	addChild(usePlum<ISpriteSceneGraph2DFactory>().createSpriteSceneGraph2D());
	addChild(usePlum<ICamera2DManagerFactory>().createCamera2DManager());
	addChild(make_shared<Camera2DControlPlum>());

	addChild(make_shared<EntityManagerPlum>());
	addChild(make_shared<SpriteVisualSystem>());
	
	//addChild(make_shared<GridManager>(100, 100, 800.0f, 800.0f, -400.0f, -400.0f));
	addChild(make_shared<GridManager>(384, 216, 1920.0f, 1080.0f, -960.0f, -540.0f));
	addChild(make_shared<GridRenderer>());
}

void TestGridRenderState::onActivation()
{
	usePlum<IKeyboard>().addKeyboardListener(this);
	usePlum<IMouse>().addMouseListener(this);

	shared_ptr<Entity> entity = make_shared<Entity>();
	entity->addComponent(make_unique<Pose2D>());
	entity->addComponent(make_unique<SpriteVisual>(SpriteNode2DBuilder(*this)
		.addNode(SpriteNode2DBuilder(*this)
			.addSprite(RectangleSpriteBuilder(*this)
				.width(400).height(400)
				.color(Color::fromRgb(220, 0, 0))
				.build())
			.build())
		.addNode(SpriteNode2DBuilder(*this)
			.addSprite(RectangleSpriteBuilder(*this)
				.width(100).height(100)
				.color(Color::fromRgb(180, 0, 0))
				.build())
			.build())
		.build()));
	//usePlum<IEntityManager>().addEntity(entity);
}

void TestGridRenderState::onDeactivation()
{
	// Place cleanup code here...
}

bool TestGridRenderState::onKeyDown(powidl::Keycode code)
{
	if (code == powidl::Keycode::K_A)
	{
		usePlum<GridManager>().AdvectAll();
	}
	if (code == powidl::Keycode::K_P)
	{
		auto& gridManager = usePlum<GridManager>();
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
	Logger::logDebug(to_string(button));

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

		usePlum<GridManager>().AddVelocity(worldPoint.x, worldPoint.y, Vector3(dragDirection.x, dragDirection.y, 0) * 25.0f);
		usePlum<GridManager>().AddDye(worldPoint.x, worldPoint.y, 5.0f);

		mouseX = x;
		mouseY = y;
	}

	return true;
}
