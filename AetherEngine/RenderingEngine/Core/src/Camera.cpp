//===============================================================================
// desc: Camera! Nuff said
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Camera.h"
//===============================================================================

namespace Aether
{
	Camera::Camera(eProjectionType type, float nearPlane, float farPlane, float fov)
		: m_ProjectionType(type), m_NearPlane(nearPlane), m_FarPlane(farPlane), m_Fov(fov)
	{
		// Calculate starting values for view, will instantiate at 0,0,0
		CalculateView();
		CalculateProjection();
	}

	void Camera::Translate()
	{
	}

	void Camera::Rotate()
	{
	}

	void Camera::SetPerspectiveCamera()
	{
		m_ProjectionType == eProjectionType::kPerspective;
		CalculateProjection();
	}

	void Camera::SetOrthographicCamera()
	{
		m_ProjectionType == eProjectionType::kOrthographic;
		CalculateProjection();
	}

	void Camera::SetPosition(const glm::vec3& pos)
	{
		m_Position = pos;
		CalculateView();
	}

	void Camera::CalculateView()
	{
		//m_View = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);

		// Black magic to get a rotation matrix out of a quaternion
		glm::mat4 rot = glm::mat4_cast(glm::conjugate(m_Rotation));
		glm::mat4 trans = glm::translate(glm::mat4(1.f), -m_Position);
		m_View = rot * trans;
	}

	void Camera::CalculateProjection()
	{
		if (m_ProjectionType == eProjectionType::kPerspective)
		{
			m_Projection = glm::perspective(glm::radians(m_Fov), m_Aspect, m_NearPlane, m_FarPlane);

		}
		else
		{
			m_Projection = glm::ortho(m_OrthoLeft, m_OrthoRight, m_OrthoBottom, m_OrthoTop, m_NearPlane, m_FarPlane);

		}
	}
}