#pragma once
#include "Math.h"
#include "vector"

namespace dae
{
	//struct Vertex_PosCol
	//{
	//	Vector3 position{};
	//	ColorRGB color{colors::White};
	//};

	struct Vertex_In
	{
		Vector3 position{};
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
	};

	struct Vertex_Out
	{
		Vertex_Out(	Vector4 posInput = Vector4{ 0,0,0,0 }, 
					Vector2 uvInput = Vector2{ 0,0 }, 
					ColorRGB colorInput = ColorRGB{ 1,1,1 },
					Vector3 normalInput = Vector3{1,0,0},
					Vector3 tangentInput = Vector3{ 1,0,0 })
		{
			position = posInput;
			color = colorInput;
			uv = uvInput;
			normal = normalInput;
			tangent = tangentInput;
		}
		Vector4 position{};
		ColorRGB color{ colors::White };
		Vector2 uv{};
		Vector3 normal{};
		Vector3 tangent{};
		Vector3 viewDirection{};
	};

	enum class PrimitiveTopology
	{
		TriangleList,
		TriangleStrip
	};

}