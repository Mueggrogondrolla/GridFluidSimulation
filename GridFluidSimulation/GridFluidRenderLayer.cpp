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
	: BaseD3d11GraphicsLayer2DPlum(keyPath, Priority::NORMAL, "")
{
	// Intentionally left empty
}

void GridFluidRenderLayer::onFirstActivation()
{
	addChild(make_shared<GridRenderer>());
}

void GridFluidRenderLayer::onActivation()
{
	auto& gridManager = usePlum<GridManager>();
	size_t columns = gridManager.getColumns();
	size_t rows = gridManager.getRows();
	float cellWidth = gridManager.GetWidth() / columns;
	float cellHeight = gridManager.GetHeight() / rows;
	float offsetX = gridManager.GetOffsetX();
	float offsetY = gridManager.GetOffsetY();

	m_numberOfDataPoints = columns * rows;

	m_renderData = getPlum<RenderData>();

	/** Indexed mesh
	mesh = SimplePlane2DMesh();

	for (size_t y = 0; y < rows; y++)
	{
		for (size_t x = 0; x < columns; x++)
		{
			mesh.AddTriangle(SimpleTriangle(
				SimpleVertex(x * cellWidth + offsetX, y * cellHeight + offsetY, powidl::Color::fromRGB(x, y, 0)),
				SimpleVertex(x * cellWidth + offsetX + cellWidth, y * cellHeight + offsetY, powidl::Color::fromRGB(x, y, 0)),
				SimpleVertex(x * cellWidth + offsetX, y * cellHeight + offsetY + cellHeight, powidl::Color::fromRGB(x, y, 0))
				));

			mesh.AddTriangle(SimpleTriangle(
				SimpleVertex(x * cellWidth + offsetX, y * cellHeight + offsetY + cellHeight, powidl::Color::fromRGB(x, y, 0)),
				SimpleVertex(x * cellWidth + offsetX + cellWidth, y * cellHeight + offsetY + cellHeight, powidl::Color::fromRGB(x, y, 0)),
				SimpleVertex(x * cellWidth + offsetX + cellWidth, y * cellHeight + offsetY, powidl::Color::fromRGB(x, y, 0))
				));
		}
	}

	auto device = usePlum<IDirect3D11>().getD3dDevice();

	m_vertexBuffer = move(D3d11VertexBufferBuilder()
		.immutable(false)
		.updatable(true)
		.vertexLength(6)
		.build(device, mesh.GetFloatValues()));

	auto indices = D3d11IndexBufferBuilder()
		.immutable(true)
		.build(device, mesh.GetIndices());

	m_mesh = std::make_shared<D3d11MeshIndexed>(m_vertexBuffer, move(indices));
	m_mesh->setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_shaderProgram = D3d11ShaderProgramBuilder()
		.inputLayout(D3d11VertexInfo()
			.addAttribute(D3d11VertexAttribute::createFloat2("POSITION", 0))
			.addAttribute(D3d11VertexAttribute::createFloat4("COLOR", 0))
			.createInputLayout(device, dataVertexShader, sizeof(dataVertexShader)))
		.vertexShader(createVertexShader(device, VertexShader))
		.pixelShader(createPixelShader(device, PixelShader))
		.build();
	*/

	auto device = usePlum<IDirect3D11>().getD3dDevice();

	vector<float> vertexData = { 
		offsetX,				offsetY, 
		offsetX,				offsetY + cellHeight, 
		offsetX + cellWidth,	offsetY, 

		offsetX + cellWidth,	offsetY, 
		offsetX,				offsetY + cellHeight, 
		offsetX + cellWidth,	offsetY + cellHeight };

	auto verticesSingleQuad = D3d11VertexBufferBuilder()
		.immutable(true)
		.vertexLength(2)
		.build(device, vertexData);

	vector<float> coordinatesData = vector<float>();
	for (size_t y = 0; y < rows; y++)
	{
		for (size_t x = 0; x < columns; x++)
		{
			coordinatesData.push_back(x * cellWidth);
			coordinatesData.push_back(y * cellHeight);
			coordinatesData.push_back(x + y * columns);
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

	m_constantBuffer->getData().columns = columns;

	m_shaderResourceViews.addSRV(m_renderData->getShaderResourceView());
}

void GridFluidRenderLayer::onDeactivation()
{
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
