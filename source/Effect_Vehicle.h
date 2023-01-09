#pragma once
#include "Effect.h"

class Effect_Vehicle : public Effect
{
public:
	Effect_Vehicle(ID3D11Device* pDeviceInput, const std::wstring& pathInput);
	~Effect_Vehicle();

	void SetNormalMap(ID3D11ShaderResourceView* pResourceView);
	void SetSpecularMap(ID3D11ShaderResourceView* pResourceView);
	void SetGlossinessMap(ID3D11ShaderResourceView* pResourceView);
	void SetWorldMatrix(const Matrix& worldMatrix);
	void SetViewInverseMatrix(const Matrix& viewInverseMatrix);
private:
	ID3DX11EffectMatrixVariable* m_pMatWorldVariable{ nullptr };
	ID3DX11EffectMatrixVariable* m_pMatViewInverseVariable{ nullptr };

	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{nullptr};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{ nullptr };
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{ nullptr };
};

