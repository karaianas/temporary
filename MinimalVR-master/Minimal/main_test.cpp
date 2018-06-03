/************************************************************************************
Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
Copyright   :   Copyright Brad Davis. All Rights reserved.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
************************************************************************************/


#include <iostream>
#include <memory>
#include <exception>
#include <algorithm>

#include <Windows.h>

#define __STDC_FORMAT_MACROS 1

#define FAIL(X) throw std::runtime_error(X)

///////////////////////////////////////////////////////////////////////////////
//
// GLM is a C++ math library meant to mirror the syntax of GLSL 
//

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

// Import the most commonly used types into the default namespace
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

///////////////////////////////////////////////////////////////////////////////
//
// GLEW gives cross platform access to OpenGL 3.x+ functionality.  
//

#include <GL/glew.h>

// My implementation
#include "Scene.h"
#include "Calibration.h"
#include "Cave.h"
#include <time.h>
#include <queue>
#include <stdlib.h>
#include <time.h> 

// Final project testing
//#include "AvatarHand.h"
//#include "AvaHand.h"

bool checkFramebufferStatus(GLenum target = GL_FRAMEBUFFER) {
	GLuint status = glCheckFramebufferStatus(target);
	switch (status) {
	case GL_FRAMEBUFFER_COMPLETE:
		return true;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		std::cerr << "framebuffer incomplete attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		std::cerr << "framebuffer missing attachment" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		std::cerr << "framebuffer incomplete draw buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		std::cerr << "framebuffer incomplete read buffer" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		std::cerr << "framebuffer incomplete multisample" << std::endl;
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		std::cerr << "framebuffer incomplete layer targets" << std::endl;
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		std::cerr << "framebuffer unsupported internal format or image" << std::endl;
		break;

	default:
		std::cerr << "other framebuffer error" << std::endl;
		break;
	}

	return false;
}

bool checkGlError() {
	GLenum error = glGetError();
	if (!error) {
		return false;
	}
	else {
		switch (error) {
		case GL_INVALID_ENUM:
			std::cerr << ": An unacceptable value is specified for an enumerated argument.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_INVALID_VALUE:
			std::cerr << ": A numeric argument is out of range.The offending command is ignored and has no other side effect than to set the error flag";
			break;
		case GL_INVALID_OPERATION:
			std::cerr << ": The specified operation is not allowed in the current state.The offending command is ignored and has no other side effect than to set the error flag..";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << ": The framebuffer object is not complete.The offending command is ignored and has no other side effect than to set the error flag.";
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << ": There is not enough memory left to execute the command.The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to underflow.";
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << ": An attempt has been made to perform an operation that would cause an internal stack to overflow.";
			break;
		}
		return true;
	}
}

void glDebugCallbackHandler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, GLvoid* data) {
	OutputDebugStringA(msg);
	std::cout << "debug call: " << msg << std::endl;
}

//////////////////////////////////////////////////////////////////////
//
// GLFW provides cross platform window creation
//

#include <GLFW/glfw3.h>

namespace glfw {
	inline GLFWwindow * createWindow(const uvec2 & size, const ivec2 & position = ivec2(INT_MIN)) {
		GLFWwindow * window = glfwCreateWindow(size.x, size.y, "glfw", nullptr, nullptr);
		if (!window) {
			FAIL("Unable to create rendering window");
		}
		if ((position.x > INT_MIN) && (position.y > INT_MIN)) {
			glfwSetWindowPos(window, position.x, position.y);
		}
		return window;
	}
}

// A class to encapsulate using GLFW to handle input and render a scene
class GlfwApp {

protected:
	uvec2 windowSize;
	ivec2 windowPosition;
	GLFWwindow * window{ nullptr };
	unsigned int frame{ 0 };

public:
	GlfwApp() {
		// Initialize the GLFW system for creating and positioning windows
		if (!glfwInit()) {
			FAIL("Failed to initialize GLFW");
		}
		glfwSetErrorCallback(ErrorCallback);
	}

	virtual ~GlfwApp() {
		if (nullptr != window) {
			glfwDestroyWindow(window);
		}
		glfwTerminate();
	}

	virtual int run() {
		preCreate();

		window = createRenderingTarget(windowSize, windowPosition);

		if (!window) {
			std::cout << "Unable to create OpenGL window" << std::endl;
			return -1;
		}

		postCreate();

		initGl();

		while (!glfwWindowShouldClose(window)) {
			++frame;
			glfwPollEvents();
			update();
			draw();
			finishFrame();
		}

		shutdownGl();

		return 0;
	}


protected:
	virtual GLFWwindow * createRenderingTarget(uvec2 & size, ivec2 & pos) = 0;

	virtual void draw() = 0;

	void preCreate() {
		glfwWindowHint(GLFW_DEPTH_BITS, 16);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	}


	void postCreate() {
		glfwSetWindowUserPointer(window, this);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, MouseButtonCallback);
		glfwMakeContextCurrent(window);

		// Initialize the OpenGL bindings
		// For some reason we have to set this experminetal flag to properly
		// init GLEW if we use a core context.
		glewExperimental = GL_TRUE;
		if (0 != glewInit()) {
			FAIL("Failed to initialize GLEW");
		}
		glGetError();

		if (GLEW_KHR_debug) {
			GLint v;
			glGetIntegerv(GL_CONTEXT_FLAGS, &v);
			if (v & GL_CONTEXT_FLAG_DEBUG_BIT) {
				//glDebugMessageCallback(glDebugCallbackHandler, this);
			}
		}
	}

	virtual void initGl() {
	}

	virtual void shutdownGl() {
	}

	virtual void finishFrame() {
		glfwSwapBuffers(window);
	}

	virtual void destroyWindow() {
		glfwSetKeyCallback(window, nullptr);
		glfwSetMouseButtonCallback(window, nullptr);
		glfwDestroyWindow(window);
	}

	virtual void onKey(int key, int scancode, int action, int mods) {
		if (GLFW_PRESS != action) {
			return;
		}

		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, 1);
			return;
		}
	}

	virtual void update() {}

	virtual void onMouseButton(int button, int action, int mods) {}

protected:
	virtual void viewport(const ivec2 & pos, const uvec2 & size) {
		glViewport(pos.x, pos.y, size.x, size.y);
	}

private:

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onKey(key, scancode, action, mods);
	}

	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		GlfwApp * instance = (GlfwApp *)glfwGetWindowUserPointer(window);
		instance->onMouseButton(button, action, mods);
	}

	static void ErrorCallback(int error, const char* description) {
		FAIL(description);
	}
};

//////////////////////////////////////////////////////////////////////
//
// The Oculus VR C API provides access to information about the HMD
//

#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

namespace ovr {

	// Convenience method for looping over each eye with a lambda
	template <typename Function>
	inline void for_each_eye(Function function) {
		for (ovrEyeType eye = ovrEyeType::ovrEye_Left;
			eye < ovrEyeType::ovrEye_Count;
			eye = static_cast<ovrEyeType>(eye + 1)) {
			function(eye);
		}
	}

	inline mat4 toGlm(const ovrMatrix4f & om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline mat4 toGlm(const ovrFovPort & fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline vec3 toGlm(const ovrVector3f & ov) {
		return glm::make_vec3(&ov.x);
	}

	inline vec2 toGlm(const ovrVector2f & ov) {
		return glm::make_vec2(&ov.x);
	}

	inline uvec2 toGlm(const ovrSizei & ov) {
		return uvec2(ov.w, ov.h);
	}

	inline quat toGlm(const ovrQuatf & oq) {
		return glm::make_quat(&oq.x);
	}

	inline mat4 toGlm(const ovrPosef & op) {
		mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		mat4 translation = glm::translate(mat4(), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const mat4 & m) {
		ovrMatrix4f result;
		mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const vec3 & v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const vec2 & v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const uvec2 & v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const quat & q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}

class RiftManagerApp {
protected:
	ovrSession _session;
	ovrHmdDesc _hmdDesc;
	ovrGraphicsLuid _luid;

public:
	RiftManagerApp() {
		if (!OVR_SUCCESS(ovr_Create(&_session, &_luid))) {
			FAIL("Unable to create HMD session");
		}

		_hmdDesc = ovr_GetHmdDesc(_session);
	}

	~RiftManagerApp() {
		ovr_Destroy(_session);
		_session = nullptr;
	}
};

class RiftApp : public GlfwApp, public RiftManagerApp {
public:

private:
	GLuint _fbo{ 0 };
	GLuint _depthBuffer{ 0 };

	//GLuint _fbo2{ 0 };
	//GLuint _depthBuffer2{ 0 };

	ovrTextureSwapChain _eyeTexture;

	GLuint _mirrorFbo{ 0 };
	ovrMirrorTexture _mirrorTexture;

	ovrEyeRenderDesc _eyeRenderDescs[2];

	mat4 _eyeProjections[2];

	ovrLayerEyeFov _sceneLayer;
	ovrViewScaleDesc _viewScaleDesc;

	uvec2 _renderTargetSize;
	uvec2 _mirrorSize;

	// My implementation
	bool cycleX;
	bool isEC;
	bool cycleA;
	bool isDebug;
	int cycleAMode;

	// -------------------------------------------------------------
	bool sizeChange;
	float cubeSize;
	bool isTrig;
	bool cycleB;
	bool isFreeze;
	glm::vec3 handPos_;
	glm::vec3 eyePos_[2];
	glm::mat4 V_[2];
	glm::mat4 V_fix[2];
	int saveState;
	glm::mat4 R_;
	glm::mat4 Re_;
	int randEye;
	int randPlane;
	// -------------------------------------------------------------

	int skybox_left;
	int skybox_right;
	int eye_left;
	int eye_right;
	bool isCube;

	glm::vec4 t_left;
	glm::vec4 t_right;
	glm::mat4 R_left;
	glm::mat4 R_right;

	ovrPosef eyePoses[2];

	unsigned char * dataBufferX;
	ovrHapticsBuffer bufferX;
	int bufferSize;


public:

	RiftApp() {
		using namespace ovr;
		_viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;

		memset(&_sceneLayer, 0, sizeof(ovrLayerEyeFov));
		_sceneLayer.Header.Type = ovrLayerType_EyeFov;
		_sceneLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;

		ovr::for_each_eye([&](ovrEyeType eye) {
			ovrEyeRenderDesc& erd = _eyeRenderDescs[eye] = ovr_GetRenderDesc(_session, eye, _hmdDesc.DefaultEyeFov[eye]);
			ovrMatrix4f ovrPerspectiveProjection =
				ovrMatrix4f_Projection(erd.Fov, 0.01f, 1000.0f, ovrProjection_ClipRangeOpenGL);
			_eyeProjections[eye] = ovr::toGlm(ovrPerspectiveProjection);
			_viewScaleDesc.HmdToEyePose[eye] = erd.HmdToEyePose;

			ovrFovPort & fov = _sceneLayer.Fov[eye] = _eyeRenderDescs[eye].Fov;
			auto eyeSize = ovr_GetFovTextureSize(_session, eye, fov, 1.0f);
			_sceneLayer.Viewport[eye].Size = eyeSize;
			_sceneLayer.Viewport[eye].Pos = { (int)_renderTargetSize.x, 0 };

			_renderTargetSize.y = std::max(_renderTargetSize.y, (uint32_t)eyeSize.h);
			_renderTargetSize.x += eyeSize.w;
		});
		// Make the on screen window 1/4 the resolution of the render target
		_mirrorSize = _renderTargetSize;
		_mirrorSize /= 4;
	}

protected:
	GLFWwindow * createRenderingTarget(uvec2 & outSize, ivec2 & outPosition) override {
		return glfw::createWindow(_mirrorSize);
	}

	void initGl() override {
		GlfwApp::initGl();

		// Disable the v-sync for buffer swap
		glfwSwapInterval(0);

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Width = _renderTargetSize.x;
		desc.Height = _renderTargetSize.y;
		desc.MipLevels = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.SampleCount = 1;
		desc.StaticImage = ovrFalse;
		ovrResult result = ovr_CreateTextureSwapChainGL(_session, &desc, &_eyeTexture);
		_sceneLayer.ColorTexture[0] = _eyeTexture;
		if (!OVR_SUCCESS(result)) {
			FAIL("Failed to create swap textures");
		}

		int length = 0;
		result = ovr_GetTextureSwapChainLength(_session, _eyeTexture, &length);
		if (!OVR_SUCCESS(result) || !length) {
			FAIL("Unable to count swap chain textures");
		}
		for (int i = 0; i < length; ++i) {
			GLuint chainTexId;
			ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, i, &chainTexId);
			glBindTexture(GL_TEXTURE_2D, chainTexId);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		// Set up the framebuffer object
		glGenFramebuffers(1, &_fbo);
		glGenRenderbuffers(1, &_depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, _renderTargetSize.x, _renderTargetSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// *********************************
	/*	glGenFramebuffers(1, &_fbo2);
		glGenTextures(1, &_depthBuffer2);
		glBindTexture(GL_TEXTURE_2D, _depthBuffer2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 500, 500, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);*/
		// *********************************

		ovrMirrorTextureDesc mirrorDesc;
		memset(&mirrorDesc, 0, sizeof(mirrorDesc));
		mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		mirrorDesc.Width = _mirrorSize.x;
		mirrorDesc.Height = _mirrorSize.y;
		if (!OVR_SUCCESS(ovr_CreateMirrorTextureGL(_session, &mirrorDesc, &_mirrorTexture))) {
			FAIL("Could not create mirror texture");
		}
		glGenFramebuffers(1, &_mirrorFbo);

		//-------------------------
		cycleX = false;
		isEC = false;
		cycleA = false;
		cycleAMode = 0;

		// -------------------------------------------------------------    
		sizeChange = false;
		cubeSize = 0.3f;
		isTrig = false;
		isDebug = false;
		cycleB = false;
		isFreeze = false;
		handPos_ = glm::vec3(0.0f);
		eyePos_[0] = glm::vec3(0.0f);
		eyePos_[1] = glm::vec3(0.0f);
		//V_fix[0] = glm::mat4(1.0f);
		//V_fix[1] = glm::mat4(1.0f);
		ovrPosef temp0 = eyePoses[0];
		ovrPosef temp1 = eyePoses[1];
		temp0.Position.x = -0.03f;
		temp0.Position.y = 0.0f;
		temp0.Position.z = 0.0f;
		temp1.Position.x = 0.03f;
		temp1.Position.y = 0.0f;
		temp1.Position.z = 0.0f;
		V_fix[0] = ovr::toGlm(temp0);
		V_fix[1] = ovr::toGlm(temp1);
		//V_fix[0][3] = glm::vec4(-0.03f, 1.6f, 0.0f, 1.0f);
		//V_fix[1][3] = glm::vec4(0.03f, 1.6f, 0.0f, 1.0f);

		saveState = -1;
		// -------------------------------------------------------------

		skybox_left = 0;
		skybox_right = 1;
		isCube = true;

		eye_left = 0;
		eye_right = 1;

		t_left = ovr::toGlm(eyePoses[eye_left])[3];
		t_right = ovr::toGlm(eyePoses[eye_right])[3];
		R_left = ovr::toGlm(eyePoses[eye_left]);
		R_right = ovr::toGlm(eyePoses[eye_right]);
		//-------------------------

		// Haptic
		bufferSize = 256;
		dataBufferX = (unsigned char *)malloc(bufferSize);
		for (int i = 0; i < bufferSize; i++)
		{
			dataBufferX[i] = (unsigned char)0;
			if (i % 5 == 0) dataBufferX[i] = 255;
		}

		bufferX.SubmitMode = ovrHapticsBufferSubmit_Enqueue;
		bufferX.SamplesCount = bufferSize;
		bufferX.Samples = (void *)dataBufferX;
	}

	void onKey(int key, int scancode, int action, int mods) override {
		if (GLFW_PRESS == action) switch (key) {
		case GLFW_KEY_R:
			ovr_RecenterTrackingOrigin(_session);
			return;
		}

		GlfwApp::onKey(key, scancode, action, mods);
	}

	void draw() final override {

		// Query Touch controllers. Query their parameters:
		double displayMidpointSeconds = ovr_GetPredictedDisplayTime(_session, 0);
		ovrTrackingState trackState = ovr_GetTrackingState(_session, displayMidpointSeconds, ovrTrue);

		// Process controller status. Useful to know if controller is being used at all, and if the cameras can see it. 
		// Bits reported:
		// Bit 1: ovrStatus_OrientationTracked  = Orientation is currently tracked (connected and in use)
		// Bit 2: ovrStatus_PositionTracked     = Position is currently tracked (false if out of range)
		unsigned int handStatus[2];
		handStatus[0] = trackState.HandStatusFlags[0];
		handStatus[1] = trackState.HandStatusFlags[1];
		// Display status for debug purposes:
		//std::cerr << "handStatus[left]  = " << handStatus[ovrHand_Left] << std::endl;
		//std::cerr << "handStatus[right] = " << handStatus[ovrHand_Right] << std::endl;

		// Process controller position and orientation:
		ovrPosef handPoses[2];
		handPoses[0] = trackState.HandPoses[0].ThePose;
		handPoses[1] = trackState.HandPoses[1].ThePose;

		ovrVector3f handPosition[2];
		handPosition[0] = handPoses[0].Position;
		handPosition[1] = handPoses[1].Position;

		ovrInputState inputState;
		if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &inputState)))
		{
			// Change skybox mode
			if (inputState.Buttons & ovrButton_X)
			{
				if (!cycleX)
				{
					cycleX = true;
					if (isEC)
					{
						isEC = false;
						randEye = -1;
						randPlane = -1;
					}
					else
					{
						isEC = true;
						srand(time(NULL));
						randEye = rand() % 2;
						randPlane = rand() % 3;
					}

					/*
					cycleXMode += 1;
					cycleXMode %= 4;

					if (cycleXMode == 0)
					{
					skybox_left = 0;
					skybox_right = 1;
					isCube = true;
					}
					else if (cycleXMode == 1)
					{
					skybox_left = 0;
					skybox_right = 1;
					isCube = false;
					}
					else if (cycleXMode == 2)
					{
					skybox_left = 0;
					skybox_right = 0;
					isCube = false;
					}
					else
					{
					skybox_left = 2;
					skybox_right = 2;
					isCube = true;
					}
					*/
				}
			}
			else
				cycleX = false;

			// Change eye mode
			if (inputState.Buttons & ovrButton_A)
			{
				if (!cycleA)
				{
					cycleA = true;

					/*cycleAMode += 1;
					cycleAMode %= 4;

					if (cycleAMode == 0)
					{
					eye_left = 0;
					eye_right = 1;
					}
					else if (cycleAMode == 1)
					{
					eye_left = 0;
					eye_right = -1;
					}
					else if (cycleAMode == 2)
					{
					eye_left = -1;
					eye_right = 1;
					}
					else
					{
					eye_left = 1;
					eye_right = 0;
					}*/

					if (isDebug)
						isDebug = false;
					else
						isDebug = true;
				}
			}
			else
				cycleA = false;

			// -------------------------------------------------------------
			// Change cube size
			if (inputState.Thumbstick[ovrHand_Right].x > 0.9f)
			{
				cubeSize += 0.01f;
				if (cubeSize > 0.5f)
					cubeSize = 0.5f;
				changeCubeSize(cubeSize);

			}
			else if (inputState.Thumbstick[ovrHand_Right].x < -0.9f)
			{

				cubeSize -= 0.01f;
				if (cubeSize < 0.01f)
					cubeSize = 0.01f;
				changeCubeSize(cubeSize);
			}
			if (inputState.Buttons & ovrButton_RThumb)
			{
				cubeSize = 0.3f;
				changeCubeSize(cubeSize);
			}

			// Move the cube position
			// Left/right
			if (inputState.Thumbstick[ovrHand_Left].x > 0.9f)
			{
				moveCubePos(glm::vec3(0.01f, 0.0f, 0.0f));
			}
			else if (inputState.Thumbstick[ovrHand_Left].x < -0.9f)
			{
				moveCubePos(glm::vec3(-0.01f, 0.0f, 0.0f));
			}
			// Forward/back
			if (inputState.Thumbstick[ovrHand_Left].y > 0.9f)
			{
				moveCubePos(glm::vec3(0.0f, 0.0f, 0.01f));
			}
			else if (inputState.Thumbstick[ovrHand_Left].y < -0.9f)
			{
				moveCubePos(glm::vec3(0.0f, 0.0f, -0.01f));
			}
			// Up/down
			if (inputState.Thumbstick[ovrHand_Right].y > 0.9f)
			{
				moveCubePos(glm::vec3(0.0f, 0.01f, 0.0f));
			}
			else if (inputState.Thumbstick[ovrHand_Right].y < -0.9f)
			{
				moveCubePos(glm::vec3(0.0f, -0.01f, 0.0f));
			}

			// Change viewpoint
			if (inputState.IndexTrigger[ovrHand_Right] > 0.9f)
				isTrig = true;
			else
				isTrig = false;

			// Change tracking mode
			if (inputState.Buttons & ovrButton_B)
			{
				if (!cycleB)
				{
					cycleB = true;
					if (isFreeze)
					{
						isFreeze = false;
						saveState = -1;
					}
					else
					{
						isFreeze = true;
						if (isTrig)
							saveState = 0;
						else
							saveState = 1;
					}

					//cout << saveState << endl;
					//setFreeze(saveState);
				}
			}
			else
				cycleB = false;
			// -------------------------------------------------------------

		}
		//cout << saveState << endl;

		ovr_GetEyePoses(_session, frame, true, _viewScaleDesc.HmdToEyePose, eyePoses, &_sceneLayer.SensorSampleTime);

		int curIndex;
		ovr_GetTextureSwapChainCurrentIndex(_session, _eyeTexture, &curIndex);
		GLuint curTexId;
		ovr_GetTextureSwapChainBufferGL(_session, _eyeTexture, curIndex, &curTexId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// ----------------------------------------------------------
		glm::mat4 V[2];
		V[0] = ovr::toGlm(eyePoses[0]);
		V[1] = ovr::toGlm(eyePoses[1]);
		// ----------------------------------------------------------

		if (saveState == -1)
		{
			handPos_ = glm::vec3(handPosition[1].x, handPosition[1].y, handPosition[1].z);
			eyePos_[0] = glm::vec3(eyePoses[0].Position.x, eyePoses[0].Position.y, eyePoses[0].Position.z);
			eyePos_[1] = glm::vec3(eyePoses[1].Position.x, eyePoses[1].Position.y, eyePoses[1].Position.z);
			R_ = glm::toMat4(glm::quat(handPoses[1].Orientation.w, handPoses[1].Orientation.x, handPoses[1].Orientation.y, handPoses[1].Orientation.z));
			//Re_ = glm::toMat4(glm::quat(handPoses[1].Orientation.w, handPoses[1].Orientation.x, handPoses[1].Orientation.y, handPoses[1].Orientation.z));
			V_[0] = ovr::toGlm(eyePoses[0]);
			V_[1] = ovr::toGlm(eyePoses[1]);
		}
		else if (saveState == 0)
		{
			handPos_[0] = handPos_[0];
			handPos_[1] = handPos_[1];
			R_ = R_;
			eyePos_[0] = glm::vec3(eyePoses[0].Position.x, eyePoses[0].Position.y, eyePoses[0].Position.z);
			eyePos_[1] = glm::vec3(eyePoses[1].Position.x, eyePoses[1].Position.y, eyePoses[1].Position.z);
			V_[0] = V_[0];
			V_[1] = V_[1];
		}
		else
		{
			handPos_ = glm::vec3(handPosition[1].x, handPosition[1].y, handPosition[1].z);
			R_ = glm::toMat4(glm::quat(handPoses[1].Orientation.w, handPoses[1].Orientation.x, handPoses[1].Orientation.y, handPoses[1].Orientation.z));
			eyePos_[0] = eyePos_[0];
			eyePos_[1] = eyePos_[1];
			V_[0] = V_[0];
			V_[1] = V_[1];
		}

		//V_[0][3] = V[0][3];
		//V_[1][3] = V[1][3];

		glm::vec3 handPos[2];
		glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(handPos_));
		handPos[0] = handPos_ - glm::vec3(0.03f, 0.0f, 0.0f);
		handPos[1] = handPos_ + glm::vec3(0.03f, 0.0f, 0.0f);
		handPos[0] = T * R_ * glm::inverse(T) * glm::vec4(handPos[0], 1.0f);
		handPos[1] = T * R_ * glm::inverse(T) * glm::vec4(handPos[1], 1.0f);


		glm::vec3 godEyePos[2];
		if (saveState == -1)
		{
			if (isTrig)
			{
				godEyePos[0] = handPos[0];
				godEyePos[1] = handPos[1];
			}
			else
			{
				godEyePos[0] = eyePos_[0];
				godEyePos[1] = eyePos_[1];
			}
		}
		else if (saveState == 0)
		{
			godEyePos[0] = handPos[0];
			godEyePos[1] = handPos[1];
		}
		else
		{
			godEyePos[0] = eyePos_[0];
			godEyePos[1] = eyePos_[1];
		}
		//cout << godEyePos[0].x << " " << godEyePos[0].y << " " << godEyePos[0].z << endl;
		// ----------------------------------------------------------


		// Left eye
		const auto& vp0 = _sceneLayer.Viewport[0];
		glViewport(vp0.Pos.x, vp0.Pos.y, vp0.Size.w, vp0.Size.h);
		_sceneLayer.RenderPose[0] = eyePoses[0];

		if (eye_left != -1)
		{
			glm::mat4 V_left = V[eye_left];
			glm::mat4 V_inv = glm::inverse(V_left);

			setViewport(vp0.Pos.x, vp0.Pos.y);
			setEye(godEyePos[0]);
			setViewMatrix(glm::inverse(V_fix[0]));


			if (randEye == 0)
				renderCAVE(_eyeProjections[eye_left], V_inv, _fbo, 1, randPlane);
			else
				renderCAVE(_eyeProjections[eye_left], V_inv, _fbo, 0, randPlane);
			renderController(_eyeProjections[eye_left], V_inv, glm::vec3(handPosition[1].x, handPosition[1].y, handPosition[1].z));

			if (isDebug)
			{
				renderPyramid(_eyeProjections[eye_left], V_inv, godEyePos[0], 0);
				renderPyramid(_eyeProjections[eye_left], V_inv, godEyePos[1], 1);
			}

			//glBindFramebuffer(GL_FRAMEBUFFER, _fbo2);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, curTexId, 0);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthBuffer2, 0);

			//glViewport(0, 0, vp0.Size.w, vp0.Size.h);
			//glDepthMask(GL_TRUE);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glEnable(GL_FRAMEBUFFER_SRGB);

			renderHands(_eyeProjections[eye_left], V_inv, godEyePos[eye_left], trackState, inputState);
			
			//glBindFramebuffer(GL_FRAMEBUFFER, _fbo2);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
			//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
		}

		// Right eye
		const auto& vp1 = _sceneLayer.Viewport[1];
		glViewport(vp1.Pos.x, vp1.Pos.y, vp1.Size.w, vp1.Size.h);
		_sceneLayer.RenderPose[1] = eyePoses[1];

		if (eye_right != -1)
		{
			glm::mat4 V_right = V[eye_right];
			glm::mat4 V_inv = glm::inverse(V_right);

			setViewport(vp1.Pos.x, vp1.Pos.y);
			setEye(godEyePos[1]);
			setViewMatrix(glm::inverse(V_fix[0]));

			if (randEye == 1)
				renderCAVE(_eyeProjections[eye_right], V_inv, _fbo, 1, randPlane);
			else
				renderCAVE(_eyeProjections[eye_right], V_inv, _fbo, 0, randPlane);
			renderController(_eyeProjections[eye_right], V_inv, glm::vec3(handPosition[1].x, handPosition[1].y, handPosition[1].z));
			if (isDebug)
			{
				renderPyramid(_eyeProjections[eye_right], V_inv, godEyePos[0], 0);
				renderPyramid(_eyeProjections[eye_right], V_inv, godEyePos[1], 1);
			}

			// ***************
			//renderHands(_eyeProjections[eye_right], V_inv, godEyePos[eye_right], trackState, inputState);
		}
		//renderScene(_eyeProjections[eye_right], glm::inverse(V[eye_right]), isEC);


		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		ovr_CommitTextureSwapChain(_session, _eyeTexture);
		ovrLayerHeader* headerList = &_sceneLayer.Header;
		ovr_SubmitFrame(_session, frame, &_viewScaleDesc, &headerList, 1);

		GLuint mirrorTextureId;
		ovr_GetMirrorTextureBufferGL(_session, _mirrorTexture, &mirrorTextureId);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, _mirrorFbo);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTextureId, 0);
		glBlitFramebuffer(0, 0, _mirrorSize.x, _mirrorSize.y, 0, _mirrorSize.y, _mirrorSize.x, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	}

	virtual void renderHands(glm::mat4 & projection, const glm::mat4 & view, glm::vec3 pos, ovrTrackingState trackState, ovrInputState inputState) = 0;
	virtual void renderScene(glm::mat4 & projection, const glm::mat4 & view, bool isEC) = 0;
	virtual void renderCAVE(glm::mat4 & projection, const glm::mat4 & view, int fboID, int randEye, int randPlane) = 0;
	virtual void renderController(glm::mat4 & projection, const glm::mat4 & view, glm::vec3 pos) = 0;
	virtual void renderPyramid(glm::mat4 & projection, const glm::mat4 & view, glm::vec3 pos, bool lr) = 0;
	virtual void setViewport(int w0, int h0) = 0;
	virtual void setEye(glm::vec3 eyePos) = 0;
	virtual void setViewMatrix(const glm::mat4 & view) = 0;
	virtual void changeCubeSize(float cubeSize) = 0;
	virtual void moveCubePos(glm::vec3 t) = 0;
};

//////////////////////////////////////////////////////////////////////
//
// The remainder of this code is specific to the scene we want to 
// render.  I use oglplus to render an array of cubes, but your 
// application would perform whatever rendering you want
//


//////////////////////////////////////////////////////////////////////
//
// OGLplus is a set of wrapper classes for giving OpenGL a more object
// oriented interface
//
#define OGLPLUS_USE_GLCOREARB_H 0
#define OGLPLUS_USE_GLEW 1
#define OGLPLUS_USE_BOOST_CONFIG 0
#define OGLPLUS_NO_SITE_CONFIG 1
#define OGLPLUS_LOW_PROFILE 1

#pragma warning( disable : 4068 4244 4267 4065)
#include <oglplus/config/basic.hpp>
#include <oglplus/config/gl.hpp>
#include <oglplus/all.hpp>
#include <oglplus/interop/glm.hpp>
#include <oglplus/bound/texture.hpp>
#include <oglplus/bound/framebuffer.hpp>
#include <oglplus/bound/renderbuffer.hpp>
#include <oglplus/bound/buffer.hpp>
#include <oglplus/shapes/cube.hpp>
#include <oglplus/shapes/wrapper.hpp>
#pragma warning( default : 4068 4244 4267 4065)



namespace Attribute {
	enum {
		Position = 0,
		TexCoord0 = 1,
		Normal = 2,
		Color = 3,
		TexCoord1 = 4,
		InstanceTransform = 5,
	};
}

static const char * VERTEX_SHADER = R"SHADER(
#version 410 core
uniform mat4 ProjectionMatrix = mat4(1);
uniform mat4 CameraMatrix = mat4(1);
layout(location = 0) in vec4 Position;
layout(location = 2) in vec3 Normal;
layout(location = 5) in mat4 InstanceTransform;
out vec3 vertNormal;
void main(void) {
   mat4 ViewXfm = CameraMatrix * InstanceTransform;
   //mat4 ViewXfm = CameraMatrix;
   vertNormal = Normal;
   gl_Position = ProjectionMatrix * ViewXfm * Position;
}
)SHADER";

static const char * FRAGMENT_SHADER = R"SHADER(
#version 410 core
in vec3 vertNormal;
out vec4 fragColor;
void main(void) {
    vec3 color = vertNormal;
    if (!all(equal(color, abs(color)))) {
        color = vec3(1.0) - abs(color);
    }
    fragColor = vec4(color, 1.0);
}
)SHADER";

// An example application that renders a simple cube
class ExampleApp : public RiftApp {

	// My implementation
	std::shared_ptr<Cave> cave;
	// *************************
	//std::shared_ptr<AvatarHand> avatarHands;
	//std::shared_ptr<AvaHand> avatarHands;
	// *************************


public:
	ExampleApp() { }

protected:
	void initGl() override {
		RiftApp::initGl();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		ovr_RecenterTrackingOrigin(_session);

		cave = std::shared_ptr<Cave>(new Cave());
		//avatarHands = std::shared_ptr<AvaHand>(new AvaHand());
	}

	void shutdownGl() override {
	}

	void renderHands(glm::mat4 & projection, const glm::mat4 & view, glm::vec3 pos, ovrTrackingState trackState, ovrInputState inputState) {
		cave->drawHands(view, projection, pos, trackState, inputState);
	}

	void renderScene(glm::mat4 & projection, const glm::mat4 & view, bool isEC) {
		cave->drawScene(view, projection, isEC);
	}

	void renderCAVE(glm::mat4 & projection, const glm::mat4 & view, int fboID, int randEye, int randPlane) {
		cave->drawMainScene(view, projection, fboID, randEye, randPlane);
	}

	void renderController(glm::mat4 & projection, const glm::mat4 & view, glm::vec3 pos) {
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
		model[3] = glm::vec4(pos, 1.0f);
		cave->drawController(model, view, projection);
	}

	void renderPyramid(glm::mat4 & projection, const glm::mat4 & view, glm::vec3 pos, bool lr) {
		cave->drawPyramid(view, projection, pos, lr);
	}

	void setViewport(int w0, int h0) {
		cave->setViewport(w0, h0);
	}

	void setEye(glm::vec3 eyePos) {
		cave->setEye(eyePos);
	}

	void setViewMatrix(const glm::mat4 & view) {
		cave->setViewMatrix(view);
	}

	void changeCubeSize(float cubeSize) {
		cave->changeCubeSize(cubeSize);
	}

	void moveCubePos(glm::vec3 t) {
		cave->moveCubePos(t);
	}
};

// Execute our example class
//int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#undef main

int main(int argc, char** argv) {
	int result = -1;


	try {
		if (!OVR_SUCCESS(ovr_Initialize(nullptr))) {
			FAIL("Failed to initialize the Oculus SDK");
		}
		//AvatarHand* hands = new AvatarHand();
		//hands->initialization();
		//hands->renderHands();
		result = ExampleApp().run();
	}
	catch (std::exception & error) {
		OutputDebugStringA(error.what());
		std::cerr << error.what() << std::endl;
	}
	ovr_Shutdown();
	return result;
}