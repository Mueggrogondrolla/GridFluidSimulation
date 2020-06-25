#include "RenderData.h"
#include "GridManager.h"

using namespace std;
using namespace powidl;

RenderData::RenderData(const std::string & keyPath)
	: KeyPlum(keyPath)
{
	// Intentionally left empty
}

const powidl::ComPtr<ID3D11ShaderResourceView>& RenderData::getShaderResourceView()
{
	if (!m_shaderResourceView)
	{
		m_shaderResourceView = m_data->createSRV(usePlum<IDirect3D11>().getD3dDevice());
	}

	return m_shaderResourceView;
}

void RenderData::updateData(ComPtr<ID3D11DeviceContext>& deviceContext)
{
	size_t columns = gridManager->getColumns();
	size_t rows = gridManager->getRows();
	float cellWidth = gridManager->GetWidth() / columns;
	float cellHeight = gridManager->GetHeight() / rows;

	float width = gridManager->GetWidth();
	float height = gridManager->GetHeight();

	float offsetX = gridManager->GetOffsetX();
	float offsetY = gridManager->GetOffsetY();

	vector<float> renderData = vector<float>();

	for (size_t y = 0; y < rows; y++)
	{
		for (size_t x = 0; x < columns; x++)
		{
			//renderData.push_back(gridManager->GetDensityAtCoordinate(offsetX + x * cellWidth, offsetY + y * cellHeight, 0));
			//renderData.push_back(gridManager->GetDensityAtCoordinate(offsetX + x * cellWidth + cellWidth / 2, offsetY + y * cellHeight, 0));

			for (size_t subStepY = 0; subStepY < gridManager->SUBSTEP_SIZE; subStepY++)
			{
				for (size_t subStepX = 0; subStepX < gridManager->SUBSTEP_SIZE; subStepX++)
				{
					renderData.push_back(gridManager->GetDensityAtCoordinate(
						offsetX + x * cellWidth + subStepX * cellWidth / gridManager->SUBSTEP_SIZE + cellWidth / gridManager->SUBSTEP_SIZE / 2,
						offsetY + y * cellHeight + subStepY * cellHeight / gridManager->SUBSTEP_SIZE + cellHeight / gridManager->SUBSTEP_SIZE / 2, 0));
				}
			}
		}
	}

	m_data->updateData(deviceContext, renderData);

	//m_data->updateData(deviceContext, gridManager->GetDataToBeRendered());
}

void RenderData::onFirstActivation()
{
	// Add child Plums here...
}

void RenderData::onActivation()
{
	auto& device = usePlum<IDirect3D11>().getD3dDevice();
	gridManager = getPlum<GridManager>();

	m_data = D3d11StructuredBufferBuilder<float>()
		.numberOfElements(gridManager->getColumns() * gridManager->getRows() * gridManager->SUBSTEP_SIZE * gridManager->SUBSTEP_SIZE)
		.cpuWrite(true)
		.shaderResource(true)
		.unorderedAccess(false)
		.build(device);
}

void RenderData::onDeactivation()
{
	// Place cleanup code here...
}
