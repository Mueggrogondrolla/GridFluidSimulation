#pragma once

#include <string>
#include <Sge.h>
#include "SimplePlane2DMesh.h"

class RenderData;

class GridFluidRenderLayer : public powidl::BaseD3d11GraphicsLayer2DPlum, public powidl::IKeyboardListener {
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

	// Geerbt �ber BaseD3d11GraphicsLayer2DPlum
	virtual void doRender(powidl::ComPtr<ID3D11DeviceContext>& deviceContext) override;

	virtual bool onKeyDown(powidl::Keycode code) override;

	struct ConstantBufferData
	{
		DirectX::XMFLOAT4X4 m_worldViewProjection;
		float totalTime;
		float fogColorR;
		float fogColorG;
		float fogColorB;
	};

	std::shared_ptr<powidl::D3d11ConstantBuffer<ConstantBufferData>> m_constantBuffer;
	std::shared_ptr<powidl::D3d11MeshInstanced> m_mesh;
	std::shared_ptr<powidl::D3d11ShaderProgram> m_shaderProgram;

	std::shared_ptr<RenderData> m_renderData;
	powidl::D3d11ShaderResourceViewGroup m_shaderResourceViews;

	void changeColor(powidl::Color color);
};

