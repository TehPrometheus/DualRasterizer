#pragma once
#include "ColorRGB.h"
#include "DataTypes.h"
#include "Mesh.h"
#include "Camera.h"
#include <array>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();


		// -----------------------------------------------
		// Copy/move constructors and assignment operators
		// -----------------------------------------------
		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;


		//------------------------------------------------
		// Public member functions						
		//------------------------------------------------
		void Update(const Timer* pTimer);
		void Render() const;
		void HardwareRender() const;
		void SoftwareRender() const;
		void ToggleRenderer();
		void ToggleBackgroundColor();
		Mesh* GetVehicleMeshPtr() const;
		Mesh* GetFireMeshPtr() const;
		bool GetIsUniformColorEnabled() const;
		bool GetIsUsingDirectX() const;
	private:

		//------------------------------------------------
		// Member variables						
		//------------------------------------------------

		SDL_Window* m_pWindow{};
		Camera* m_pCamera{};
		ColorRGB m_BackgroundColor{ colors::CornflowerBlue };

		int m_Width{};
		int m_Height{};
		const static int m_NROFMESHES{ 2 };

		float m_AspectRatio{};
		
		bool m_IsInitialized{ false };
		bool m_IsUsingDirectX{ true };
		bool m_IsUniformColorEnabled{ false };

		std::array<Mesh*, m_NROFMESHES> m_pMeshArr{};

		ID3D11Device* m_pDevice{ nullptr };
		ID3D11DeviceContext* m_pDeviceContext{ nullptr };
		IDXGISwapChain* m_pSwapChain{ nullptr };

		ID3D11Texture2D* m_pDepthStencilBuffer{ nullptr };
		ID3D11DepthStencilView* m_pDepthStencilView{ nullptr };

		ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
		ID3D11Resource* m_pRenderTargetBuffer{ nullptr };


		SDL_Surface* m_pFrontBuffer{ nullptr };
		SDL_Surface* m_pBackBuffer{ nullptr };
		uint32_t* m_pBackBufferPixels{};

		float* m_pDepthBufferPixels{};

		//------------------------------------------------
		// Private member functions						
		//------------------------------------------------
		HRESULT InitializeDirectX();
	};
}
