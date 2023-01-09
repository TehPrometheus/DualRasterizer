#pragma once
#include "Texture.h"
using namespace dae;

enum class sampleState
{
	point,
	linear,
	anisotropic
};

enum class cullMode
{
	backCulling,
	frontCulling,
	noCulling
};

class Effect
{
public:

	Effect(ID3D11Device* pDeviceInput, const std::wstring& pathInput);
	virtual ~Effect();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Effect(const Effect& other)					= delete;
	Effect(Effect&& other) noexcept				= delete;
	Effect& operator=(const Effect& other)		= delete;
	Effect& operator=(Effect&& other) noexcept  = delete;

	//------------------------------------------------
	// Public member functions						
	//------------------------------------------------
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	ID3DX11EffectTechnique* GetTechniquePtr();

	void SetWorldViewProjectionMatrix(const Matrix& worldViewProjectionMatrix);
	void SetDiffuseMap(dae::Texture* pDiffuseTexture);
	

	void ToggleSampleState();
	void ToggleCullingMode();

	sampleState GetSampleState() const;
	cullMode GetCullMode() const;

protected:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------

	const int NROFSAMPLESTATES{3};
	const int NROFCULLMODES{3};


	sampleState m_SampleState = sampleState::point;
	cullMode m_CullMode = cullMode::noCulling;

	ID3DX11Effect* m_pEffect{ nullptr };
	ID3DX11EffectTechnique* m_pActiveTechnique{nullptr};

	ID3DX11EffectTechnique* m_pPointBackCullTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pLinearBackCullTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pAnisotropicBackCullTechnique{ nullptr };

	ID3DX11EffectTechnique* m_pPointFrontCullTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pLinearFrontCullTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pAnisotropicFrontCullTechnique{ nullptr };

	ID3DX11EffectTechnique* m_pPointNoCullTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pLinearNoCullTechnique{ nullptr };
	ID3DX11EffectTechnique* m_pAnisotropicNoCullTechnique{ nullptr };

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };


	//------------------------------------------------
	// Private member functions						
	//------------------------------------------------

	void BindShaderTechniques();
	void BindShaderMatrices();
	void BindShaderMaps();
};

