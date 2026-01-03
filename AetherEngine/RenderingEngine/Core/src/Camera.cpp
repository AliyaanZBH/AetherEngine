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

	void Camera::Translate(const glm::vec3& delta)
	{
		m_Position += delta;
		CalculateView();
	}

	void Camera::Rotate(const glm::quat& delta)
	{
		// Use normalise to avoid quat drifting
		m_Rotation = glm::normalize(delta * m_Rotation);
		CalculateView();
	}

	void Camera::RotateEuler(const glm::vec3& eulerDegrees)
	{
		glm::vec3 radians = glm::radians(eulerDegrees);
		glm::quat delta = glm::quat(radians);
		Rotate(delta);
	}

	void Camera::SetPerspectiveCamera()
	{
		m_ProjectionType = eProjectionType::kPerspective;
		CalculateProjection();
	}

	void Camera::SetOrthographicCamera()
	{
		m_ProjectionType = eProjectionType::kOrthographic;
		CalculateProjection();
	}

	void Camera::SetAspectRatio(const float aspect)
	{
		m_Aspect = aspect;
		CalculateProjection();
	}

	void Camera::SetPosition(const glm::vec3& pos)
	{
		m_Position = pos;
		CalculateView();
	}

	void Camera::CalculateView()
	{
		// Black magic to get a rotation matrix out of a quaternion. Conjugate here to account for view matrix being the inverse of the cameras world rotation
		glm::mat4 rot = glm::mat4_cast(glm::conjugate(m_Rotation));
		glm::mat4 trans = glm::translate(glm::mat4(1.f), -m_Position);

		// Flip Z in the name of sanity (and intuitive use for designers)
		glm::mat4 zFlip = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, -1.f));
		m_View = zFlip * rot * trans;
	}

	void Camera::CalculateProjection()
	{
		if (m_ProjectionType == eProjectionType::kPerspective)
			m_Projection = glm::perspective(glm::radians(m_Fov), m_Aspect, m_NearPlane, m_FarPlane);
		else
			m_Projection = glm::ortho(m_OrthoLeft, m_OrthoRight, m_OrthoBottom, m_OrthoTop, m_NearPlane, m_FarPlane);
	}
}