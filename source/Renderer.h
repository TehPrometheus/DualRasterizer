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
		Mesh* GetVehicleMeshPtr() const;
		Mesh* GetFireMeshPtr() const;
	private:

		//------------------------------------------------
		// Member variables						
		//------------------------------------------------

		SDL_Window* m_pWindow{};
		Camera* m_pCamera{};
		
		int m_Width{};
		int m_Height{};
		static const int m_NROFMESHES{ 2 };

		float m_AspectRatio{};
		
		bool m_IsInitialized{ false };


		std::array<Mesh*, m_NROFMESHES> m_pMeshArr{};

		ID3D11Device* m_pDevice{ nullptr };
		ID3D11DeviceContext* m_pDeviceContext{ nullptr };
		IDXGISwapChain* m_pSwapChain{ nullptr };

		ID3D11Texture2D* m_pDepthStencilBuffer{ nullptr };
		ID3D11DepthStencilView* m_pDepthStencilView{ nullptr };

		ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
		ID3D11Resource* m_pRenderTargetBuffer{ nullptr };

		//------------------------------------------------
		// Private member functions						
		//------------------------------------------------
		HRESULT InitializeDirectX();
	};
}
