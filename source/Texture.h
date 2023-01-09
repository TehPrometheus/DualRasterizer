#pragma once
#include "pch.h"
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
	struct Vector2;

	class Texture final
	{
	public:
		Texture(ID3D11Device* pDeviceInput, const char* filePath);
		~Texture();

		// -----------------------------------------------
		// Copy/move constructors and assignment operators
		// -----------------------------------------------
		Texture(const Texture& other)					= delete;
		Texture(Texture&& other) noexcept				= delete;
		Texture& operator=(const Texture& other)		= delete;
		Texture& operator=(Texture&& other) noexcept	= delete;

		//------------------------------------------------
		// Public member functions						
		//------------------------------------------------
		ColorRGB Sample(const Vector2& uv) const;
		ID3D11ShaderResourceView* GetResourceViewTexturePtr();

	private:
		//------------------------------------------------
		// Member Variables						
		//------------------------------------------------

		SDL_Surface* m_pSurface;
		ID3D11Texture2D* m_pTexture;
		ID3D11ShaderResourceView* m_pResourceViewTexture{ nullptr };
	};
}