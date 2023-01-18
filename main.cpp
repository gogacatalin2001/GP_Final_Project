#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include <iostream>

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"


// ####################
//		WINDOW
// ####################

gps::Window myWindow;
int screenWidth = 800, screenHeight = 600;

// ####################
//		MATRICES
// ####################

glm::mat4 model;
GLint modelLoc;

glm::mat4 view;
GLint viewLoc;

glm::mat4 projection;
GLint projectionLoc;

// needed for light computation
glm::mat3 normalMatrix;
GLint normalMatrixLoc;

// needed for shadow computation
glm::mat3 lightDirMatrix;
GLuint lightDirMatrixLoc;

// ####################
//		LIGHT
// ####################

glm::vec3 lightDir;
GLint lightDirLoc;

glm::vec3 lightColor;
GLint lightColorLoc;

GLfloat lightAngle;

GLuint shadowMapFBO;
GLuint shadowMapTexture;
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;


// spotlight
int spotinit;
float spotlight1;
float spotlight2;

glm::vec3 spotLightDirection;
glm::vec3 spotLightPosition;


// lumina punctiforma
int pointinit = 0;
glm::vec3 lightPos1; // pe sperietoare
GLuint lightPos1Loc;
glm::vec3 lightPos2; // pe casa + fan
GLuint lightPos2Loc;



// ####################
//		MDELS
// ####################

GLfloat angle;
gps::Model3D house;
gps::Model3D fullScene;


// ####################
//		SHADERS
// ####################

gps::Shader mainShader;
gps::Shader lightShader;
gps::Shader depthMapShader;



// ####################
//		CAMERA
// ####################

glm::vec3 initialCameraPosition = glm::vec3(-0.1f, 0.21f, 3.55f);

gps::Camera myCamera(
	initialCameraPosition,
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

float cameraSpeed;
float currentFrame = 0.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float pitch = 0.0f, yaw = 0.0f;
const float nearPlane = 0.1f, farPlane = 1000.0f;
const float FOV = 45.0f;


// ####################
//		CONTROLS
// ####################


// keyboard
GLboolean pressedKeys[1024];

// mouse
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
const float mouseSensitivity = 0.5f;

// ####################
//		EFFECTS
// ####################

// fog
bool fogActive = false;
int fogInit = 0;
GLint fogInitLoc;
GLfloat fogDensity = 0.005f;

// for camera preview
bool cameraPreview = false;
float previewAngle = 0.0f;


// ####################
//		MAIN PROGRAM
// ####################


void preview() {
	if (cameraPreview) {
		previewAngle += 0.6f;
		myCamera.preview(previewAngle);
		std::cout << "Preview active" << std::endl;
	}
}

void moveCameraToInitialPos() {
	myCamera.setCameraPosition(initialCameraPosition);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	const GLfloat near_plane = 0.1f, far_plane = 6.0f;
	glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

	glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
	glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraPosition() + myCamera.getCameraFrontDirection(), glm::vec3(0.0f, 1.0f, 0.0f));

	return lightProjection * lightView;
}

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_STACK_OVERFLOW:
			error = "STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			error = "STACK_UNDERFLOW";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	
	// update the window dimensions for retina display
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
	WindowDimensions windowDimensions = { screenWidth, screenHeight };
	myWindow.setWindowDimensions(windowDimensions);

	// set the shader program
	mainShader.useShaderProgram();

	// update the projection
	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
		nearPlane, farPlane);
	GLint projectionLoc = glGetUniformLocation(mainShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glViewport(0, 0, screenWidth, screenHeight);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
}

void processKeyboardInputs() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angle -= 1.0f;
		if (angle < 0.0f)
			angle += 360.0f;
		// update model matrix for teapot
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		// update normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angle += 1.0f;
		if (angle > 360.0f)
			angle -= 360.0f;
		// update model matrix for teapot
		model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
		// update normal matrix for teapot
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (pressedKeys[GLFW_KEY_F]) {
		// TODO: implement fog
		if (!fogActive) {
			fogActive = true;
			mainShader.useShaderProgram();
			fogInit = 1;
			fogInitLoc = glGetUniformLocation(mainShader.shaderProgram, "fogInit");
			glUniform1i(fogInitLoc, fogInit);
		}
		else {
			fogActive = false;
			mainShader.useShaderProgram();
			fogInit = 1;
			fogInitLoc = glGetUniformLocation(mainShader.shaderProgram, "fogInit");
			glUniform1i(fogInitLoc, fogInit);
		}
	}

	// increase the intensity of fog
	if (pressedKeys[GLFW_KEY_M])
	{
		fogDensity = glm::min(fogDensity + 0.0001f, 1.0f);
	}

	// decrease the intensity of fog
	if (pressedKeys[GLFW_KEY_N])
	{
		fogDensity = glm::max(fogDensity - 0.0001f, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_P]) {
		// reset the preview angle if stopping preview
		if (cameraPreview) {
			previewAngle = 0.0f;
			cameraPreview = false;
			moveCameraToInitialPos();
		}
		else
		{
			cameraPreview = true;
		}
	}


	if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
		myCamera.move(gps::MOVE_UP, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_LEFT_CONTROL]) {
		myCamera.move(gps::MOVE_DOWN, cameraSpeed);
	}

	// ########################
	//		VIEW CONTROLS
	// ########################

	// line view
	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// point view
	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	// normal view
	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// TODO: Implement camera preview

	// ########################
	//		VIEW CONTROLS
	// ########################
}

void initOpenGLWindow() {
	myWindow.Create(screenWidth, screenHeight, "OpenGL Project");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height); // set the viewport
	glEnable(GL_FRAMEBUFFER_SRGB); // set the color range
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initFBO() {
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &shadowMapTexture);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void initModels() {
	fullScene.LoadModel("objects/full_scene/scene.obj");
}

void initShaders() {
	mainShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	// lightShader.loadShader("shaders/light.vert", "shaders/light.frag");
	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
}

void initUniforms() {
	mainShader.useShaderProgram();

	// create model matrix for model
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(mainShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(mainShader.shaderProgram, "view");
	// send view matrix to shader
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// compute normal matrix
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(mainShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
		(float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
		nearPlane, farPlane);
	projectionLoc = glGetUniformLocation(mainShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(mainShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(mainShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}


void renderFullScene(gps::Shader shader) {
//	// select active shader program
	shader.useShaderProgram();
	//send teapot model matrix data to shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//send teapot normal matrix data to shader
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	// draw teapot
	fullScene.Draw(shader);
}

void renderScene() {
	
	// clear the frame buffer and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Depth buffer rendering
	depthMapShader.useShaderProgram();

	glUniformMatrix4fv(glGetUniformLocation(
		depthMapShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	renderFullScene(mainShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2nd step: render the scene

	mainShader.useShaderProgram();

	// send lightSpace matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

	// send view matrix to shader
	view = myCamera.getViewMatrix();

	glUniformMatrix4fv(glGetUniformLocation(mainShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	// compute light direction transformation matrix
	lightDirMatrix = glm::mat3(glm::inverseTranspose(view));
	// send lightDir matrix data to shader
	glUniformMatrix3fv(lightDirMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightDirMatrix));

	glViewport(0, 0, screenWidth, screenHeight);
	mainShader.useShaderProgram();

	// bind the depth map
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glUniform1i(glGetUniformLocation(mainShader.shaderProgram, "shadowMap"), 3);


	glUniform1f(glGetUniformLocation(mainShader.shaderProgram, "fogDensity"), fogDensity);

	renderFullScene(mainShader);

	// camera preview
	preview();
}

void updateView() {
	// update view matrix
	view = myCamera.getViewMatrix();
	mainShader.useShaderProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	// compute normal matrix for teapot
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

void cleanup() {
	myWindow.Delete();
	//cleanup code for your own data
}

int main(int argc, const char* argv[]) {

	// Create the application window
	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	initOpenGLState(); // initialize the GL 
	initFBO(); // init the depthMap FBO
	initModels(); // load the models
	initShaders(); // load the shaders
	initUniforms(); // initialize the uniforms
	setWindowCallbacks();
	moveCameraToInitialPos();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
		


		// calculate the camera speed each frame, 
		// relative to the previous one
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		cameraSpeed = 4.0f * deltaTime;


		std::cout 
			<< "Camera position (x, y, z): " 
			<< "("  << myCamera.getCameraPosition().x << ", " 
			<< myCamera.getCameraPosition().y << ", "
			<< myCamera.getCameraPosition().z << ")" << std::endl;

		std::cout << "Camera target (x, y, z): "
			<< "(" << myCamera.getCameraFrontDirection().x << ", "
			<< myCamera.getCameraFrontDirection().x << ", "
			<< myCamera.getCameraFrontDirection().x << ")" << std::endl;
		
		

		processKeyboardInputs();
		updateView();
		renderScene();

		glfwSwapBuffers(myWindow.getWindow()); // swap between front and back buffer
		glfwPollEvents(); // wait for event

		glCheckError();
	}

	cleanup();

	return EXIT_SUCCESS;
}
