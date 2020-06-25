#include "GridFluidRenderLayer.h"
#include "ShaderUtils.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "GridManager.h"
#include "GridRenderer.h"
#include "SimplePlane2DMesh.h"
#include "RenderData.h"

using namespace std;
using namespace powidl;

GridFluidRenderLayer::GridFluidRenderLayer(const std::string& keyPath)
	: BaseD3d11GraphicsLayer2DPlum(keyPath, Priority::LOW, "")
{
	// Intentionally left empty
}

void GridFluidRenderLayer::onFirstActivation()
{
	addChild(make_shared<GridRenderer>());
}

void GridFluidRenderLayer::onActivation()
{
	usePlum<IKeyboard>().addKeyboardListener(this);

	auto& gridManager = usePlum<GridManager>();
	size_t columns = gridManager.getColumns();
	size_t rows = gridManager.getRows();
	float cellWidth = gridManager.GetWidth() / columns;
	float cellHeight = gridManager.GetHeight() / rows;
	float offsetX = gridManager.GetOffsetX();
	float offsetY = gridManager.GetOffsetY();

	m_renderData = getPlum<RenderData>();

	auto device = usePlum<IDirect3D11>().getD3dDevice();
	vector<float> vertexDataQuadHalfed = {
		0,										0,
		0,										cellHeight / gridManager.SUBSTEP_SIZE,
		cellWidth / gridManager.SUBSTEP_SIZE,	0,

		cellWidth / gridManager.SUBSTEP_SIZE,	0,
		0,										cellHeight / gridManager.SUBSTEP_SIZE,
		cellWidth / gridManager.SUBSTEP_SIZE,	cellHeight / gridManager.SUBSTEP_SIZE
	};

	auto verticesSingleQuad = D3d11VertexBufferBuilder()
		.immutable(true)
		.vertexLength(2)
		.build(device, vertexDataQuadHalfed);

	size_t index = 0;

	vector<float> coordinatesData = vector<float>();
	for (size_t y = 0; y < rows; y++)
	{
		for (size_t x = 0; x < columns; x++)
		{
			for (size_t subStepY = 0; subStepY < gridManager.SUBSTEP_SIZE; subStepY++)
			{
				for (size_t subStepX = 0; subStepX < gridManager.SUBSTEP_SIZE; subStepX++)
				{
					coordinatesData.push_back(offsetX + x * cellWidth + subStepX * cellWidth / gridManager.SUBSTEP_SIZE);
					coordinatesData.push_back(offsetY + y * cellHeight + subStepY * cellHeight / gridManager.SUBSTEP_SIZE);
					coordinatesData.push_back((x + y * columns) * gridManager.SUBSTEP_SIZE * gridManager.SUBSTEP_SIZE + gridManager.SUBSTEP_SIZE * subStepY + subStepX);
				}
			}
		}
	}

	auto coordinates = D3d11VertexBufferBuilder()
		.immutable(true)
		.vertexLength(3)
		.build(device, coordinatesData);

	m_mesh = make_shared<D3d11MeshInstanced>(move(verticesSingleQuad), move(coordinates));
	m_mesh->setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_shaderProgram = D3d11ShaderProgramBuilder()
		.inputLayout(D3d11VertexInfo()
			.addAttribute(D3d11VertexAttribute::createFloat2("POSITION", 0))
			.addAttribute(D3d11VertexAttribute::createFloat3("TEXCOORD", 0, 1, true))
			.createInputLayout(device, dataVertexShader, sizeof(dataVertexShader)))
		.vertexShader(createVertexShader(device, VertexShader))
		.pixelShader(createPixelShader(device, PixelShader))
		.build();



	m_constantBuffer = D3d11ConstantBufferBuilder<ConstantBufferData>()
		.cpuRead(false)
		.cpuWrite(true)
		.build(device);

	changeColor(StandardColors::RED);

	m_shaderResourceViews.addSRV(m_renderData->getShaderResourceView());
}

void GridFluidRenderLayer::onDeactivation()
{
	usePlum<IKeyboard>().removeKeyboardListener(this);

	m_constantBuffer = nullptr;
}

void GridFluidRenderLayer::doRender(powidl::ComPtr<ID3D11DeviceContext>& deviceContext)
{
	getCamera().update();
	m_constantBuffer->getData().m_worldViewProjection = getCamera().getCombinedTransposed();
	m_constantBuffer->getData().totalTime += 0.01f;

	m_constantBuffer->update(deviceContext);
	m_constantBuffer->bindVS(deviceContext);

	m_renderData->updateData(deviceContext);
	m_shaderResourceViews.bindVS(deviceContext);

	m_shaderProgram->bind(deviceContext);
	m_mesh->render(deviceContext);


	m_shaderProgram->unbind(deviceContext);
	m_shaderResourceViews.unbindVS(deviceContext);
	m_constantBuffer->unbindVS(deviceContext);
}

bool GridFluidRenderLayer::onKeyDown(powidl::Keycode code)
{
	if (usePlum<IKeyboard>().isKeyPressed(Keycode::K_LEFT_CTRL))
	{
		if (code == Keycode::K_C) { usePlum<GridRenderer>().m_drawCoordinateSystem = !usePlum<GridRenderer>().m_drawCoordinateSystem; }
		if (code == Keycode::K_G) { usePlum<GridRenderer>().m_drawGridLines = !usePlum<GridRenderer>().m_drawGridLines; }
		if (code == Keycode::K_I) { usePlum<GridRenderer>().m_drawIntermediateVelocityVectors = !usePlum<GridRenderer>().m_drawIntermediateVelocityVectors; }
		if (code == Keycode::K_O) { usePlum<GridRenderer>().m_drawOutline = !usePlum<GridRenderer>().m_drawOutline; }
		if (code == Keycode::K_V) { usePlum<GridRenderer>().m_drawVelocityVectors = !usePlum<GridRenderer>().m_drawVelocityVectors; }
	}

	if (code == Keycode::K_F)
	{
		changeColor(Color::fromRGB(rand() % 256, rand() % 256, rand() % 256, 255));
	}

	return false;
}

void GridFluidRenderLayer::changeColor(Color color)
{
	m_constantBuffer->getData().fogColorR = color.r;
	m_constantBuffer->getData().fogColorG = color.g;
	m_constantBuffer->getData().fogColorB = color.b;
}
