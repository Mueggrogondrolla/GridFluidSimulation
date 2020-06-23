#pragma once

#include <string>
#include <Sge.h>

class GridManager;

class RenderData : public powidl::KeyPlum {
public:

	/**
	 * Constructor.
	 *
	 * @param keyPath	the path to the data depot
	 */
	RenderData(const std::string& keyPath = "");

	const powidl::ComPtr<ID3D11ShaderResourceView>& getShaderResourceView();

	void updateData(powidl::ComPtr<ID3D11DeviceContext>& deviceContext);

private:

	std::shared_ptr<powidl::D3d11StructuredBuffer<float>> m_data;
	powidl::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;

	std::shared_ptr<GridManager> gridManager;

	// Inherited via Plum
	virtual void onFirstActivation() override;
	virtual void onActivation() override;
	virtual void onDeactivation() override;
};

