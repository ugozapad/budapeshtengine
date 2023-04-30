#include "SDL.h"

#include "engine/camera.h"

Camera g_camera;

Camera::Camera()
{
	m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
	m_front = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_yaw = -90.0f;
	m_pitch = 0.0f;
}

Camera::~Camera()
{

}

void Camera::updateLook(int width, int height)
{
	SDL_SetRelativeMouseMode(SDL_TRUE);

	int mouseX = 0, mouseY = 0;
	SDL_GetMouseState(&mouseX, &mouseY);

	float x = (float)mouseX;
	float y = (float)mouseY;

	static float lastX = (float)width / 2.0;
	static float lastY = (float)height / 2.0;

	float sensitivity = 0.2f;

	float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top
	lastX = x;
	lastY = y;

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	m_yaw += xoffset;
	m_pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;

	m_dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_dir.y = sin(glm::radians(m_pitch));
	m_dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_front = glm::normalize(m_dir);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(m_pos, m_pos + m_front, m_up);
}

void Camera::lookAt(glm::vec3& dir)
{

}

