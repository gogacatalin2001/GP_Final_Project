#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <string>

namespace gps {
    
    enum MOVE_DIRECTION {MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN};
    
    class Camera
    {
    public:
        // Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3);
        // return the view matrix, using the glm::lookAt() function
		glm::mat4 getViewMatrix();
        glm::vec3 getCameraTarget();
        glm::vec3 getCameraPosition();
        // update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        // update the camera internal parameters following a camera rotate event
        // yaw - camera rotation around the y axis
		// pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);
        

    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraTarget;
        glm::vec3 cameraUpDirection;
        glm::vec3 cameraRightDirection;
    };
    
}

#endif /* Camera_hpp */
