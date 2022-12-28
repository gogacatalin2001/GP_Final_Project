#include "Camera.hpp"
#include <GL/glew.h>

namespace gps {

	// Camera constructors
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraUpDirection = cameraUp;
		this->cameraFrontDirection = glm::normalize(cameraTarget- cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
	}

	// return the view matrix, using the glm::lookAt() function
	glm::mat4 Camera::getViewMatrix() {
		// TODO: iplement the veiw matrix

		return glm::lookAt(cameraPosition, cameraFrontDirection, cameraUpDirection);
	}

	// update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		switch (direction)
		{
		case gps::MOVE_FORWARD:
			cameraPosition += cameraFrontDirection * speed;
			break;
		case gps::MOVE_BACKWARD:
			cameraPosition -= cameraFrontDirection * speed;
			break;
		case gps::MOVE_RIGHT:
			cameraPosition += cameraRightDirection * speed;
			break;
		case gps::MOVE_LEFT:
			cameraPosition -= cameraRightDirection * speed;
			break;
		default:
			break;
		}
	}

	//update the camera internal parameters following a camera rotate event
	//yaw - camera rotation around the y axis
	//pitch - camera rotation around the x axis
	void Camera::rotate(float pitch, float yaw) {
		// TODO: implement camera ratoation
		glm::vec3 front;
		
	}
}