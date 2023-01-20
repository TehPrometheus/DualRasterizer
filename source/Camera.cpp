#include "pch.h"
#include "Camera.h"




Camera::Camera(float _fovAngle, const Vector3& _origin, float _aspectRatio)
:
	m_Origin{ _origin },
	m_FovAngle{ _fovAngle },
	m_AspectRatio{_aspectRatio}
{
	m_FOV = tanf((m_FovAngle * TO_RADIANS) / 2.f);
	CalculateProjectionMatrix();
}

Camera::~Camera()
{
}

void Camera::Update(const Timer* pTimer)
{
	const float deltaTime = pTimer->GetElapsed();

	//Camera Update Logic
	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	m_SpeedMultiplier = 1.f;
	if (pKeyboardState[SDL_SCANCODE_LSHIFT])
	{
		m_SpeedMultiplier = 3.f;
	}
	if (pKeyboardState[SDL_SCANCODE_W])
	{
		m_Origin += m_SpeedMultiplier * m_MovementSpeed * deltaTime * m_Forward;
	}
	if (pKeyboardState[SDL_SCANCODE_A])
	{
		m_Origin += m_SpeedMultiplier * m_MovementSpeed * deltaTime * -m_Right;
	}
	if (pKeyboardState[SDL_SCANCODE_S])
	{
		m_Origin += m_SpeedMultiplier * m_MovementSpeed * deltaTime * -m_Forward;
	}
	if (pKeyboardState[SDL_SCANCODE_D])
	{
		m_Origin += m_SpeedMultiplier * m_MovementSpeed * deltaTime * m_Right;
	}

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	if (mouseState & SDL_BUTTON(1))
	{

		if (mouseState & SDL_BUTTON(3))
		{
			if (mouseY > 0)
			{
				m_Origin += m_MovementSpeed * deltaTime * -m_Up;
			}
			else if (mouseY < 0)
			{
				m_Origin += m_MovementSpeed * deltaTime * m_Up;
			}
		}
		else
		{
			if (mouseY > 0)
			{
				m_Origin += m_MovementSpeed * deltaTime * -m_Forward;
			}
			else if (mouseY < 0)
			{
				m_Origin += m_MovementSpeed * deltaTime * m_Forward;
			}
			if (mouseX > 0)
			{
				m_TotalYaw += m_RotationSpeed * deltaTime;
			}
			else if (mouseX < 0)
			{
				m_TotalYaw -= m_RotationSpeed * deltaTime;
			}
		}


	}
	else if (mouseState & SDL_BUTTON(3))
	{

		if (mouseX < 0)
		{
			m_TotalYaw -= m_RotationSpeed * deltaTime;
		}
		else if (mouseX > 0)
		{
			m_TotalYaw += m_RotationSpeed * deltaTime;
		}
		if (mouseY > 0)
		{
			m_TotalPitch -= m_RotationSpeed * deltaTime;
		}
		else if (mouseY < 0)
		{
			m_TotalPitch += m_RotationSpeed * deltaTime;
		}
	}
	//Update Matrix
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}


void Camera::CalculateViewMatrix()
{
	Matrix rotationMatrix = Matrix::CreateRotationX(m_TotalPitch * TO_RADIANS) * Matrix::CreateRotationY(m_TotalYaw * TO_RADIANS);

	m_Forward = rotationMatrix.GetAxisZ();
	m_Up = rotationMatrix.GetAxisY();
	m_Right = rotationMatrix.GetAxisX();

	Matrix ONB
	{
		Vector4{ m_Right, 0.f},
		Vector4{ m_Up, 0.f},
		Vector4{ m_Forward, 0.f},
		Vector4{ m_Origin ,1.f}
	};

	m_ViewMatrix = ONB;
	m_InvViewMatrix = Matrix::Inverse(m_ViewMatrix);

}

void Camera::CalculateProjectionMatrix()
{
	m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FOV, m_AspectRatio, m_NearPlane, m_FarPlane);
}

Matrix Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

Matrix Camera::GetProjectionMatrix() const
{
	return m_ProjectionMatrix;
}

Vector3 Camera::GetOrigin() const
{
	return m_Origin;
}

