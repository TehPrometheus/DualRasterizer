#include "pch.h"
#include "Effect_Vehicle.h"

Effect_Vehicle::Effect_Vehicle(ID3D11Device* pDeviceInput, const std::wstring& pathInput)
	: Effect(pDeviceInput,pathInput)
{
	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
	{
		std::wcout << L"m_pNormalMapVariable invalid\n";
	}

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
	{
		std::wcout << L"m_pSpecularMapVariable invalid\n";
	}

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
	{
		std::wcout << L"m_pGlossinessMapVariable invalid\n";
	}

	m_pMatWorldVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldVariable->IsValid())
	{
		std::wcout << L"variable gWorldMatrix invalid\n";
	}

	m_pMatViewInverseVariable = m_pEffect->GetVariableByName("gViewInverseMatrix")->AsMatrix();
	if (!m_pMatViewInverseVariable->IsValid())
	{
		std::wcout << L"variable gViewInverseMatrix invalid\n";
	}

}

Effect_Vehicle::~Effect_Vehicle()
{
}

void Effect_Vehicle::SetNormalMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pNormalMapVariable->IsValid())
		m_pNormalMapVariable->SetResource(pResourceView);
}

void Effect_Vehicle::SetSpecularMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pSpecularMapVariable->IsValid())
		m_pSpecularMapVariable->SetResource(pResourceView);
}

void Effect_Vehicle::SetGlossinessMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pGlossinessMapVariable->IsValid())
		m_pGlossinessMapVariable->SetResource(pResourceView);
}

void Effect_Vehicle::SetWorldMatrix(const Matrix& worldMatrix)
{
	m_pMatWorldVariable->SetMatrix(reinterpret_cast<const float*>(&worldMatrix));
}

void Effect_Vehicle::SetViewInverseMatrix(const Matrix& viewInverseMatrix)
{
	m_pMatViewInverseVariable->SetMatrix(reinterpret_cast<const float*>(&viewInverseMatrix));
}

