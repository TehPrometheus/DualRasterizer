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
		// -----------------------------------------------
		// Constructor and destructor
		// -----------------------------------------------
		Renderer(SDL_Window* pWindow);
		~Renderer();


		// -----------------------------------------------
		// Copy/move constructors and assignment operators
		// -----------------------------------------------
		Renderer(const Renderer&)					= delete;
		Renderer(Renderer&&) noexcept				= delete;
		Renderer& operator=(const Renderer&)		= delete;
		Renderer& operator=(Renderer&&) noexcept	= delete;


		//------------------------------------------------
		// Public member functions						
		//------------------------------------------------
		bool GetIsUsingDirectX() const;
		bool GetIsUniformColorEnabled() const;

		void Render() const;
		void ToggleRenderer();
		void HardwareRender() const;
		void SoftwareRender() const;
		void ToggleBackgroundColor();
		void Update(const Timer* pTimer);

		Mesh* GetFireMeshPtr() const;
		Mesh* GetVehicleMeshPtr() const;
	private:

		//------------------------------------------------
		// Member variables						
		//------------------------------------------------
		int								m_Width					{ 0 };
		int								m_Height				{ 0 };
		const static int				m_NROFMESHES			{ 2 };

		float							m_AspectRatio			{ 0 };
		float*							m_pDepthBufferPixels	{ nullptr };

		bool							m_IsInitialized			{ false };
		bool							m_IsUsingDirectX		{ true  };
		bool							m_IsUniformColorEnabled	{ false };

		SDL_Window*						m_pWindow				{ nullptr };
		SDL_Surface*					m_pFrontBuffer			{ nullptr };
		SDL_Surface*					m_pBackBuffer			{ nullptr };
		uint32_t*						m_pBackBufferPixels		{ nullptr };

		Camera*							m_pCamera				{ nullptr };

		ID3D11Device*					m_pDevice				{ nullptr };
		ID3D11DeviceContext*			m_pDeviceContext		{ nullptr };
		IDXGISwapChain*					m_pSwapChain			{ nullptr };

		ID3D11Texture2D*				m_pDepthStencilBuffer	{ nullptr };
		ID3D11DepthStencilView*			m_pDepthStencilView		{ nullptr };

		ID3D11RenderTargetView*			m_pRenderTargetView		{ nullptr };
		ID3D11Resource*					m_pRenderTargetBuffer	{ nullptr };

		std::array<Mesh*, m_NROFMESHES> m_pMeshArr				{};
		ColorRGB						m_BackgroundColor		{ colors::CornflowerBlue };


		//------------------------------------------------
		// Private member functions						
		//------------------------------------------------
		HRESULT InitializeDirectX();
	};
}
