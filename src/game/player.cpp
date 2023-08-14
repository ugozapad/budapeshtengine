#include "pch.h"
#include "player.h"

#include "engine/engine.h"
#include "engine/input_system.h"

Player::Player() {
}

Player::~Player() {
}

void Player::update(float dt)
{
	viewport_t viewport = g_engine->getViewport();
	m_camera.updateLook(viewport.width, viewport.height);

	const float fWalkSpeed = 9.0f;

	// calculate player movement

	if (g_input_system->isKeyPressed(SDL_SCANCODE_W))
		m_position += (fWalkSpeed * dt) * m_camera.m_front;
	if (g_input_system->isKeyPressed(SDL_SCANCODE_S))
		m_position -= (fWalkSpeed * dt) * m_camera.m_front;
	if (g_input_system->isKeyPressed(SDL_SCANCODE_A))
		m_position -= glm::normalize(glm::cross(m_camera.m_front, m_camera.m_up)) * (fWalkSpeed * dt);
	if (g_input_system->isKeyPressed(SDL_SCANCODE_D))
		m_position += glm::normalize(glm::cross(m_camera.m_front, m_camera.m_up)) * (fWalkSpeed * dt);

	glm::vec3 front = glm::vec3(1.0f);
	front.x = cos(glm::radians(m_camera.m_yaw)) * cos(glm::radians(-m_camera.m_pitch));
	front.y = sin(glm::radians(-m_camera.m_pitch));
	front.z = sin(glm::radians(m_camera.m_yaw)) * cos(glm::radians(-m_camera.m_pitch));
	front = glm::normalize(front);
	m_camera.setDirection(front);

	// set position back to camera for calculation view matrix
	m_camera.setPosition(m_position);
}

void Player::activateCamera()
{
	g_CameraManager.setActiveCamera(&m_camera);
}
