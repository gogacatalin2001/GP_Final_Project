#include "Camera.hpp"

namespace gps {

	// Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
		this->cameraRightDirection = glm::normalize(glm::cross(cameraUp, this->cameraFrontDirection));
		this->cameraUpDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraRightDirection));
	}

	// return the view matrix, using the glm::lookAt() function
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

	// update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		switch (direction) {
			case MOVE_FORWARD: {
				this->cameraPosition += speed * this->cameraFrontDirection;
				this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
				//this->cameraTarget += speed * cameraFrontDirection;
				break;
			}
			case MOVE_BACKWARD: {
				this->cameraPosition -= speed * this->cameraFrontDirection;
				this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));

				//this->cameraTarget -= speed * cameraFrontDirection;
				break;
			}
			case MOVE_LEFT: {
				this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
				this->cameraPosition -= speed * cameraRightDirection;
				//this->cameraTarget -= speed * cameraRightDirection;
				break;
			}
			case MOVE_RIGHT: {
				this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
				this->cameraPosition += speed * cameraRightDirection;
				//this->cameraTarget += speed * cameraRightDirection;
				break;
			}
			case MOVE_UP: {
				this->cameraPosition += this->cameraUpDirection * speed;
				//this->cameraTarget += glm::vec3(0.0f, speed, 0.0f);
				break;
			}
			case MOVE_DOWN: {
				this->cameraPosition -= this->cameraUpDirection * speed;
				//this->cameraTarget += glm::vec3(0.0f, -speed, 0.0f);
				break;
			}
			/*case MOVE_ANIM: {
				float camX = sin(glfwGetTime());
				this->cameraPosition = glm::vec3(sin(speed), 0.5f, cos(speed));
				this->cameraPosition *= 6;
				this->cameraTarget = glm::vec3(0.0f);
				this->cameraFrontDirection = cameraTarget - cameraPosition;
				this->cameraRightDirection = glm::cross(this->cameraFrontDirection, this->cameraUpDirection);
				break;
			}*/
		}
	}

	// update the camera internal parameters following a camera rotate event
	// yaw - camera rotation around the y axis
	// pitch - camera rotation around the x axis
	void Camera::rotate(float pitch, float yaw) {
		glm::vec3 viewDirection;
		viewDirection.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		viewDirection.y = sin(glm::radians(pitch));
		viewDirection.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		this->cameraFrontDirection = glm::normalize(viewDirection);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
	}
}