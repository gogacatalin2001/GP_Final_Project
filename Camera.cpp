#include "Camera.hpp"

namespace gps {

	// Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->initialPosition = cameraPosition;
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

	glm::vec3 Camera::getCameraFrontDirection()
	{
		return this->cameraFrontDirection;
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return cameraPosition;
	}

	void Camera::setCameraPosition(glm::vec3 position) {
		this->cameraPosition = position;
	}

	void Camera::goToInitialPosition() {
		this->cameraPosition = this->initialPosition;
		this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget);
		this->cameraRightDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->cameraFrontDirection));
		this->cameraUpDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraRightDirection));
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
	}

	void Camera::preview(float angle) {
		// set the camera position
		this->cameraPosition = glm::vec3(-9.0, 4.0, 4.0);

		// rotate with specific angle around Y axis
		glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

		// compute the new position of the camera 
		// previous position * rotation matrix
		this->cameraPosition = glm::vec4(r * glm::vec4(this->cameraPosition, 1));
		this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
		cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
}