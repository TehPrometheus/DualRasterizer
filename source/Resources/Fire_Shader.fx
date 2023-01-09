//------------------------------------------------
// Variables						
//------------------------------------------------

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap   : DiffuseMap;

//------------------------------------------------
// I/O Structs						
//------------------------------------------------

struct VS_INPUT
{
	float3 Position		 : POSITION;
	float2 TexCoord		 : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 Position		 : SV_POSITION;
	float2 TexCoord		 : TEXCOORD;
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

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

//------------------------------------------------
// DepthStencil State				
//------------------------------------------------

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;
};

//----------------------------
// Rasterizer States
//----------------------------

RasterizerState gBackCullRasterizerState
{
	CullMode = front;
	FrontCounterClockwise = true;
};

RasterizerState gFrontCullRasterizerState
{
	CullMode = back;
	FrontCounterClockwise = true;
};

RasterizerState gNoCullRasterizerState
{
	CullMode = none;
	FrontCounterClockwise = true;
};

//------------------------------------------------
// Vertex Shader					
//------------------------------------------------

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f),  gWorldViewProj);
	output.TexCoord = input.TexCoord;

	return output;
}

//------------------------------------------------
// Pixel Shaders						
//------------------------------------------------

float4 PS_POINT(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samPoint, input.TexCoord);
}

float4 PS_LINEAR(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samLinear, input.TexCoord);
}

float4 PS_ANISOTROPIC(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(samAnisotropic, input.TexCoord);
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