#include "pch.h"
#include "Renderer.h"

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
		m_pMeshArr[0] = new Mesh(m_pDevice, "Resources/vehicle.obj", "Resources/vehicle_diffuse.png", "Resources/vehicle_normal.png", "Resources/vehicle_specular.png", "Resources/vehicle_gloss.png", {0,0,50.f});
		m_pMeshArr[1] = new Mesh(m_pDevice, "Resources/fireFX.obj", "Resources/fireFX_diffuse.png", { 0,0,50.f });

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
		if (!m_IsInitialized)
			return;


		//1. Clear RTV & DSV
		ColorRGB clearColor = ColorRGB{ 0,0,0.3f };
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		
		//2. Set Pipeline + Invoke drawcalls (=Render)
		for (Mesh* mesh : m_pMeshArr)
		{
			mesh->Render(m_pDeviceContext, m_pCamera);
		}
		

		//4 Present Backbuffer (Swap)
		m_pSwapChain->Present(0,0);
	}

	Mesh* Renderer::GetVehicleMeshPtr() const
	{
		return m_pMeshArr[0];
	}

	Mesh* Renderer::GetFireMeshPtr() const
	{
		return m_pMeshArr[1];
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

		//INFO
		/*
			USE F1!!!MSDN IS YOUR FRIEND
			Pattern that you'll come across often is that you need to create a description
			of something before creating an object using said description
			All resources/objects are created with a device, which then needs a description = the pattern

			We have to instruct DirectX to release memory! Not release it ourselves
			D3D11_TEXTURE2D_DESC just refers to a 2D array of colorRGB values	
			ID2D11Resource refers to the bits & bytes reserved for said resource
			DirectX knows how to interpret that resource (those bytes) using Resource Views
			resources can only be accessed through their respective VIEWS

			viewport specifies what part of the screen you want to render to. Think Splitscreen games
			RSSetViewport : RS stands for Rasterizer Space
			
			Slide 20 shows an example of DirectX telling us what hasn't been released yet!
			clear state
			flush
			release
			are the three functions you need to give to the deviceContext pointer.
			you need to release resources in reverse order!

			For 1 vertex, where does my POSITION start? == AlignedByteOffset.
			Line above is strange, it is to explain what AlignedByteOffsetis is on slide 27

			Slide32: stages that the hardware will do for you

			Your mesh needs to undergo some steps in the pipeline before you give it to the render function

			slide38: this is hlsl code, not C++ that's why it looks strange

			You need to make an effect class. I saw that Thomas also has an EffectUtils.h file so might need that aswell
			You need to make a mesh class. The constructor needs a ID3D11Device* and Effect*
			he has a function 'createresources' which at least makes a few buffers like a vertex buffer and index buffer
			slide42: top 2 snippets are for Effect. Bottom 2 snippets are for createresources in the mesh class
			Inside the mesh class you CAN have a render function (with the pDeviceContext* as parameter)  which you call in the render here. 
				1 Set primitive topology
				2 Set Input Layout
				3 Set Vertexbuffer
				4 Set IndexBuffer
				5 Draw
			You need pDeviceContext for all those. This is shown in slide43. Those 5 steps represent step 2 in the Render function above. 
			You could also just have those 5 steps in that render function. As long as you loop over your meshes
			Drawindexed means we're going to draw using index buffer

			Regarding the hlsl file. Open it in notepad or something and code it like that. Then that file's path
			is received as an input by the LoadEffect function in slide 40. (called assetFile)
		*/ 
	}
}

/*
	notes regarding Week 11
	put the float 4x4 global at the top of your file (gworldviewproj)

	the ID3DX11Effect* m_pEffect{} pointer in the effect class can be used to query data

	slide 3: I believe the righthandside code needs to be inside the effect class we created
	The left hand side is in the .hlsl file

	in his effect class he has the m_pmatworldviewprojvariable of type ID3DX11EffectMatrixVariable*
	
	you need to call setmatrix on this m_pmatworldviewprojvariable in slide 4. You'll have to cast the variable it needs

	you need to use the hlsl function mul which is used to multiply a float with a 4x4 matrix
	inside the VS_OUTPUT function
	instead of output.Position = ...
	it is output.position = mul(float4(input.position,1.f),gworldviewproj);

	for your quad. Create a new vertex type which has a position and a uv. You need 4 indices.
	e.g. the uv values need to be from topleft starting: (0,0)->(1,0)->(1,1)->(0,1)

	so you need a new struct VertexPosTex{} instead of posCol. Contains position and Vector2 uv.
	defined in the Mesh.h file above the class. This also means you need a separate hlsl file where
	the type of the uv is TEXTCOORD
	float3 Position:POSITION;
	float2 UV: TEXCOORD;

	for output use float4 Position: SV_POSITION;

	slide 5: implement
	virtual void BuildInputLayout() = 0;
	vitrtual void LoadEffectVariable() = 0;
	because you have different inputs (color VS uv)

	so he creates two child classes from the effect class, one called Effect_PosCol. One called Effect_PosTex
	because the input is different. So change the effect class to a base class.
	the 3 existing member variables of effect will need to be set to protected so they can be inherited
	
	the mesh constructor expects a vector of poscols. He templated it to also allow for a vector of postex objects
	however I think you can also just add a second constructor for the mesh class

	resources can only be interpreted through a resourceview

	the texture class needs to be able to return the ID3D11ShaderResourceView. We need it to bind the texture to the shader

	m_pSRV, seen in slide 6, is an object is your ID3D11ShaderResourceView, the most important part of your pipeline. It's a member variable of your
	texture class.

	every time he says 'shader' he's referring to the .hlsl file

	getSRV() = get shader resource view

	sample function of directX. The sampler stateS tells the sample function what we need to do if the uv 
	coordinate goes outside of bounds. Second argument is the uv coordinate. Will return a float4 object (RGBA)

	he has an m_pDevice variable stored in the effect class.
	You cant call virtual functions in the constructor, that's why he has an m_pDevice pointer stored.
	so he has an initialize function which is implemented in the child classes
	it loadEfect
	loadeffectvariale 
	BuildInputLayout


*/