#include "pch.h"
#include "Renderer.h"

#define vehicleMesh m_pMeshArr[0];
#define fireMesh m_pMeshArr[1];



namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize Window
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize Aspect Ratio
		m_AspectRatio = (float)m_Width / (float)m_Height;

		//Initialize Camera
		m_pCamera = new Camera(45.f, {0,0,-50.f}, m_AspectRatio);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		//Initialize Meshes
		m_pMeshArr[0] = new Mesh(m_pDevice, "Resources/vehicle.obj", "Resources/vehicle_diffuse.png", "Resources/vehicle_normal.png", "Resources/vehicle_specular.png", "Resources/vehicle_gloss.png", {0,0,50.f}, m_Width, m_Height);
		m_pMeshArr[1] = new Mesh(m_pDevice, "Resources/fireFX.obj", "Resources/fireFX_diffuse.png", { 0,0,50.f });

		//Create Buffers for software rasterizer
		m_pFrontBuffer = SDL_GetWindowSurface(pWindow);
		m_pBackBuffer = SDL_CreateRGBSurface(0, m_Width, m_Height, 32, 0, 0, 0, 0);
		m_pBackBufferPixels = (uint32_t*)m_pBackBuffer->pixels;
		m_pDepthBufferPixels = new float[m_Width * m_Height];
	}

	Renderer::~Renderer()
	{
		// Release all the objects created by calling DirectX release functions
		m_pDevice->Release();

		if(m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		m_pSwapChain->Release();
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilView->Release();
		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();

		for (Mesh*& mesh : m_pMeshArr)
		{
			delete mesh;
			mesh = nullptr;
		}
		delete m_pCamera;
		delete m_pDepthBufferPixels;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_pCamera->Update(pTimer);

		for (Mesh*& mesh : m_pMeshArr)
		{
			mesh->Update(pTimer);
		}
	}


	void Renderer::Render() const
	{
		if (m_IsUsingDirectX)
		{
			HardwareRender();
		}
		else
		{
			SoftwareRender();
		}
		
	}

	void Renderer::HardwareRender() const
	{
		if (!m_IsInitialized)
			return;

		//1. Clear RTV & DSV
		ColorRGB clearColor = ColorRGB{ m_BackgroundColor };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);


		//2. Set Pipeline + Invoke drawcalls (=Render)
		for (Mesh* mesh : m_pMeshArr)
		{
			mesh->HardwareRender(m_pDeviceContext, m_pCamera);
		}

		//3 Present Backbuffer (Swap)
		m_pSwapChain->Present(0, 0);
	}

	void Renderer::SoftwareRender() const
	{
		//@START
		std::fill_n(m_pDepthBufferPixels, m_Width * m_Height, FLT_MAX);
		SDL_FillRect(m_pBackBuffer, NULL, SDL_MapRGB(m_pBackBuffer->format,static_cast<Uint8>(m_BackgroundColor.r * 255), static_cast<Uint8>(m_BackgroundColor.g * 255), static_cast<Uint8>(m_BackgroundColor.b * 255)));

		//Lock BackBuffer
		SDL_LockSurface(m_pBackBuffer);

		//Perform render
		m_pMeshArr[0]->SoftwareRender(m_pCamera, m_pBackBuffer, m_pBackBufferPixels, m_pDepthBufferPixels);

		//@END
		//Update SDL Surface
		SDL_UnlockSurface(m_pBackBuffer);
		SDL_BlitSurface(m_pBackBuffer, 0, m_pFrontBuffer, 0);
		SDL_UpdateWindowSurface(m_pWindow);


	}

	void Renderer::ToggleRenderer()
	{
		m_IsUsingDirectX = !m_IsUsingDirectX;

		if (m_IsUniformColorEnabled) m_BackgroundColor = colors::DarkGrey;
		else if (m_IsUsingDirectX) m_BackgroundColor = colors::CornflowerBlue;
		else m_BackgroundColor = colors::LightGrey;
	}

	void Renderer::ToggleBackgroundColor()
	{
		m_IsUniformColorEnabled = !m_IsUniformColorEnabled;

		if (m_IsUniformColorEnabled) m_BackgroundColor = colors::DarkGrey;
		else if (m_IsUsingDirectX) m_BackgroundColor = colors::CornflowerBlue;
		else m_BackgroundColor = colors::LightGrey;
	}

	Mesh* Renderer::GetVehicleMeshPtr() const
	{
		return vehicleMesh;
	}

	Mesh* Renderer::GetFireMeshPtr() const
	{
		return fireMesh;
	}

	bool Renderer::GetIsUsingDirectX() const
	{
		return m_IsUsingDirectX;
	}


	HRESULT Renderer::InitializeDirectX()
	{
		//TODO: 1. Create Device & DeviceContext
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
		uint32_t createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, &featureLevel, 
											1, D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);
		if (FAILED(result))
		{
			return result;
		}

		//TODO 2: Create DXGI Factory. Creates a pointer to an interface that can query information from the GPU. e.g. what is your clock speed?
		// We need it only to create the swapchain. Afterwards we'll release it
		IDXGIFactory1* pDxgiFactory{};
		result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&pDxgiFactory));
		if (FAILED(result))
		{
			return result;
		}



		//TODO 3: Create Swapchain
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

			// You'll set the framerate here at 60. But don't worry, that should be overwritten later 

			// Bufferuseage: The swapchain is going to create a buffer for us. It's setting means the buffer that we're going to attach to the render pipeline
			// Buffercount = 1 means that you are double buffering


		//TODO 4: get the handle HWND from the SDL Backbuffer
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version)
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		//TODO 5: Create Swapchain function, call it from the pDxgiFactory
		result = pDxgiFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
		if (FAILED(result))
		{
			return result;
		}



		//TODO 6: cleanup factory
		pDxgiFactory->Release();

		//TODO 7: create DepthStencil (DS) & DepthstencilView (DSV) Resource
		// depthstencil is een buffer. Een combinatie van 2 dingen. Hij wordt gebruikt als mask over uw image voor special effects zoals die fire effect
		// depthstencil = depthbuffer + stencilbuffer = buffer voor opaque depth + buffer voor special effects. Superverwarrende naam.
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // D24= 24 bits, een unsigned normalized int om depth bij te houden. S8=8bits een unsigned int voor
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilViewDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
		if (FAILED(result))
		{
			return result;
		}

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
		if (FAILED(result))
		{
			return result;
		}

		//TODO 8: Create rendertarget & rendertargetview
			//resource
		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
		if (FAILED(result))
		{
			return result;
		}

			//view
		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(result))
		{
			return result;
		}

		//TODO 9: Bind RTV & DSV to output merger stage using the m_pDeviceContext
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


		//TODO10: Set viewport
		D3D11_VIEWPORT viewport{};
		viewport.Width = static_cast<float>(m_Width);
		viewport.Height = static_cast<float>(m_Height);
		viewport.TopLeftX = 0.f;
		viewport.TopLeftY = 0.f;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_pDeviceContext->RSSetViewports(1, &viewport);

		return S_OK;
	}

}