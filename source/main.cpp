#include "pch.h"

#if defined(_DEBUG)
#include "vld.h"
#endif

#undef main
#include "Renderer.h"

using namespace dae;
using namespace std;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#define SharedColor FOREGROUND_RED|FOREGROUND_GREEN;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void PrintStartingInfo()
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
	cout	<< "[Key Bindings] - SHARED\n"
			<< "[F1] Toggle Rasterizer Mode (HARDWARE/SOFTWARE)\n"
			<< "[F2] Toggle Vehicle Rotation (ON/OFF)\n"
			<< "[F9] Cycle Cull Modes (BACK/FRONT/NONE)\n"
			<< "[F10] Toggle Uniform ClearColor [On/Off]\n"
			<< "[F11] Toggle Print FPS [On/Off]\n\n";

	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
	cout	<< "[Key Bindings] - HARDWARE\n"
			<< "[F3] Toggle FireFX (ON/OFF)\n"
			<< "[F4] Cycle Sampler State (POINT/LINEAR/ANISOTROPIC)\n\n";

	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED);
	cout	<< "[Key Bindings] - SOFTWARE\n"
			<< "[F5] Cycle Shading Mode (COMBINED/OBSERVED_AREA/DIFFUSE/SPECULAR)\n"
			<< "[F6] Toggle Normal Map (ON/OFF)\n"
			<< "[F7] Toggle DepthBuffer Visualisation (ON/OFF)\n"
			<< "[F8] Toggle BoundingBoxVisualisation (ON/OFF)\n\n";
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
	PrintStartingInfo();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool isFPSEnabled = true;
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
				if (e.key.keysym.scancode == SDL_SCANCODE_F1)
				{
					pRenderer->ToggleRenderer();
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
					cout << "**(SHARED) Rasterizer Mode = ";
					if (pRenderer->GetIsUsingDirectX())
					{
						cout << "HARDWARE\n";
					}
					else
					{
						cout << "SOFTWARE\n";
					}
				}

				if (e.key.keysym.scancode == SDL_SCANCODE_F2)
				{
					pRenderer->GetVehicleMeshPtr()->ToggleRotation();
					pRenderer->GetFireMeshPtr()->ToggleRotation();
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
					cout << "**(SHARED) Vehicle Rotation ";
					if (pRenderer->GetVehicleMeshPtr()->GetIsRotating())
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F3 && pRenderer->GetIsUsingDirectX())
				{
					pRenderer->GetFireMeshPtr()->ToggleVisibility();
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					cout << "**(HARDWARE) FireFX ";
					if (pRenderer->GetFireMeshPtr()->GetVisibility())
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F4 && pRenderer->GetIsUsingDirectX())
				{
					pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->ToggleSampleState();
					pRenderer->GetFireMeshPtr()->GetEffectPtr()->ToggleSampleState();
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					cout << "**(HARDWARE) Sampler Filter = ";
					switch (pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->GetSampleState())
					{
					case sampleState::point:
						cout << "POINT\n";
						break;
					case sampleState::linear:
						cout << "LINEAR\n";
						break;
					case sampleState::anisotropic:
						cout << "ANISOTROPIC\n";
						break;
					default:
						break;
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F5 && !pRenderer->GetIsUsingDirectX())
				{
					pRenderer->GetVehicleMeshPtr()->ToggleShadingMode();
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED);
					cout << "**(SOFTWARE) Shading Mode = ";

					switch (pRenderer->GetVehicleMeshPtr()->GetShadingMode())
					{
						case ShadingMode::ObservedArea:
							cout << "OBSERVED_AREA\n";
							break;
						case ShadingMode::Diffuse:
							cout << "DIFFUSE\n";
							break;
						case ShadingMode::Specular:
							cout << "SPECULAR\n";
							break;
						case ShadingMode::Combined:
							cout << "COMBINED\n";
							break;
						default:
							break;
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F6 && !pRenderer->GetIsUsingDirectX())
				{
					pRenderer->GetVehicleMeshPtr()->ToggleNormalMap();
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED);
					cout << "**(SOFTWARE) NormalMap ";

					if (pRenderer->GetVehicleMeshPtr()->GetIsNormalMapEnabled())
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
					}

				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F7 && !pRenderer->GetIsUsingDirectX())
				{
					pRenderer->GetVehicleMeshPtr()->ToggleDepthBufferVisualization();
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED);
					cout << "**(SOFTWARE) DepthBuffer Visualization ";

					if (pRenderer->GetVehicleMeshPtr()->GetDepthBufferBool())
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F8 && !pRenderer->GetIsUsingDirectX())
				{
					pRenderer->GetVehicleMeshPtr()->ToggleBoundingBoxVisualization();
					SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED);
					cout << "**(SOFTWARE) BoundingBox Visualization ";

					if (pRenderer->GetVehicleMeshPtr()->GetBoundingBoxBool())
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F9)
				{
					pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->ToggleCullingMode();
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
					cout << "**(SHARED) CullMode = ";
					switch (pRenderer->GetVehicleMeshPtr()->GetEffectPtr()->GetCullMode())
					{
					case cullMode::backCulling:
						cout << "BACK\n";
						break;
					case cullMode::frontCulling:
						cout << "FRONT\n";
						break;
					case cullMode::noCulling:
						cout << "NONE\n";
						break;
					default:
						break;
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F10)
				{
					pRenderer->ToggleBackgroundColor();
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
					cout << "**(SHARED) Uniform ClearColor ";
					if (pRenderer->GetIsUniformColorEnabled())
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
					}
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_F11)
				{
					isFPSEnabled = !isFPSEnabled;
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED);
					cout << "**(SHARED) Print FPS ";

					if (isFPSEnabled)
					{
						cout << "ON\n";
					}
					else
					{
						cout << "OFF\n";
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
		if (printTimer >= 1.f && isFPSEnabled)
		{
			printTimer = 0.f;
			SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
			cout << "dFPS: " << pTimer->GetdFPS() << endl;
		}
	}
	pTimer->Stop();
	SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}