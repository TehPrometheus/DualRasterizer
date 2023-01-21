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
	// -----------------------------------------------
	// Constructor and destructor
	// -----------------------------------------------
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
	void ToggleSampleState();
	void ToggleCullingMode();
	void SetDiffuseMap(Texture* pDiffuseTexture);
	void SetWorldViewProjectionMatrix(const Matrix& worldViewProjectionMatrix);

	cullMode GetCullMode() const;
	sampleState GetSampleState() const;

	ID3DX11EffectTechnique* GetTechniquePtr() const;
	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

protected:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------
	const int								m_NROFCULLMODES							{3};
	const int								m_NROFSAMPLESTATES						{3};

	cullMode								m_CullMode						{ cullMode::noCulling };
	sampleState								m_SampleState					{ sampleState::point };

	ID3DX11Effect*							m_pEffect							{ nullptr };
	ID3DX11EffectTechnique*					m_pActiveTechnique					{ nullptr };

	ID3DX11EffectTechnique*					m_pPointBackCullTechnique			{ nullptr };
	ID3DX11EffectTechnique*					m_pLinearBackCullTechnique			{ nullptr };
	ID3DX11EffectTechnique*					m_pAnisotropicBackCullTechnique		{ nullptr };

	ID3DX11EffectTechnique*					m_pPointFrontCullTechnique			{ nullptr };
	ID3DX11EffectTechnique*					m_pLinearFrontCullTechnique			{ nullptr };
	ID3DX11EffectTechnique*					m_pAnisotropicFrontCullTechnique	{ nullptr };

	ID3DX11EffectTechnique*					m_pPointNoCullTechnique				{ nullptr };
	ID3DX11EffectTechnique*					m_pLinearNoCullTechnique			{ nullptr };
	ID3DX11EffectTechnique*					m_pAnisotropicNoCullTechnique		{ nullptr };

	ID3DX11EffectMatrixVariable*			m_pMatWorldViewProjVariable			{ nullptr };

	ID3DX11EffectShaderResourceVariable*	m_pDiffuseMapVariable				{ nullptr };
	ID3DX11EffectShaderResourceVariable*	m_pNormalMapVariable				{ nullptr };
	ID3DX11EffectShaderResourceVariable*	m_pSpecularMapVariable				{ nullptr };
	ID3DX11EffectShaderResourceVariable*	m_pGlossinessMapVariable			{ nullptr };


	//------------------------------------------------
	// Private member functions						
	//------------------------------------------------
	void BindShaderMaps();
	void BindShaderMatrices();
	void BindShaderTechniques();
};

