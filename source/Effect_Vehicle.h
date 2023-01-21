#pragma once
#include "Effect.h"

class Effect_Vehicle final: public Effect
{
public:
	// -----------------------------------------------
	// Constructor and destructor
	// -----------------------------------------------
	Effect_Vehicle(ID3D11Device* pDeviceInput, const std::wstring& pathInput);
	~Effect_Vehicle();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Effect_Vehicle(const Effect_Vehicle& other)					= default;
	Effect_Vehicle(Effect_Vehicle&& other) noexcept				= default;
	Effect_Vehicle& operator=(const Effect_Vehicle& other)		= default;
	Effect_Vehicle& operator=(Effect_Vehicle&& other) noexcept	= default;

	//------------------------------------------------
	// Public member functions						
	//------------------------------------------------
	void SetWorldMatrix(const Matrix& worldMatrix);
	void SetViewInverseMatrix(const Matrix& viewInverseMatrix);
	void SetNormalMap(ID3D11ShaderResourceView* pResourceView);
	void SetSpecularMap(ID3D11ShaderResourceView* pResourceView);
	void SetGlossinessMap(ID3D11ShaderResourceView* pResourceView);

private:

	//------------------------------------------------
	// Member Variables						
	//------------------------------------------------
	ID3DX11EffectMatrixVariable*		 m_pMatWorldVariable		{ nullptr };
	ID3DX11EffectMatrixVariable*		 m_pMatViewInverseVariable	{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable		{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable		{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable	{ nullptr };
};

