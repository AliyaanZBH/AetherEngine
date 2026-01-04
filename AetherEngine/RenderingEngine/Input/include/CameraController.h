#pragma once
//===============================================================================
// desc: Simple camera controller class
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "Core.h"
#include "glm/vec2.hpp"
//===============================================================================

namespace Aether
{
	class Camera;
	class Event;

	class AETHER_API CameraController
	{
	public:
		CameraController(Camera& camera)
			: m_Camera(camera) {}

		void Update(float deltaTime);
		void OnEvent(Event& e);
	private:

		void HandleKeyboardMovement(float deltaTime);
		void HandleKeyboardRotation(float deltaTime);
		void HandleMouseRotation(float deltaTime);
		void HandleReset();


		Camera& m_Camera;
		
		glm::vec2 m_LastMousePos = { 0.f, 0.f };
		float m_MoveSpeed = 2.f;
		float m_MouseLookSens = 0.001f;
		float m_KeyLookSens = 0.5f;
		bool m_bRotatingWithMouse = false;
	};
}