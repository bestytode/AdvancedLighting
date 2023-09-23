// This header file provides a collection of functions for easily rendering specific geometric shapes in OpenGL.
// Note: Each function includes vertex attributes for position, normal, and texture coordinates.
// 
// Future updates may extend these functions to include tangents and bitangents, intergrated to a CLASS,
// 
//
// Author: Zhenhuan Yu
// Date: 2023/09/17

#pragma once

#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace yzh {

	// This class provides a cube using OpenGL with dimensions of 2 * 2 * 2 units
	// The cube's vertex attributes include position, normal, and texture coordinates.
	// Note: This function does not use an Index Buffer Object (IBO).
	class Cube
	{
	public:
		Cube()
		{
			if (this->VAO == 0) {
				float vertices[] = {
					// Position           // Normal           // TexCoords
					-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 
					 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
					 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,      
					 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
					-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 
					-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 

					-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 
					 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 
					 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 
					 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, 
					-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, 
					-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 

					-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
					-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, 
					-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
					-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
					-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, 
					-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 

					 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
					 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
					 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,     
					 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, 
					 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 
					 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,   

					 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 
					  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 
					  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
					  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
					 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 
					 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 

					 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 
					  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
					  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,   
					  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
					 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 
					 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f      
				};

				glGenVertexArrays(1, &this->VAO);
				glGenBuffers(1, &this->VBO);
				// fill buffer
				glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				// link vertex attributes
				glBindVertexArray(this->VAO);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}
		}

		Cube(const Cube& others) = delete;
		Cube& operator=(const Cube& others) = delete;

		~Cube()
		{
			if (this->VAO != 0) {
				glDeleteVertexArrays(1, &this->VAO);
				glDeleteBuffers(1, &this->VBO);
				this->VAO = 0;
			}
		}

		void Render()
		{
			if (this->VAO != 0) {
				glBindVertexArray(this->VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				glBindVertexArray(0);
			}
		}

	private:
		unsigned int VAO = 0, VBO;
	};

	// This class provides a sphere in OpenGL with a radius of 2.0 units.
	// Vertex attributes include position, normal, and texture coordinates.
	// The sphere is detailed with 64 segments along both the X and Y axes, resulting in a highly detailed mesh.
	// he function allows for customization of the sphere's detail level through x_segments and y_segments.
	class Sphere
	{
	public:
		Sphere(const unsigned int x_segments = 64,
			const unsigned int y_segments = 64)
		{
			// Ensure the process will only be excuted once
			if (this->VAO == 0) {
				glGenVertexArrays(1, &this->VAO);
				glGenBuffers(1, &this->VBO);
				glGenBuffers(1, &this->IBO);

				std::vector<float> vertices;
				std::vector<unsigned int> indices;

				const unsigned int X_SEGMENTS = x_segments;
				const unsigned int Y_SEGMENTS = y_segments;
				const float PI = 3.14159265359;
				float radius = 2.0f;
				for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
						float xSegment = (float)x / (float)X_SEGMENTS;
						float ySegment = (float)y / (float)Y_SEGMENTS;

						float xPos = radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
						float yPos = radius * std::cos(ySegment * PI);
						float zPos = radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

						// Normalizing the normal
						float norm = std::sqrt(xPos * xPos + yPos * yPos + zPos * zPos);

						vertices.push_back(xPos); // Position
						vertices.push_back(yPos);
						vertices.push_back(zPos);
						vertices.push_back(xPos / norm); // Normal
						vertices.push_back(yPos / norm);
						vertices.push_back(zPos / norm);
						vertices.push_back(xSegment); // UV coords
						vertices.push_back(ySegment);
					}
				}

				bool oddRow = false;
				for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
					if (!oddRow) {
						// even rows: y == 0, y == 2; and so on 
						for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
							indices.push_back(y * (X_SEGMENTS + 1) + x);
							indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						}
					}
					else {
						for (int x = X_SEGMENTS; x >= 0; --x) {
							indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
							indices.push_back(y * (X_SEGMENTS + 1) + x);
						}
					}
					oddRow = !oddRow;
				}

				glBindVertexArray(this->VAO);
				glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				glBindVertexArray(0);
			}
		}

		~Sphere()
		{
			// Prevent multiple de-allocation
			if (this->VAO != 0) {
				glDeleteVertexArrays(1, &this->VAO);
				glDeleteBuffers(1, &this->VBO);
				glDeleteBuffers(1, &this->IBO);
				this->VAO = 0;
			}
		}

		Sphere(const Sphere& other) = delete;
		Sphere& operator=(const Sphere& other) = delete;

		void Render()
		{
			if (this->VAO != 0) {
				glBindVertexArray(this->VAO);
				glDrawElements(GL_TRIANGLE_STRIP, (64 + 1) * 64 * 2, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}

		const unsigned int GetVAO() const { return VAO; }

	private:
		unsigned int VAO = 0, VBO, IBO;
	};

	// This class provides a 2D quad in OpenGL with dimensions of 2 * 2 units.
    // Vertex attributes include position, normal, and texture coordinates.
    // The quad is rendered using GL_TRIANGLE_STRIP for optimized rendering.
	class Quad
	{
	public:
		Quad()
		{
			if (this->VAO == 0) {
				// positions, normals, texture Coords
				float quadVertices[] = {
					-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
				};

				glGenVertexArrays(1, &this->VAO);
				glGenBuffers(1, &this->VBO);
				glBindVertexArray(this->VAO);
				glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

				// Position attribute
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

				// Normal attribute
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

				// Texture coordinate attribute
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

				glBindVertexArray(0);
			}
		}

		~Quad()
		{
			if (VAO != 0) {
				glDeleteVertexArrays(1, &this->VAO);
				glDeleteBuffers(1, &this->VBO);
				this->VAO = 0;
			}
		}

		void Render()
		{
			if (VAO != 0) {
				glBindVertexArray(this->VAO);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				glBindVertexArray(0);
			}
		}
	    
		const unsigned int GetVAO() const { return this->VAO; }

	private:
		unsigned int VAO = 0, VBO;
	};
};

namespace yzh {
	// Function declarations:
	// ----------------------

    // This function renders a cube using OpenGL with dimensions of 2 * 2 * 2 units
	// The cube's vertex attributes include position, normal, and texture coordinates.
    // Note: This function does not use an Index Buffer Object (IBO).
	void RenderCube();

    // This function renders a sphere in OpenGL with a radius of 2.0 units.
    // Vertex attributes include position, normal, and texture coordinates.
    // The sphere is detailed with 64 segments along both the X and Y axes, resulting in a highly detailed mesh.
	// he function allows for customization of the sphere's detail level through x_segments and y_segments.
	void RenderSphere(const unsigned int x_segments = 64, const unsigned int y_segments = 64);

    // This function renders a 2D quad in OpenGL with dimensions of 2 * 2 units.
    // Vertex attributes include position, normal, and texture coordinates.
    // The quad is rendered using GL_TRIANGLE_STRIP for optimized rendering.
	void RenderQuad();

	// Function implementations:
	// -------------------------
	void RenderCube()
	{
		static unsigned int cubeVAO = 0;
		static unsigned int cubeVBO;

		// Preventing redefinition
		if (cubeVAO == 0) {
			float vertices[] = {
				// Position           // Normal           // TexCoords
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				 // bottom face
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 // top face
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};

			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void RenderSphere(const unsigned int x_segments, const unsigned int y_segments)
	{
		static unsigned int sphereVAO = 0;
		static unsigned int sphereVBO, sphereEBO;

		// Preventing redefinition
		if (sphereVAO == 0) {
			glGenVertexArrays(1, &sphereVAO);
			glGenBuffers(1, &sphereVBO);
			glGenBuffers(1, &sphereEBO);

			std::vector<float> vertices;
			std::vector<unsigned int> indices;

			const unsigned int X_SEGMENTS = x_segments;
			const unsigned int Y_SEGMENTS = y_segments;
			const float PI = 3.14159265359;
			float radius = 2.0f;
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
					float xSegment = (float)x / (float)X_SEGMENTS;
					float ySegment = (float)y / (float)Y_SEGMENTS;

					float xPos = radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
					float yPos = radius * std::cos(ySegment * PI);
					float zPos = radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

					// Normalizing the normal
					float norm = std::sqrt(xPos * xPos + yPos * yPos + zPos * zPos);

					vertices.push_back(xPos); // Position
					vertices.push_back(yPos);
					vertices.push_back(zPos);
					vertices.push_back(xPos / norm); // Normal
					vertices.push_back(yPos / norm);
					vertices.push_back(zPos / norm);
					vertices.push_back(xSegment); // UV coords
					vertices.push_back(ySegment);
				}
			}

			bool oddRow = false;
			for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
				if (!oddRow) {
					// even rows: y == 0, y == 2; and so on 
					for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
						indices.push_back(y * (X_SEGMENTS + 1) + x);
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					}
				}
				else {
					for (int x = X_SEGMENTS; x >= 0; --x) {
						indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
						indices.push_back(y * (X_SEGMENTS + 1) + x);
					}
				}
				oddRow = !oddRow;
			}

			glBindVertexArray(sphereVAO);
			glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindVertexArray(0);
		}

		glBindVertexArray(sphereVAO);
		glDrawElements(GL_TRIANGLE_STRIP, (64 + 1) * 64 * 2, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void RenderQuad()
	{
		static unsigned int quadVAO = 0;
		static unsigned int quadVBO;
		if (quadVAO == 0) {
			// positions, normals, texture Coords
			float quadVertices[] = {
				-1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
			};

			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

			// Position attribute
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

			// Normal attribute
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

			// Texture coordinate attribute
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

			glBindVertexArray(0);
		}

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
};