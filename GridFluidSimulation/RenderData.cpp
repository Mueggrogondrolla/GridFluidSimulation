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
	m_data->updateData(deviceContext, gridManager->GetDataToBeRendered());
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
		.numberOfElements(gridManager->getColumns() * gridManager->getRows())
		.cpuWrite(true)
		.shaderResource(true)
		.unorderedAccess(false)
		.build(device);
}

void RenderData::onDeactivation()
{
	// Place cleanup code here...
}
