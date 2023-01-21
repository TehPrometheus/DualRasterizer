#include "pch.h"
#include "Effect.h"
#include <assert.h>

Effect::Effect(ID3D11Device* pDeviceInput, const std::wstring& pathInput)
{
	m_pEffect = LoadEffect(pDeviceInput, pathInput);

	BindShaderTechniques();

	BindShaderMatrices();

	BindShaderMaps();
}

Effect::~Effect()
{
	m_pActiveTechnique->Release();
	m_pEffect->Release();
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);
	
	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
			{
				ss << pErrors[i];
			}

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;

		}
		else
		{
			std::wstringstream ss;
			ss << "Effectloader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechniquePtr() const
{
	return m_pActiveTechnique;
}


void Effect::SetWorldViewProjectionMatrix(const Matrix& worldViewProjectionMatrix)
{
	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<const float*>(&worldViewProjectionMatrix));
}

void Effect::SetDiffuseMap(Texture* pDiffuseTexture)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetResourceViewTexturePtr());
}

void Effect::ToggleSampleState()
{
	m_SampleState = static_cast<sampleState>((static_cast<int>(m_SampleState) + 1) % m_NROFSAMPLESTATES);

	switch (m_SampleState)
	{
	case sampleState::point:
		switch (m_CullMode)
		{
		case cullMode::backCulling:
			m_pActiveTechnique = m_pPointBackCullTechnique;
			break;
		case cullMode::frontCulling:
			m_pActiveTechnique = m_pPointFrontCullTechnique;
			break;
		case cullMode::noCulling:
			m_pActiveTechnique = m_pPointNoCullTechnique;
			break;
		default:
			break;
		}
		break;
	case sampleState::linear:
		switch (m_CullMode)
		{
		case cullMode::backCulling:
			m_pActiveTechnique = m_pLinearBackCullTechnique;
			break;
		case cullMode::frontCulling:
			m_pActiveTechnique = m_pLinearFrontCullTechnique;
			break;
		case cullMode::noCulling:
			m_pActiveTechnique = m_pLinearNoCullTechnique;
			break;
		default:
			break;
		}
		break;
	case sampleState::anisotropic:
		switch (m_CullMode)
		{
		case cullMode::backCulling:
			m_pActiveTechnique = m_pAnisotropicBackCullTechnique;
			break;
		case cullMode::frontCulling:
			m_pActiveTechnique = m_pAnisotropicFrontCullTechnique;
			break;
		case cullMode::noCulling:
			m_pActiveTechnique = m_pAnisotropicNoCullTechnique;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

}

void Effect::ToggleCullingMode()
{
	m_CullMode = static_cast<cullMode>((static_cast<int>(m_CullMode) + 1) % m_NROFCULLMODES);

	switch (m_CullMode)
	{
	case cullMode::backCulling:
		switch (m_SampleState)
		{
		case sampleState::point:
			m_pActiveTechnique = m_pPointBackCullTechnique;
			break;
		case sampleState::linear:
			m_pActiveTechnique = m_pLinearBackCullTechnique;
			break;
		case sampleState::anisotropic:
			m_pActiveTechnique = m_pAnisotropicBackCullTechnique;
			break;
		default:
			break;
		}
		break;
	case cullMode::frontCulling:
		switch (m_SampleState)
		{
		case sampleState::point:
			m_pActiveTechnique = m_pPointFrontCullTechnique;
			break;
		case sampleState::linear:
			m_pActiveTechnique = m_pLinearFrontCullTechnique;
			break;
		case sampleState::anisotropic:
			m_pActiveTechnique = m_pAnisotropicFrontCullTechnique;
			break;
		default:
			break;
		}
		break;
	case cullMode::noCulling:
		switch (m_SampleState)
		{
		case sampleState::point:
			m_pActiveTechnique = m_pPointNoCullTechnique;
			break;
		case sampleState::linear:
			m_pActiveTechnique = m_pLinearNoCullTechnique;
			break;
		case sampleState::anisotropic:
			m_pActiveTechnique = m_pAnisotropicNoCullTechnique;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

sampleState Effect::GetSampleState() const
{
	return m_SampleState;
}

cullMode Effect::GetCullMode() const
{
	return m_CullMode;
}

void Effect::BindShaderTechniques()
{
	m_pPointBackCullTechnique = m_pEffect->GetTechniqueByName("PointBackCullTechnique");
	if (!m_pPointBackCullTechnique->IsValid())
	{
		std::wcout << L"PointBackCullTechnique invalid\n";
	}

	m_pLinearBackCullTechnique = m_pEffect->GetTechniqueByName("LinearBackCullTechnique");
	if (!m_pLinearBackCullTechnique->IsValid())
	{
		std::wcout << L"LinearBackCullTechnique invalid\n";
	}

	m_pAnisotropicBackCullTechnique = m_pEffect->GetTechniqueByName("AnisotropicBackCullTechnique");
	if (!m_pAnisotropicBackCullTechnique->IsValid())
	{
		std::wcout << L"AnisotropicBackCullTechnique invalid\n";
	}

	m_pPointFrontCullTechnique = m_pEffect->GetTechniqueByName("PointFrontCullTechnique");
	if (!m_pPointFrontCullTechnique->IsValid())
	{
		std::wcout << L"PointFrontCullTechnique invalid\n";
	}

	m_pLinearFrontCullTechnique = m_pEffect->GetTechniqueByName("LinearFrontCullTechnique");
	if (!m_pLinearFrontCullTechnique->IsValid())
	{
		std::wcout << L"LinearFrontCullTechnique invalid\n";
	}

	m_pAnisotropicFrontCullTechnique = m_pEffect->GetTechniqueByName("AnisotropicFrontCullTechnique");
	if (!m_pAnisotropicFrontCullTechnique->IsValid())
	{
		std::wcout << L"AnisotropicFrontCullTechnique invalid\n";
	}

	m_pPointNoCullTechnique = m_pEffect->GetTechniqueByName("PointNoCullTechnique");
	if (!m_pPointNoCullTechnique->IsValid())
	{
		std::wcout << L"PointNoCullTechnique invalid\n";
	}

	m_pLinearNoCullTechnique = m_pEffect->GetTechniqueByName("LinearNoCullTechnique");
	if (!m_pLinearNoCullTechnique->IsValid())
	{
		std::wcout << L"LinearNoCullTechnique invalid\n";
	}

	m_pAnisotropicNoCullTechnique = m_pEffect->GetTechniqueByName("AnisotropicNoCullTechnique");
	if (!m_pAnisotropicNoCullTechnique->IsValid())
	{
		std::wcout << L"AnisotropicNoCullTechnique invalid\n";
	}

	//Set the active technique to point sampling by default
	m_pActiveTechnique = m_pPointNoCullTechnique;
}

void Effect::BindShaderMatrices()
{
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"variable gWorldViewProj invalid\n";
	}

}

void Effect::BindShaderMaps()
{
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
	{
		std::wcout << L"m_pDiffuseMapVariable invalid\n";
	}

}
