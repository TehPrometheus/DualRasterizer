#include "pch.h"
#include "Mesh.h"
#include "Effect_Vehicle.h"
#include "Effect_Fire.h"
#include <assert.h>

Mesh::Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath, const Vector3& position)
{
	m_pEffect = new Effect_Fire(pDeviceInput,L"Resources/Fire_Shader.fx");

	ParseFireObj(objPath);

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 2 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechniquePtr()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDeviceInput->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false && "Unable to create input layout in constructor of Mesh class");
	}


	//Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_Fire) * static_cast<uint32_t>(m_FireVertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_FireVertices.data();
	
	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create vertex buffer in constructor of Mesh class");
	}

	//Create index buffer
	m_NumIndices = static_cast<uint32_t>(m_Indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initData.pSysMem = m_Indices.data();

	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create index buffer in constructor of Mesh class");
	}

	m_pDiffuseMap = new dae::Texture(pDeviceInput, diffuseMapPath.c_str());
	m_pEffect->SetDiffuseMap(m_pDiffuseMap);
}

Mesh::Mesh(ID3D11Device* pDeviceInput, const std::string& objPath, const std::string& diffuseMapPath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossinessMapPath, const Vector3& position, int windowWidth, int windowHeight)
	: m_WindowWidth{windowWidth}
	, m_WindowHeight{windowHeight}
{
	m_pEffect = new Effect_Vehicle(pDeviceInput,L"Resources/Vehicle_Shader.fx");

	//Parse OBJ
	ParseObj(objPath,m_VehicleVertices,m_Indices);

	//Set software vertices
	m_VehicleVerticesOut.resize(m_VehicleVertices.size());

	//Create Vertex Layout
	static constexpr uint32_t numElements{ 4 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "NORMAL";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TANGENT";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TEXCOORD";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 36;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create Input Layout
	D3DX11_PASS_DESC passDesc{};
	m_pEffect->GetTechniquePtr()->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDeviceInput->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
	{
		assert(false && "Unable to create input layout in constructor of Mesh class");
	}

	//Create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_Vehicle) * static_cast<uint32_t>(m_VehicleVertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_VehicleVertices.data();
	
	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create vertex buffer in constructor of Mesh class");
	}

	//Create index buffer
	m_NumIndices = static_cast<uint32_t>(m_Indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initData.pSysMem = m_Indices.data();

	result = pDeviceInput->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		assert(false && "Unable to create index buffer in constructor of Mesh class");
	}

	m_pDiffuseMap		= new dae::Texture(pDeviceInput, diffuseMapPath.c_str());
	m_pNormalMap		= new dae::Texture(pDeviceInput, normalMapPath.c_str());
	m_pSpecularMap		= new dae::Texture(pDeviceInput, specularMapPath.c_str());
	m_pGlossinessMap	= new dae::Texture(pDeviceInput, glossinessMapPath.c_str());

	m_pEffect->SetDiffuseMap(m_pDiffuseMap);
	Effect_Vehicle* vehicleEffect{ dynamic_cast<Effect_Vehicle*>(m_pEffect) };
	if (vehicleEffect)
	{
		vehicleEffect->SetNormalMap(m_pNormalMap->GetResourceViewTexturePtr());
		vehicleEffect->SetSpecularMap(m_pSpecularMap->GetResourceViewTexturePtr());
		vehicleEffect->SetGlossinessMap(m_pGlossinessMap->GetResourceViewTexturePtr());
	}

}

Mesh::~Mesh()
{
	delete m_pEffect;
	delete m_pDiffuseMap;
	delete m_pNormalMap;
	delete m_pSpecularMap;
	delete m_pGlossinessMap;
	m_pVertexBuffer->Release();
	m_pIndexBuffer->Release();
	m_pInputLayout->Release();
}


void Mesh::Update(const Timer* pTimer)
{
	if (m_IsRotating)
	{
		m_AccuSec += pTimer->GetElapsed();
	}

	m_VehicleYaw = PI_DIV_4 * m_AccuSec;
}

void Mesh::HardwareRender(ID3D11DeviceContext* pDeviceContext, Camera* pCamera) const
{
	if (!m_IsVisible) return;

	//1. Set Matrices
	dae::Matrix worldMatrix = Matrix::CreateTranslation(m_Position) * Matrix::CreateRotationY(m_VehicleYaw);
	dae::Matrix viewMatrix{ pCamera->GetViewMatrix().Inverse() };
	dae::Matrix inverseViewMatrix{ pCamera->GetViewMatrix() };
	dae::Matrix projectionMatrix{ pCamera->GetProjectionMatrix() };
	dae::Matrix worldViewProjectionMatrix{ worldMatrix * viewMatrix * projectionMatrix};

	m_pEffect->SetWorldViewProjectionMatrix(worldViewProjectionMatrix);

	Effect_Vehicle* vehicleEffect{ dynamic_cast<Effect_Vehicle*>(m_pEffect) };

	if (vehicleEffect)
	{
		vehicleEffect->SetWorldMatrix(worldMatrix);
		vehicleEffect->SetViewInverseMatrix(inverseViewMatrix);
	}

	//1. Set Primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. Set Input Layout
	pDeviceContext->IASetInputLayout(m_pInputLayout);

	//3. Set VertexBuffer
	UINT stride{};

	if (vehicleEffect)
	{
		stride = sizeof(Vertex_Vehicle);
	}
	else
	{
		stride = sizeof(Vertex_Fire);
	}
	
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. Set IndexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. Draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	m_pEffect->GetTechniquePtr()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; p++)
	{
		m_pEffect->GetTechniquePtr()->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}


}

void Mesh::SoftwareRender(Camera* pCamera, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels) 
{
	VertexTransformationFunction(pCamera);

	for (uint32_t idx = 0; idx < m_NumIndices; idx += 3)
	{
		std::vector<Vertex_Out> triangle{};

		triangle.push_back(m_VehicleVerticesOut[ m_Indices[idx + 0] ]);
		triangle.push_back(m_VehicleVerticesOut[ m_Indices[idx + 1] ]);
		triangle.push_back(m_VehicleVerticesOut[ m_Indices[idx + 2] ]);

		// Optimisation Stage
		if (IsFrustumCullingRequired(triangle))
		{
			continue;
		}

		// NDC -> Screen Space Coordinates
		for (Vertex_Out& vertex : triangle)
		{
			vertex.position.x = ((1 + vertex.position.x) / 2) * m_WindowWidth;
			vertex.position.y = ((1 - vertex.position.y) / 2) * m_WindowHeight;
		}

		// Rasterization Stage
		RenderTriangle(triangle, pBackBuffer, pBackBufferPixels, pDepthBufferPixels);
	}

}

void Mesh::VertexTransformationFunction(Camera* pCamera)
{
	Matrix worldMatrix = Matrix::CreateTranslation(m_Position) * Matrix::CreateRotationY(m_VehicleYaw);
	Matrix worldViewProjectionMatrix{ worldMatrix * pCamera->GetViewMatrix().Inverse() * pCamera->GetProjectionMatrix()};

	for (size_t idx = 0; idx < m_VehicleVertices.size(); ++idx)
	{
		Vertex_Out vertex_out{ {m_VehicleVertices[idx].position.x,
								m_VehicleVertices[idx].position.y,
								m_VehicleVertices[idx].position.z,
								1}
								, 
								m_VehicleVertices[idx].uv
								,
								{colors::White}
								,
								m_VehicleVertices[idx].normal
								,
								m_VehicleVertices[idx].tangent
							};

		// Position Transformation To NDC
		vertex_out.position = worldViewProjectionMatrix.TransformPoint(vertex_out.position);

		// Perspective Divide
		vertex_out.position.x /= vertex_out.position.w;
		vertex_out.position.y /= vertex_out.position.w;
		vertex_out.position.z /= vertex_out.position.w;

		// Normal & Tangent Transformation To World Space
		vertex_out.normal = worldMatrix.TransformVector(vertex_out.normal);
		vertex_out.tangent = worldMatrix.TransformVector(vertex_out.tangent);

		// Create ViewDirection
		vertex_out.viewDirection = worldMatrix.TransformVector(m_VehicleVertices[idx].position) - pCamera->GetOrigin();

		m_VehicleVerticesOut[idx] = vertex_out;
	}
}

bool Mesh::IsFrustumCullingRequired(const std::vector<Vertex_Out>& triangle) const
{
	for (const Vertex_Out& vertex : triangle)
	{
		if (vertex.position.x < -1.f || vertex.position.x > 1.f ||
			vertex.position.y < -1.f || vertex.position.y > 1.f ||
			vertex.position.z <  0.f || vertex.position.z > 1.f)
		{
			return true;
		}
	}

	return false;
}

void Mesh::RenderTriangle(std::vector<Vertex_Out>& triangle, SDL_Surface* pBackBuffer, uint32_t* pBackBufferPixels, float* pDepthBufferPixels) const
{
	const Vector2 v0{ triangle[0].position.x, triangle[0].position.y };
	const Vector2 v1{ triangle[1].position.x, triangle[1].position.y };
	const Vector2 v2{ triangle[2].position.x, triangle[2].position.y };

	Vector2 topLeft{};
	Vector2 botRight{};
	FindBoundingBoxCorners(topLeft, botRight, triangle);


	for (int py{ int(topLeft.y) }; py < int(botRight.y); ++py)
	{
		for (int px{ int(topLeft.x) }; px < int(botRight.x); ++px)
		{
			Vector3 weights{};
			ColorRGB finalColor{};
			const Vector2 pixel_ssc{ float(px) + 0.5f , float(py) + 0.5f };

			const float area{ Vector2::Cross(Vector2(v1, v2), Vector2(v1, v0)) };

			weights.x = CalculateWeights(v1, v2, pixel_ssc, area);
			weights.y = CalculateWeights(v2, v0, pixel_ssc, area);
			weights.z = CalculateWeights(v0, v1, pixel_ssc, area);

			if (IsPointInTriangle(weights))
			{
				const float zBufferValue{ 1 / ((weights.x / triangle[0].position.z) + (weights.y / triangle[1].position.z) + (weights.z / triangle[2].position.z)) };

				if (zBufferValue < pDepthBufferPixels[px + py * m_WindowWidth])
				{
					pDepthBufferPixels[px + (py * m_WindowWidth)] = zBufferValue;

					const float wInterpolated{ 1 / ((weights.x / triangle[0].position.w) + (weights.y / triangle[1].position.w) + (weights.z / triangle[2].position.w)) };

					Vector2 uvInterpolated{ wInterpolated * (weights.x * (triangle[0].uv / triangle[0].position.w) +
																weights.y * (triangle[1].uv / triangle[1].position.w) +
																weights.z * (triangle[2].uv / triangle[2].position.w)) };

					Vertex_Out interpolatedData{};
					interpolatedData.uv = uvInterpolated;
					interpolatedData.color = m_pDiffuseMap->Sample(uvInterpolated);
					interpolatedData.normal = ((triangle[0].normal / triangle[0].position.w) * weights.x +
						(triangle[1].normal / triangle[1].position.w) * weights.y +
						(triangle[2].normal / triangle[2].position.w) * weights.z) * wInterpolated;
					interpolatedData.normal.Normalize();

					interpolatedData.tangent = ((triangle[0].tangent / triangle[0].position.w) * weights.x +
						(triangle[1].tangent / triangle[1].position.w) * weights.y +
						(triangle[2].tangent / triangle[2].position.w) * weights.z) * wInterpolated;
					interpolatedData.tangent.Normalize();

					interpolatedData.viewDirection = ((triangle[0].viewDirection / triangle[0].position.w) * weights.x +
						(triangle[1].viewDirection / triangle[1].position.w) * weights.y +
						(triangle[2].viewDirection / triangle[2].position.w) * weights.z) * wInterpolated;
					interpolatedData.viewDirection.Normalize();


					finalColor = PixelShading(interpolatedData);



					//finalColor = m_pTextureVehicle->Sample(uvInterpolated);
					finalColor.MaxToOne();

					pBackBufferPixels[px + (py * m_WindowWidth)] = SDL_MapRGB(pBackBuffer->format,
						static_cast<uint8_t>(finalColor.r * 255),
						static_cast<uint8_t>(finalColor.g * 255),
						static_cast<uint8_t>(finalColor.b * 255));
				}
			}
		}
	}
}

ColorRGB Mesh::PixelShading(const Vertex_Out& v) const
{
	const Vector3 lightDirection{ 0.577f, -0.577f ,0.577f };

	//Construct correct normal
	Vector3 binormal{ Vector3::Cross(v.normal,v.tangent) };
	Matrix tangentSpaceAxis = Matrix{ v.tangent,binormal,v.normal,Vector3{} };
	ColorRGB normalMapSample{ m_pNormalMap->Sample(v.uv) };
	Vector3 sampledNormal{ normalMapSample.r, normalMapSample.g, normalMapSample.b };
	sampledNormal.x = (2.f * sampledNormal.x) - 1.f;
	sampledNormal.y = (2.f * sampledNormal.y) - 1.f;
	sampledNormal.z = (2.f * sampledNormal.z) - 1.f;
	sampledNormal = tangentSpaceAxis.TransformVector(sampledNormal);


	//LambertCosine
	float lambertCosine{};
	if (m_IsNormalMapEnabled)
	{
		lambertCosine = std::max(Vector3::Dot(sampledNormal.Normalized(), -lightDirection), 0.f);
	}
	else
	{
		lambertCosine = std::max(Vector3::Dot(v.normal, -lightDirection), 0.f);
	}


	//Lambert Diffuse
	ColorRGB lambertDiffuse{};
	const float kd{ 7.f }; 
	lambertDiffuse = m_pDiffuseMap->Sample(v.uv) * kd / static_cast<float>(M_PI);


	//Phong
	const Vector3 reflect{ lightDirection - 2 * (Vector3::Dot(v.normal,lightDirection)) * v.normal };
	const float cosine{ std::max(Vector3::Dot(reflect, -v.viewDirection),0.f) };
	const float phongExponent{ m_pGlossinessMap->Sample(v.uv).r };
	const float shininess{ 25.f };
	const float ks{ 1.f };
	ColorRGB specularColor{ m_pSpecularMap->Sample(v.uv) };
	const float phongSpecularReflection{ ks * powf(cosine,phongExponent * shininess) };
	const ColorRGB phong{ specularColor * phongSpecularReflection };


	switch (m_ShadingMode)
	{
	case ShadingMode::ObservedArea:
		return ColorRGB{ lambertCosine ,lambertCosine ,lambertCosine };
		break;
	case ShadingMode::Diffuse:
		return lambertDiffuse;
		break;
	case ShadingMode::Specular:
		return phong;
		break;
	case ShadingMode::Combined:
		return (lambertDiffuse + phong) * lambertCosine;
		break;
	default:
		return (lambertDiffuse + phong) * lambertCosine;
		break;
	}
}

void Mesh::FindBoundingBoxCorners(Vector2& topLeft, Vector2& botRight, const std::vector<Vertex_Out>& triangle) const
{
	topLeft.x = std::min(std::min(triangle[0].position.x, triangle[1].position.x), triangle[2].position.x);
	topLeft.x = Clamp(topLeft.x, 0.f, float(m_WindowWidth - 1));
	topLeft.y = std::min(std::min(triangle[0].position.y, triangle[1].position.y), triangle[2].position.y);
	topLeft.y = Clamp(topLeft.y, 0.f, float(m_WindowHeight - 1));

	botRight.x = std::max(std::max(triangle[0].position.x, triangle[1].position.x), triangle[2].position.x);
	botRight.x = Clamp(botRight.x, 0.f, float(m_WindowWidth - 1));
	botRight.x = std::ceil(botRight.x);

	botRight.y = std::max(std::max(triangle[0].position.y, triangle[1].position.y), triangle[2].position.y);
	botRight.y = Clamp(botRight.y, 0.f, float(m_WindowHeight - 1));
	botRight.y = std::ceil(botRight.y);
}

float Mesh::CalculateWeights(const Vector2& vertex1, const Vector2& vertex2, const Vector2& pixel, float area) const
{
	return Vector2::Cross(vertex2 - vertex1, pixel - vertex1) / area;
}

bool Mesh::IsPointInTriangle(const Vector3& weights) const
{
	return ((weights.x > 0) && (weights.y > 0) && (weights.z > 0));
}


ID3D11InputLayout* Mesh::GetInputLayoutPtr() const
{
	return m_pInputLayout;
}

Effect* Mesh::GetEffectPtr() const
{
	return m_pEffect;
}

void Mesh::ToggleRotation()
{
	m_IsRotating = !m_IsRotating;
}

bool Mesh::GetIsRotating() const
{
	return m_IsRotating;
}

Vector3 Mesh::GetPosition() const
{
	return m_Position;
}

ShadingMode Mesh::GetShadingMode() const
{
	return m_ShadingMode;
}

bool Mesh::GetIsNormalMapEnabled() const
{
	return m_IsNormalMapEnabled;
}

void Mesh::ToggleVisibility()
{
	m_IsVisible = !m_IsVisible;
}

void Mesh::ToggleShadingMode()
{
	m_ShadingMode = static_cast<ShadingMode>((static_cast<int>(m_ShadingMode) + 1) % m_NROFSHADINGMODES);
}

void Mesh::ToggleNormalMap()
{
	m_IsNormalMapEnabled = !m_IsNormalMapEnabled;
}

float Mesh::GetYaw() const
{
	return m_VehicleYaw;
}

void Mesh::ParseFireObj(const std::string& filename)
{
	std::vector<Vertex_Vehicle> parserVertices{};

	ParseObj(filename, parserVertices, m_Indices);

	for (const Vertex_Vehicle& vert : parserVertices)
	{
		Vertex_Fire v{};
		v.position = vert.position;
		v.uv = vert.uv;

		m_FireVertices.push_back(v);
	}
}

bool Mesh::ParseObj(const std::string& filename, std::vector<Vertex_Vehicle>& vertices, std::vector<uint32_t>& indices, bool flipAxisAndWinding)
{
	std::ifstream file(filename);
	if (!file)
		return false;

	std::vector<Vector3> positions{};
	std::vector<Vector3> normals{};
	std::vector<Vector2> UVs{};

	vertices.clear();
	indices.clear();

	std::string sCommand;
	// start a while iteration ending when the end of file is reached (ios::eof)
	while (!file.eof())
	{
		//read the first word of the string, use the >> operator (istream::operator>>) 
		file >> sCommand;
		//use conditional statements to process the different commands	
		if (sCommand == "#")
		{
			// Ignore Comment
		}
		else if (sCommand == "v")
		{
			//Vertex
			float x, y, z;
			file >> x >> y >> z;

			positions.emplace_back(x, y, z);
		}
		else if (sCommand == "vt")
		{
			// Vertex TexCoord
			float u, v;
			file >> u >> v;
			UVs.emplace_back(u, 1 - v);
		}
		else if (sCommand == "vn")
		{
			// Vertex Normal
			float x, y, z;
			file >> x >> y >> z;

			normals.emplace_back(x, y, z);
		}
		else if (sCommand == "f")
		{
			//if a face is read:
			//construct the 3 vertices, add them to the vertex array
			//add three indices to the index array
			//add the material index as attibute to the attribute array
			//
			// Faces or triangles
			Vertex_Vehicle vertex{};
			size_t iPosition, iTexCoord, iNormal;

			uint32_t tempIndices[3];
			for (size_t iFace = 0; iFace < 3; iFace++)
			{
				// OBJ format uses 1-based arrays
				file >> iPosition;
				vertex.position = positions[iPosition - 1];

				if ('/' == file.peek())//is next in buffer ==  '/' ?
				{
					file.ignore();//read and ignore one element ('/')

					if ('/' != file.peek())
					{
						// Optional texture coordinate
						file >> iTexCoord;
						vertex.uv = UVs[iTexCoord - 1];
					}

					if ('/' == file.peek())
					{
						file.ignore();

						// Optional vertex normal
						file >> iNormal;
						vertex.normal = normals[iNormal - 1];
					}
				}

				vertices.push_back(vertex);
				tempIndices[iFace] = uint32_t(vertices.size()) - 1;
				//indices.push_back(uint32_t(vertices.size()) - 1);
			}

			indices.push_back(tempIndices[0]);
			if (flipAxisAndWinding)
			{
				indices.push_back(tempIndices[2]);
				indices.push_back(tempIndices[1]);
			}
			else
			{
				indices.push_back(tempIndices[1]);
				indices.push_back(tempIndices[2]);
			}
		}
		//read till end of line and ignore all remaining chars
		file.ignore(1000, '\n');
	}

	//Cheap Tangent Calculations
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		uint32_t index0 = indices[i];
		uint32_t index1 = indices[size_t(i) + 1];
		uint32_t index2 = indices[size_t(i) + 2];

		const Vector3& p0 = vertices[index0].position;
		const Vector3& p1 = vertices[index1].position;
		const Vector3& p2 = vertices[index2].position;
		const Vector2& uv0 = vertices[index0].uv;
		const Vector2& uv1 = vertices[index1].uv;
		const Vector2& uv2 = vertices[index2].uv;

		const Vector3 edge0 = p1 - p0;
		const Vector3 edge1 = p2 - p0;
		const Vector2 diffX = Vector2(uv1.x - uv0.x, uv2.x - uv0.x);
		const Vector2 diffY = Vector2(uv1.y - uv0.y, uv2.y - uv0.y);
		float r = 1.f / Vector2::Cross(diffX, diffY);

		Vector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		vertices[index0].tangent += tangent;
		vertices[index1].tangent += tangent;
		vertices[index2].tangent += tangent;
	}

	//Create the Tangents (reject)
	for (auto& v : vertices)
	{
		v.tangent = Vector3::Reject(v.tangent, v.normal).Normalized();

		if (flipAxisAndWinding)
		{
			v.position.z *= -1.f;
			v.normal.z *= -1.f;
			v.tangent.z *= -1.f;
		}

	}

	return true;

}


