#pragma once
#include "DataTypes.h"
#include "Effect.h"
#include "Camera.h"
#include <fstream>

struct Vertex_Fire
{
	Vector3 position{};
	Vector2 uv{};
};

struct Vertex_Vehicle
{
	Vector3 position{};
	Vector3 normal{};
	Vector3 tangent{};
	Vector2 uv{};
};

enum class ShadingMode
{
	ObservedArea = 0,
	Diffuse = 1,
	Specular = 2,
	Combined = 3
};

class Mesh final
{
public:
	// -----------------------------------------------
	// Constructors and destructor
	// -----------------------------------------------
	Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath, const Vector3& position);
	Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossinessMapPath, const Vector3& position, int windowWidth, int windowHeight);
	~Mesh();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Mesh(const Mesh& other)					= default;
	Mesh(Mesh&& other) noexcept				= default;
	Mesh& operator=(const Mesh& other)		= default;
	Mesh& operator=(Mesh&& other) noexcept	= default;

	//------------------------------------------------
	// Public member functions						
	//------------------------------------------------
	float				GetYaw() const;

	bool				GetIsRotating() const;
	bool				GetIsNormalMapEnabled() const;
	bool				GetVisibility() const;
	bool				GetDepthBufferBool() const;
	bool				GetBoundingBoxBool() const;

	void				ToggleNormalMap();
	void				ToggleRotation();
	void				ToggleVisibility();
	void				ToggleShadingMode();
	void				Update(const Timer* pTimer);
	void				ToggleDepthBufferVisualization();
	void				ToggleBoundingBoxVisualization();
	void				HardwareRender(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const;
	void				SoftwareRender(Camera* pCamera, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels);

	Vector3				GetPosition() const;
	Effect*				GetEffectPtr() const;
	ShadingMode			GetShadingMode() const;
	ID3D11InputLayout*	GetInputLayoutPtr() const;

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------
	const int					m_WindowWidth						{};
	const int					m_WindowHeight						{};
	const static int			m_NROFSHADINGMODES					{ 4 };
	
	float						m_VehicleYaw						{};
	float						m_AccuSec							{};

	bool						m_IsRotating						{ true };
	bool						m_IsNormalMapEnabled				{ true };
	bool						m_IsVisible							{ true };
	bool						m_IsDepthBufferVisualizationEnabled	{ false };
	bool						m_IsBoundingBoxVisualizationEnabled	{ false };

	Vector3						m_Position				{};
	ShadingMode					m_ShadingMode			{ ShadingMode::Combined };

	Effect*						m_pEffect				{ nullptr };
	ID3D11InputLayout*			m_pInputLayout			{ nullptr };

	ID3D11Buffer*				m_pVertexBuffer			{ nullptr };
	ID3D11Buffer*				m_pIndexBuffer			{ nullptr };
	uint32_t					m_NumIndices			{};

	Texture*					m_pNormalMap			{ nullptr };
	Texture*					m_pDiffuseMap			{ nullptr };
	Texture*					m_pSpecularMap			{ nullptr };
	Texture*					m_pGlossinessMap		{ nullptr };


	std::vector<uint32_t>		m_Indices				{};
	std::vector<Vertex_Out>		m_VehicleVerticesOut	{};
	std::vector<Vertex_Fire>	m_FireVertices			{};
	std::vector<Vertex_Vehicle> m_VehicleVertices		{};

	//------------------------------------------------
	// Private member functions						
	//------------------------------------------------
	float		RemapValue(float value, float min, float max) const;
	float		CalculateWeights(const Vector2& vertex1, const Vector2& vertex2, const Vector2& pixel, float area) const;

	void		ParseFireObj(const std::string& filename);
	void		VertexTransformationFunction(Camera* pCamera);
	void		RenderTriangle(std::vector<Vertex_Out>& triangle, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels) const;
	void		FindBoundingBoxCorners(Vector2& topLeft, Vector2& botRight, const std::vector<Vertex_Out>& triangle) const;

	bool		IsFrustumCullingRequired(const std::vector<Vertex_Out>& triangle) const;
	bool		IsTriangleCullingRequired(Camera* pCamera, const std::vector<Vertex_Out>& triangle) const;
	bool		ParseObj(const std::string& filename, std::vector<Vertex_Vehicle>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true);
	bool		IsPointInTriangle(const Vector3& weights) const;

	ColorRGB	PixelShading(const Vertex_Out& v) const;
	Vertex_Out	ConstructInterpolatedData(const Vector3& weights, const std::vector<Vertex_Out>& triangle, float wInterpolated) const;
};

