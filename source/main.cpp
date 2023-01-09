#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	SDL_Window* pWindow = SDL_CreateWindow(
		"DirectX - Tanguy Aerts - 2DAE07",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);
	//enum class sampleState
	//{
	//	point,
	//	linear,
	//	anisotropic
	//};

	//enum class cullMode
	//{
	//	backCulling,
	//	frontCulling,
	//	noCulling
	//};

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->GetVehicleMeshPtr()->ToggleRotation();
					pRenderer->GetFireMeshPtr()->ToggleRotation();

					if (pRenderer->GetVehicleMeshPtr()->GetIsRotating())
					{
						std::cout << "Rotation Enabled\n";
					}
					else
					{
						std::cout << "Rotation Disabled\n";
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->ToggleSampleState();
					pRenderer->GetFireMeshPtr()->GetEffectPtr()->ToggleSampleState();

					switch (pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->GetSampleState())
					{
					case sampleState::point:
						std::cout << "Sample State = Point Sampling\n";
						break;
					case sampleState::linear:
						std::cout << "Sample State = Linear Sampling\n";
						break;
					case sampleState::anisotropic:
						std::cout << "Sample State = Anisotropic Sampling\n";
						break;
					default:
						break;
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->ToggleCullingMode();

					switch (pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->GetCullMode())
					{
					case cullMode::backCulling:
						std::cout << "Cull Mode = Back Culling\n";
						break;
					case cullMode::frontCulling:
						std::cout << "Cull Mode = Front Culling\n";
						break;
					case cullMode::noCulling:
						std::cout << "Cull Mode = No Culling\n";
						break;
					default:
						break;
					}
				}
				break;
			default: ;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "dFPS: " << pTimer->GetdFPS() << std::endl;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}