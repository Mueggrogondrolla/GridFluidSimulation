#pragma once

#include <string>
#include <Sge.h>
#include "SimplePlane2DMesh.h"

class GridFluidRenderLayer : public powidl::BaseD3d11GraphicsLayer2DPlum {
public:

	/**
	 * Constructor.
	 * 
	 * @param keyPath	the path to the data depot
	 */
	GridFluidRenderLayer(const std::string & keyPath = "");

private:
	// Add private members here.

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;

	struct ConstantBufferData
	{
		DirectX::XMFLOAT4X4 m_worldViewProjection;
		float totalTime;
	};

	std::shared_ptr<powidl::D3d11ConstantBuffer<ConstantBufferData>> m_constantBuffer;
	std::shared_ptr<powidl::D3d11VertexBuffer> m_vertexBuffer;
	std::shared_ptr<powidl::D3d11MeshIndexed> m_mesh;
	std::shared_ptr<powidl::D3d11ShaderProgram> m_shaderProgram;

	SimplePlane2DMesh mesh;

	// Geerbt über BaseD3d11GraphicsLayer2DPlum
	virtual void doRender(powidl::ComPtr<ID3D11DeviceContext>& deviceContext) override;
};

