#pragma once
//===============================================================================
// desc: Camera! Nuff said
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
//===============================================================================

namespace Aether
{
	enum class eProjectionType {kPerspective, kOrthographic};

	class AETHER_API Camera
	{
	public:
		Camera(const eProjectionType type, const float nearPlane = 0.1f, const float farPlane = 1000.f, const float fov = 90.f);

		void Translate(const glm::vec3& delta);
		void RotateEuler(const glm::vec3& eulerDegrees);
		void Rotate(const glm::quat& delta);

		void SetPerspectiveCamera();
		void SetOrthographicCamera();
		void SetAspectRatio(const float aspect);

		void ResetRotation();
		void ResetPosition();

		void SetPosition(const glm::vec3& pos);
		void SetRotation(const glm::quat& rot);

		const eProjectionType GetProjectionType()	const { return m_ProjectionType; }

		const glm::mat4& GetView()			const { return m_View; }
		const glm::mat4& GetProjection()	const { return m_Projection; }
		const glm::mat4 GetViewProj()		const { return m_Projection * m_View; }

		const glm::vec3 GetWorldForward()	const { return m_WorldForward; }
		const glm::vec3 GetWorldRight()		const { return m_WorldRight; }
		const glm::vec3 GetWorldUp()		const { return m_WorldUp; }

		const glm::vec3 GetForward()	const { return m_Rotation * m_WorldForward; }
		const glm::vec3 GetRight()		const { return m_Rotation * m_WorldRight; }
		const glm::vec3 GetUp()			const { return m_Rotation * m_WorldUp; }

	private:
		void CalculateView();
		void CalculateProjection();
		
		// World data
		static constexpr glm::vec3 m_WorldForward{ 0.f, 0.f, 1.f };
		static constexpr glm::vec3 m_WorldRight{ 1.f, 0.f, 0.f };
		static constexpr glm::vec3 m_WorldUp{ 0.f, 1.f, 0.f };

		// View data
		glm::mat4 m_View		{ 1.f };
		glm::vec3 m_Position	{ 0.f };
		
		// Projection data
		glm::mat4 m_Projection	{ 1.f };
		float m_Fov = 90.f;						
		float m_Aspect = 1.f;						
		float m_NearPlane = 0.01f;					
		float m_FarPlane = 1000.0f;				

		// Orthographic data
		float m_OrthoLeft =		-1.0f;
		float m_OrthoRight =	 1.0f;
		float m_OrthoBottom =	-1.0f;
		float m_OrthoTop =		 1.0f;

		// Rotation data
		glm::quat m_Rotation = glm::identity<glm::quat>();

		eProjectionType m_ProjectionType = eProjectionType::kPerspective;
	};
}
