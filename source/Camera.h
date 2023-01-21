#pragma once
#include "DataTypes.h"

using namespace dae;

class Camera final
{
public:
	// -----------------------------------------------
	// Constructor and destructor
	// -----------------------------------------------
	Camera(float _fovAngle, const Vector3& _m_Origin, float _aspectRatio);
	~Camera();

	// -----------------------------------------------
	// Copy/move constructors and assignment operators
	// -----------------------------------------------
	Camera(const Camera& other)					= default;
	Camera(Camera&& other) noexcept				= default;
	Camera& operator=(const Camera& other)		= default;
	Camera& operator=(Camera&& other) noexcept	= default;

	//------------------------------------------------
	// Public member functions						
	//------------------------------------------------
	void	CalculateViewMatrix();
	void	CalculateProjectionMatrix();
	void	Update(const Timer* pTimer);

	Matrix	GetViewMatrix() const;
	Matrix	GetProjectionMatrix() const;

	Vector3 GetOrigin() const;
	Vector3 GetForwardVector() const;
private:

	//------------------------------------------------
	// Member Variables						
	//------------------------------------------------
	float		m_FOV				{};
	float		m_TotalYaw			{};
	float		m_TotalPitch		{};
	float		m_SpeedMultiplier	{ 1.f };

	const float m_FovAngle			{};
	const float m_AspectRatio		{};
	const float m_NearPlane			{ 0.1f };
	const float m_FarPlane			{ 100.f };
	const float m_MovementSpeed		{ 20.f };
	const float m_RotationSpeed		{ 1000.f };

	Vector3		m_Origin			{};
	Vector3		m_Up				{ Vector3::UnitY };
	Vector3		m_Right				{ Vector3::UnitX };
	Vector3		m_Forward			{ Vector3::UnitZ };

	Matrix		m_ViewMatrix		{};
	Matrix		m_InvViewMatrix		{};
	Matrix		m_ProjectionMatrix	{};
};

