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

class Mesh final
{
public:
	Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath, const Vector3& position);
	Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossinessMapPath, const Vector3& position);
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
	void Update(const Timer* pTimer);
	void Render(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const;
	ID3D11InputLayout* GetInputLayoutPtr();
	Effect* GetEffectPtr() const;
	void ToggleRotation();
	bool GetIsRotating() const;

private:

	//------------------------------------------------
	// Member variables						
	//------------------------------------------------
	float m_VehicleYaw{};
	float m_AccuSec{};
	bool m_IsRotating{ true };

	Vector3 m_Position{};

	Effect* m_pEffect{};
	ID3D11InputLayout* m_pInputLayout{};

	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	uint32_t m_NumIndices{};

	Texture* m_pNormalMap		{ nullptr };
	Texture* m_pDiffuseMap		{ nullptr };
	Texture* m_pSpecularMap		{ nullptr };
	Texture* m_pGlossinessMap	{ nullptr };


	std::vector<Vertex_Vehicle> m_VehicleVertices{};
	std::vector<Vertex_Fire> m_FireVertices{};
	std::vector<uint32_t> m_Indices{};

	void ParseFireObj(const std::string& filename);

	bool ParseObj(const std::string& filename, std::vector<Vertex_Vehicle>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding = true);
};

