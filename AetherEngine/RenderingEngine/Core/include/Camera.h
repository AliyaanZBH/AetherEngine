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

		void SetPosition(const glm::vec3& pos);

		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetViewProj() const { return m_Projection * m_View; }

		const glm::vec3 GetForward()	const	{ return m_Rotation * m_Forward; }
		const glm::vec3 GetRight()		const	{ return m_Rotation * m_Right; }
		const glm::vec3 GetUp()			const	{ return m_Rotation * m_Up; }

	private:
		void CalculateView();
		void CalculateProjection();
		
		// View data
		glm::mat4 m_View		{ 1.f };
		glm::vec3 m_Position	{ 0.f };
		glm::vec3 m_Forward		{ 0.f, 0.f, 1.f };	// DX view space looks down -Z
		glm::vec3 m_Right		{ 1.f, 0.f, 0.f };
		glm::vec3 m_Up			{ 0.f, 1.f, 0.f };
		
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