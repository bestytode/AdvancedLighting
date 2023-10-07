// This is really the most complex rendering technique I've tackled so far.
// 
// 1. Note that the depth values being compared are in view space.
// 
// 2. Various techniques are employed to improve quality and reduce sample size, 
//    such as scaling samples in the kernel, tiling a 4x4 noise texture, applying a blur process, 
//    and using the smoothstep function for range checking.
// 
// 3. The sample kernel is distributed in a hemisphere and is directly used in the GLSL shader.
// 
// 4. The noise texture is aligned with the z-axis, consistent with the sample kernel, 
//    and is combined with a TBN matrix for orientation.
// 
// 5. The sample[i] is used to sample from the off-screen rendered texture after converting 
//    from view space to the [0, 1] range.
// 
// 6. Only a single float value (GL_RED channel) is stored as the output color buffer texture 
//    for both the SSAO and blur shaders, as we only need an occlusion coefficient (i.e., out float FragColor).
// 
// 2023-10-7
// Author: Yu

#include <iostream>
#include <stdexcept>
#include <random>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "shader.h"
#include "geometry_renderers.h"
#include "model.h"
#include "timer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Function declarations (callback functions)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow* window);
unsigned int LoadTexture(const std::string& path);

// Scene settings
int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

// Camera settings
Camera camera(0.0f, 0.0f, 5.0f);
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float plane_near = 0.1f;
float plane_far = 100.0f;
bool mouseButtonPressed = true; // only move the camera when mouse is being pressed
bool enableCameraMovement = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// SSAO settings
bool enableSSAO = true;

int main()
{
	Timer timer;
	timer.start();

	// glfw & glew configs
	// -------------------
	GLFWwindow* window = nullptr;
	try {
		if (!glfwInit())
			throw std::runtime_error("failed to init glfw");
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "hnzz", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("failed to create window");

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		if (glewInit() != GLEW_OK)
			throw std::runtime_error("failed to init glew");

		// OpenGL global status settings here
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	// ImGui Initialization
	// --------------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 2.0f;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	// build and compile shader(s)
	// ---------------------------
	Shader shaderGeometryPass("res/shaders/ssao_geometry.vs", "res/shaders/ssao_geometry.fs");
	Shader shaderSSAO("res/shaders/ssao.vs", "res/shaders/ssao.fs");
	Shader shaderSSAOBlur("res/shaders/ssao.vs", "res/shaders/ssao_blur.fs");
	Shader shaderLightingPass("res/shaders/ssao.vs", "res/shaders/ssao_lighting.fs");
	Shader shaderLightSource("res/shaders/deferred_light_box.vs", "res/shaders/deferred_light_box.fs");

	// load model(s)
	// -------------
	//Model nanosuit("res/models/nanosuit/nanosuit.obj");
	Model backpack("res/models/backpack/backpack.obj");

	// Set VAO for geometry shape for later use
	yzh::Quad quad;
	yzh::Cube cube;
	yzh::Sphere sphere;

	// Set up G-Buffer with 3 textures:
	// 
    // 1. Position
    // 2. Color
    // 3. Normal 
	// ---------------
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	unsigned int gPosition, gNormal, gAlbedo;
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindBuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "GBuffer Framebuffer not complete!" << std::endl;

	// 2. Create SSAO texture
    // ----------------------
	unsigned int ssaoFBO, ssaoColorBuffer;
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Framebuffer not complete!" << std::endl;

	// 3. Blur SSAO texture to remove noise
	// ------------------------------------
	unsigned int ssaoBlurFBO, ssaoColorBufferBlur;
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	glGenTextures(1, &ssaoColorBufferBlur);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 4. Sample kernel (in tangent space)
	// -----------------------------------
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1, 1);

	std::vector<glm::vec3>sampleKernel;
	unsigned int sampleSize = 16;
	float radius = 0.05f;
	for (size_t i = 0; i < sampleSize; i++) {
		glm::vec3 sample(dis(gen), dis(gen), (dis(gen) * 0.5f + 0.5f)); // Generate random sample point in a hemisphere oriented along the z-axis
		sample = glm::normalize(sample);
		sample *= dis(gen); // Provide random length of sample vector

		// Scale samples so that they are more aligned to the center of the kernel
		float scale = float(i) / (float)sampleSize;
		scale = glm::mix(0.1f, 1.0f, scale * scale);
		sample *= scale;
		sampleKernel.emplace_back(sample);
	}

	// 5. Noise texture (in tangent space)
	unsigned int noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	std::vector<glm::vec3>ssaoNoise;
	for (size_t i = 0; i < 16; i++) {
        // Set z-coordinate to 0.0 to ensure the points will still be distributed in a hemisphere oriented along z-axis,
		glm::vec3 noise(dis(gen), dis(gen), 0.0f); 
		ssaoNoise.emplace_back(noise);
	}
	// Notice we set both width and height to 4, which is the parameter that related to the size of 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Use GL_REPEAT to fill the quad as the noise texture is relatively small
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

	// lighting info
	// -------------
	struct Light {
		glm::vec3 position = glm::vec3(2.0, 4.0, -2.0);
	    glm::vec3 color = glm::vec3(0.2, 0.2, 0.7);
		const float linear = 0.09f;
		const float quadratic = 0.032f;
	};
	Light light;

	// shader configs
	// --------------
	shaderLightingPass.Bind();
	shaderLightingPass.SetInt("gPosition", 0);
	shaderLightingPass.SetInt("gNormal", 1);
	shaderLightingPass.SetInt("gAlbedo", 2);
	shaderLightingPass.SetInt("ssao", 3);
	shaderSSAO.Bind();
	shaderSSAO.SetInt("gPosition", 0);
	shaderSSAO.SetInt("gNormal", 1);
	shaderSSAO.SetInt("noiseTexture", 2);
	shaderSSAOBlur.Bind();
	shaderSSAOBlur.SetInt("ssaoInput", 0);

	timer.stop();

	// Imgui settings
	// --------------
	bool firstTime = true;
	double cursor_x, cursor_y;
	unsigned char pixel[4];
	while (!glfwWindowShouldClose(window)){
		// Per-frame logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Process input
		ProcessInput(window);

		// 1. Geometry Pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderGeometryPass.Bind();

		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, plane_near, plane_far);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
		model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
		shaderGeometryPass.SetMat4("projection", projection);
		shaderGeometryPass.SetMat4("view", view);
		shaderGeometryPass.SetMat4("model", model);
		shaderGeometryPass.SetInt("invertedNormals", 1); // invert normals as we're inside the cube
		cube.Render();
		shaderGeometryPass.SetInt("invertedNormals", 0);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(1.0f));
		shaderGeometryPass.SetMat4("model", model);
		//nanosuit.Render(shaderGeometryPass);
		backpack.Render(shaderGeometryPass);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. Perform SSAO Calculation
		// ---------------------------
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderSSAO.Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		for (size_t i = 0; i < sampleSize; i++) {
			shaderSSAO.SetVec3("samples[" + std::to_string(i) + "]", sampleKernel[i]);
		}
		shaderSSAO.SetMat4("projection", projection);
		shaderSSAO.SetFloat("kernelSize", (float)sampleSize);
		shaderSSAO.SetFloat("radius", radius);
		quad.Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. Blur ssao texture to remove noise
		// ------------------------------------
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderSSAOBlur.Bind();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
		quad.Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 4. Lighting Pass: traditional deferred Blinn-Phong lighting now with added screen-space ambient occlusion
		// ---------------------------------------------------------------------------------------------------------
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderLightingPass.Bind();
		
		// Notice we multiply view matrix here instead of in glsl code
		glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(light.position, 1.0));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        
		// Send lighting uniforms
		shaderLightingPass.SetVec3("light.Position", lightPosView);
		shaderLightingPass.SetVec3("light.Color", light.color);
		shaderLightingPass.SetFloat("light.Linear", light.linear);
		shaderLightingPass.SetFloat("light.Quadratic", light.quadratic);

		shaderLightingPass.SetInt("enableSSAO", enableSSAO);
		quad.Render();

		// 5. Render light source
		// ----------------------
		glEnable(GL_DEPTH_TEST);
		shaderLightSource.Bind();

		model = glm::mat4(1.0f);
		model = glm::translate(model, light.position);
		model = glm::scale(model, glm::vec3(0.05f));

		shaderLightSource.SetMat4("projection", projection);
		shaderLightSource.SetMat4("view", view);
		shaderLightSource.SetMat4("model", model);
		shaderLightSource.SetVec3("lightColor", light.color);
		sphere.Render();

		// ImGui code here
        // ---------------
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (firstTime) {
			ImGui::SetNextWindowSize(ImVec2(500, 250));
			ImGui::SetNextWindowPos(ImVec2(50, 50));
			//firstTime = false;
		}
		ImGui::Begin("hnzz");
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

		// Retrieve and display the cursor position and the RGBA color of the pixel under the cursor
		glfwGetCursorPos(window, &cursor_x, &cursor_y);
		glReadPixels(cursor_x, SCR_HEIGHT - cursor_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
		ImGui::Text("Cursor position: (%.2f, %.2f)", cursor_x, cursor_y);
		ImGui::Text("RGBA: (%d, %d, %d, %d)", pixel[0], pixel[1], pixel[2], pixel[3]);
		ImGui::End();

		if (firstTime) {
			ImGui::SetNextWindowSize(ImVec2(500, 250));
			ImGui::SetNextWindowPos(ImVec2(50, 350));
			firstTime = false;
		}
		ImGui::Begin("ssao");
		ImGui::Text("Sample Kernel: %u", sampleSize);
		ImGui::SliderFloat("Radius", &radius, 0.0f, 1.0f);
		ImGui::Checkbox("Enable camera movement", &enableCameraMovement);
		ImGui::Checkbox("Enable SSAO", &enableSSAO);
		ImGui::End();
		// ImGui Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Optional: release all the resources of OpenGL (VAO, VBO, etc.)
	glfwTerminate();

	// ImGui Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that SCR_WIDTH and 
	// SCR_HEIGHT will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (!enableCameraMovement)
		return;

	// Check if the left mouse button is pressed
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (mouseButtonPressed) {
			lastX = xpos;
			lastY = ypos;
			mouseButtonPressed = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
		mouseButtonPressed = true;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// Utility function for loading a 2D texture from file
unsigned int LoadTexture(const std::string& path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format = GL_RGBA;
		if (nrComponents == 1) format = GL_RED;
		else if (nrComponents == 3) format = GL_RGB;
		else if (nrComponents == 4) format = GL_RGBA;
		else {
			std::cerr << "Invalid texture format: Unsupported number of components!\n";
			return -1;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else {
		std::cerr << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		return -1;
	}

	return textureID;
}