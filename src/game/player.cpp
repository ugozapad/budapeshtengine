#include "pch.h"
#include "player.h"

#include "engine/input_system.h"
#include "engine/camera.h"

Player::Player() {
}

Player::~Player() {
}

void Player::update(float dt)
{
	const float fWalkSpeed = 9.0f;

	// calculate player movement

	if (g_input_system->isKeyPressed(SDL_SCANCODE_W))
		m_position += (fWalkSpeed * dt) * g_camera.m_front;
	if (g_input_system->isKeyPressed(SDL_SCANCODE_S))
		m_position -= (fWalkSpeed * dt) * g_camera.m_front;
	if (g_input_system->isKeyPressed(SDL_SCANCODE_A))
		m_position -= glm::normalize(glm::cross(g_camera.m_front, g_camera.m_up)) * (fWalkSpeed * dt);
	if (g_input_system->isKeyPressed(SDL_SCANCODE_D))
		m_position += glm::normalize(glm::cross(g_camera.m_front, g_camera.m_up)) * (fWalkSpeed * dt);

	glm::vec3 front;
	front.x = cos(glm::radians(g_camera.m_yaw)) * cos(glm::radians(-g_camera.m_pitch));
	front.y = sin(glm::radians(-g_camera.m_pitch));
	front.z = sin(glm::radians(g_camera.m_yaw)) * cos(glm::radians(-g_camera.m_pitch));
	front = glm::normalize(front);
	g_camera.setDirection(front);

	// set position back to camera for calculation view matrix
	g_camera.setPosition(m_position);
}
