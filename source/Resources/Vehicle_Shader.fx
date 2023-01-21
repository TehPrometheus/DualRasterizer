//------------------------------------------------
// Variables						
//------------------------------------------------
float gPi = 3.1415f;
float gShininess = 25.f;
float gLightIntensity = 7.f;

float3 gLightDirection = float3(0.577f, -0.577f, 0.577f);

float4x4 gViewInverseMatrix : ViewInverseMatrix;
float4x4 gWorldViewProj		: WorldViewProjection;
float4x4 gWorldMatrix		: WorldMatrix;

Texture2D gNormalMap	 : NormalMap;
Texture2D gDiffuseMap	 : DiffuseMap;
Texture2D gSpecularMap	 : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;



//------------------------------------------------
// I/O Structs						
//------------------------------------------------

struct VS_INPUT
{
	float3 Position		 : POSITION;
	float3 Normal		 : NORMAL;
	float3 Tangent		 : TANGENT;
	float2 TexCoord		 : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position		 : SV_POSITION;
	float3 Normal		 : NORMAL;
	float3 Tangent		 : TANGENT;
	float2 TexCoord		 : TEXCOORD;
	float4 WorldPosition : COLOR;
};

//------------------------------------------------
// Sample States						
//------------------------------------------------

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
	AddressV = Wrap;
};

//------------------------------------------------
// Blend State				
//------------------------------------------------

BlendState gBlendState{};

//------------------------------------------------
// DepthStencil State				
//------------------------------------------------

DepthStencilState gDepthStencilState{};

//----------------------------
// Rasterizer States
//----------------------------

RasterizerState gBackCullRasterizerState
{
	CullMode = back;
	FrontCounterClockwise = false;
};

RasterizerState gFrontCullRasterizerState
{
	CullMode = front;
	FrontCounterClockwise = false;
};

RasterizerState gNoCullRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = false;
};

//------------------------------------------------
// Vertex Shader					
//------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output	= (VS_OUTPUT)0;
	output.Position		= mul(float4(input.Position, 1.f),  gWorldViewProj);

	output.WorldPosition = mul(float4(input.Position, 1.f),gWorldMatrix );
	output.Normal		 = mul(normalize(input.Normal),(float3x3)gWorldMatrix );
	output.Tangent		 = mul(normalize(input.Tangent), (float3x3)gWorldMatrix );

	output.TexCoord = input.TexCoord;
	return output;
}

//------------------------------------------------
// Pixel Shaders						
//------------------------------------------------

float3 GetNormal(float2 texCoord, float3 normal, float3 tangent,  SamplerState samplerState)
{
	const float3 binormal = cross(normal,tangent);
	const float3x3 tangentSpaceMatrix =
	{	
		tangent, binormal, normal
	};
	float3 normalMapSample = gNormalMap.Sample(samplerState, texCoord).xyz;
	normalMapSample = 2.f * normalMapSample - float3(1, 1, 1);

	return normalize(mul(normalMapSample,tangentSpaceMatrix ));
}

float3 GetPhong(float2 texCoord, float3 normal, float3 worldPos,  SamplerState samplerState)
{
	const float3 reflect = gLightDirection - 2 * (dot(normal, gLightDirection)) * normal;
	const float3 viewDirection = normalize(worldPos - gViewInverseMatrix[3].xyz);
	const float  cosine = saturate(dot(reflect, -viewDirection));

	const float3 specularMapSample = gSpecularMap.Sample(samplerState, texCoord).xyz;
	const float  glossinessMapSample = gGlossinessMap.Sample(samplerState, texCoord).r * gShininess;

	return specularMapSample * pow(cosine, glossinessMapSample);
}

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
	const float3 lambertDiffuse = gDiffuseMap.Sample(samPoint, input.TexCoord).xyz * gLightIntensity / gPi;
	const float3 normal = GetNormal(input.TexCoord, input.Normal, input.Tangent, samPoint);
	const float3 phong = GetPhong(input.TexCoord, normal, input.WorldPosition.xyz,  samPoint);
	const float lambertCosine = saturate(dot(normal, -gLightDirection));

	float3 colorValue = (phong + lambertDiffuse) * lambertCosine;

	colorValue.r = saturate(colorValue.r);
	colorValue.g = saturate(colorValue.g);
	colorValue.b = saturate(colorValue.b);

	return float4(colorValue, 1);
}


float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
	const float3 lambertDiffuse = gDiffuseMap.Sample(samLinear, input.TexCoord).xyz * gLightIntensity / gPi;
	const float3 normal = GetNormal(input.TexCoord, input.Normal, input.Tangent, samLinear);
	const float3 phong = GetPhong(input.TexCoord, normal, input.WorldPosition.xyz,  samLinear);
	const float lambertCosine = saturate(dot(normal, -gLightDirection));

	float3 colorValue = (phong + lambertDiffuse) * lambertCosine;

	colorValue.r = saturate(colorValue.r);
	colorValue.g = saturate(colorValue.g);
	colorValue.b = saturate(colorValue.b);

	return float4(colorValue, 1);
}

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
	const float3 lambertDiffuse = gDiffuseMap.Sample(samAnisotropic, input.TexCoord).xyz * gLightIntensity / gPi;
	const float3 normal = GetNormal(input.TexCoord, input.Normal, input.Tangent,  samAnisotropic);
	const float3 phong = GetPhong(input.TexCoord, normal, input.WorldPosition.xyz,  samAnisotropic);
	const float lambertCosine = saturate(dot(normal, -gLightDirection));

	float3 colorValue = (phong + lambertDiffuse) * lambertCosine;

	colorValue.r = saturate(colorValue.r);
	colorValue.g = saturate(colorValue.g);
	colorValue.b = saturate(colorValue.b);

	return float4(colorValue, 1);
}


//------------------------------------------------
// Techniques					
//------------------------------------------------

technique11 PointBackCullTechnique
{
	pass P0
	{
		SetRasterizerState(gBackCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
	}
}

technique11 LinearBackCullTechnique
{
	pass P0
	{
		SetRasterizerState(gBackCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
	}
}

technique11 AnisotropicBackCullTechnique
{
	pass P0
	{
		SetRasterizerState(gBackCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
	}
}

technique11 PointFrontCullTechnique
{
	pass P0
	{
		SetRasterizerState(gFrontCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
	}
}

technique11 LinearFrontCullTechnique
{
	pass P0
	{
		SetRasterizerState(gFrontCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
	}
}

technique11 AnisotropicFrontCullTechnique
{
	pass P0
	{
		SetRasterizerState(gFrontCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
	}
}

technique11 PointNoCullTechnique
{
	pass P0
	{
		SetRasterizerState(gNoCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_POINT()));
	}
}

technique11 LinearNoCullTechnique
{
	pass P0
	{
		SetRasterizerState(gNoCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_LINEAR()));
	}
}

technique11 AnisotropicNoCullTechnique
{
	pass P0
	{
		SetRasterizerState(gNoCullRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_ANISOTROPIC()));
	}
}
