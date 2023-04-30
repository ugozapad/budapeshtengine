#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

class Camera {
public:
	Camera();
	~Camera();

	void updateLook(int width, int height);

	glm::mat4 getViewMatrix();

	void lookAt(glm::vec3& dir);

	glm::vec3 getPosition() { return m_pos; }
	void setPosition(glm::vec3& pos) { m_pos = pos; }

	glm::vec3 getDirection() { return m_dir; }
	void setDirection(glm::vec3& dir) { m_dir = dir; }
public:
	glm::vec3 m_pos;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_dir;

	float m_yaw;
	float m_pitch;
};

extern Camera g_camera;

#endif // !CAMERA_H
