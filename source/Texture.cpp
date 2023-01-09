#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <iostream>
#include <assert.h>

namespace dae
{
	Texture::Texture(ID3D11Device* pDeviceInput, const char* filePath)
	{
		m_pSurface = IMG_Load(filePath);

		D3D11_TEXTURE2D_DESC desc;
		desc.Width = m_pSurface->w;
		desc.Height = m_pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = m_pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(m_pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(m_pSurface->h * m_pSurface->pitch);

		HRESULT result = pDeviceInput->CreateTexture2D(&desc, &initData, &m_pTexture);
		if (FAILED(result))
		{
			assert(false && "Unable to create Texture2D in constructor of Texture class");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		result = pDeviceInput->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pResourceViewTexture);
		if (FAILED(result))
		{
			assert(false && "Unable to create Shader Resource View in constructor of Texture class");
		}
	}

	Texture::~Texture()
	{
		m_pTexture->Release();
		m_pResourceViewTexture->Release();

		SDL_FreeSurface(m_pSurface);
	}


	ColorRGB Texture::Sample(const Vector2& uv) const
	{
		//TODO
		//Sample the correct texel for the given uv
		const float uvX{ Saturate(uv.x) };
		const float uvY{ Saturate(uv.y) };

		uint32_t* m_pSurfacePixels{ (uint32_t*)m_pSurface->pixels };

		ColorRGB color{};

		Uint8 r{};
		Uint8 g{};
		Uint8 b{};

		float width { static_cast<float>(m_pSurface->w) };
		float height{ static_cast<float>(m_pSurface->h) };

		uint32_t px{ static_cast<uint32_t>( width * uvX) },
				 py{ static_cast<uint32_t>(height * uvY) };

		SDL_GetRGB(m_pSurfacePixels[px + (py * m_pSurface->w)], m_pSurface->format, &r, &g, &b);

		color.r = static_cast<float>(r);
		color.g = static_cast<float>(g);
		color.b = static_cast<float>(b);

		color /= 255.f;

		return color;
	}

	ID3D11ShaderResourceView* Texture::GetResourceViewTexturePtr()
	{
		return m_pResourceViewTexture;
	}
}