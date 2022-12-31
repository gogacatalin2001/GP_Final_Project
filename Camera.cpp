#include "Camera.hpp"

namespace gps {

	//Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraUpDirection = cameraUp;
		this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
	}

	//return the view matrix, using the glm::lookAt() function
	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
	}

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return cameraPosition;
	}

	//update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		switch (direction) {
			case MOVE_FORWARD: {
				this->cameraPosition += speed * cameraFrontDirection;
				this->cameraTarget += speed * cameraFrontDirection;
				break;
			}
			case MOVE_BACKWARD: {
				this->cameraPosition -= speed * cameraFrontDirection;
				this->cameraTarget -= speed * cameraFrontDirection;
				break;
			}
			case MOVE_LEFT: {
				this->cameraPosition -= speed * cameraRightDirection;
				this->cameraTarget -= speed * cameraRightDirection;
				break;
			}
			case MOVE_RIGHT: {
				this->cameraPosition += speed * cameraRightDirection;
				this->cameraTarget += speed * cameraRightDirection;
				break;
			}
			case MOVE_UP: {
				this->cameraPosition += glm::vec3(0.0f, speed, 0.0f);
				this->cameraTarget += glm::vec3(0.0f, speed, 0.0f);
				break;
			}
			case MOVE_DOWN: {
				this->cameraPosition += glm::vec3(0.0f, -speed, 0.0f);
				this->cameraTarget += glm::vec3(0.0f, -speed, 0.0f);
				break;
			}
			/*case MOVE_ANIM: {
				this->cameraPosition = glm::vec3(sin(speed), 0.5f, cos(speed));
				this->cameraPosition *= 6;
				this->cameraTarget = glm::vec3(0.0f);
				this->cameraFrontDirection = cameraTarget - cameraPosition;
				this->cameraRightDirection = glm::cross(this->cameraFrontDirection, this->cameraUpDirection);
				break;
			}*/
		}
	}

	//update the camera internal parameters following a camera rotate event
	//yaw - camera rotation around the y axis STANGA DREAPTA
	//pitch - camera rotation around the x axis SUS JOS
	void Camera::rotate(float pitch, float yaw) {
		//-89 < pitch < 89
		// x = cos(pitch) * cos(yaw)
		// y = sin(pitch)
		// z = cos(pitch) * sin(yaw)
		// cameraFrontDir = vec3(x,y,z)
		glm::vec3 newCamDir = glm::vec3();
		newCamDir.x = glm::cos(pitch) * glm::cos(yaw);
		newCamDir.y = glm::sin(pitch);
		newCamDir.z = glm::cos(pitch) * glm::sin(yaw);
		this->cameraFrontDirection = glm::normalize(newCamDir);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
		//TODO
	}
}