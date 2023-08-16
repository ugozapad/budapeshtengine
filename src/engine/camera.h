#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

class ENGINE_API Camera {
public:
	Camera();
	~Camera();

	void UpdateLook(int width, int height);

	glm::mat4 GetViewMatrix();

	void LookAt(glm::vec3& dir);

	glm::vec3 GetPosition() { return m_pos; }
	void SetPosition(glm::vec3& pos) { m_pos = pos; }

	glm::vec3 GetDirection() { return m_dir; }
	void SetDirection(glm::vec3& dir) { m_dir = dir; }
public:
	glm::vec3 m_pos;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_dir;

	float m_yaw;
	float m_pitch;
};

class ENGINE_API CameraManager
{
public:
	void SetActiveCamera(Camera* p_camera);
	Camera* GetActiveCamera();

private:
	Camera* mp_active_camera;
};

extern ENGINE_API CameraManager g_CameraManager;

#endif // !CAMERA_H
