//===============================================================================
// desc: Simple camera controller class
// auth: Aliyaan Zulfiqar
//===============================================================================
#include "CameraController.h"
#include "Camera.h"
#include "Input.h"
//===============================================================================

namespace Aether
{
	void CameraController::Update(float deltaTime)
	{
		HandleKeyboardMovement(deltaTime);
		HandleKeyboardRotation(deltaTime);
		HandleMouseRotation(deltaTime);

		// Press R to reset
		HandleReset();
	}

	void CameraController::HandleReset()
	{
		//if (Input::IsKeyReleased(KeyCode::kR))
		if (Input::IsKeyPressed(KeyCode::kR))
		{
			m_Camera.ResetRotation();
			m_Camera.ResetPosition();
		}
	}
		
	void CameraController::HandleKeyboardMovement(float deltaTime)
	{
		glm::vec3 move{};

		if (Input::IsKeyPressed(KeyCode::kA))
			move -= m_Camera.GetRight();
		if (Input::IsKeyPressed(KeyCode::kD))
			move += m_Camera.GetRight();

		// Depending on our projection type, we want this to up+down to mean different things
		if (m_Camera.GetProjectionType() == eProjectionType::kPerspective)
		{
			// Move camera in and out of Z axis / forward direction
			if (Input::IsKeyPressed(KeyCode::kW))
				move += m_Camera.GetForward();
			if (Input::IsKeyPressed(KeyCode::kS))
				move -= m_Camera.GetForward();

			// Add in another method to move camera up and down

			if (Input::IsKeyPressed(KeyCode::kLeftShift))
				move += m_Camera.GetUp();
			if (Input::IsKeyPressed(KeyCode::kLeftControl))
				move -= m_Camera.GetUp();
		}
		else
		{
			// Only move camera up and down for orthographic
			if (Input::IsKeyPressed(KeyCode::kW))
				move += m_Camera.GetUp();
			if (Input::IsKeyPressed(KeyCode::kS))
				move -= m_Camera.GetUp();
		}

		if (glm::length(move) > 0)
			m_Camera.Translate(glm::normalize(move) * m_MoveSpeed * deltaTime);
	}

	void CameraController::HandleKeyboardRotation(float deltaTime)
	{
		float yaw = 0.f;
		float pitch = 0.f;

		if (Input::IsKeyPressed(KeyCode::kLeft))
			yaw -= m_KeyLookSens * deltaTime;
		if (Input::IsKeyPressed(KeyCode::kRight))
			yaw += m_KeyLookSens * deltaTime;
		if (Input::IsKeyPressed(KeyCode::kUp))
			pitch -= m_KeyLookSens * deltaTime;
		if (Input::IsKeyPressed(KeyCode::kDown))
			pitch += m_KeyLookSens * deltaTime;

		if (yaw == 0 && pitch == 0)
		{
			return;
		}

		// Construct magic quats - use World up to avoid drift
		glm::quat qYaw = glm::angleAxis(yaw, m_Camera.GetWorldUp());
		glm::quat qPitch = glm::angleAxis(pitch, m_Camera.GetRight());

		m_Camera.Rotate(qYaw * qPitch);
	}

	void CameraController::HandleMouseRotation(float deltaTime)
	{
		if (!Input::IsMouseButtonPressed(MouseCode::kLMB))
		{
			m_bRotatingWithMouse = false;
			return;
		}

		glm::vec2 mousePos = Input::GetMousePosition();

		if (!m_bRotatingWithMouse)
		{
			// First frame of drag, establish reference point and return early this one time
			m_LastMousePos = mousePos;
			m_bRotatingWithMouse = true;
			return;
		}

		glm::vec2 delta = mousePos - m_LastMousePos;
		m_LastMousePos = mousePos;

		float yaw = delta.x * m_MouseLookSens;
		float pitch = delta.y * m_MouseLookSens;

		// Construct magic quats
		glm::quat qYaw = glm::angleAxis(yaw, m_Camera.GetWorldUp());
		glm::quat qPitch = glm::angleAxis(pitch, m_Camera.GetRight());

		m_Camera.Rotate(qYaw * qPitch);
	}
}
