#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include "shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

struct Texture
{
	std::string type;
	unsigned int id;
	std::string path;
};

class Mesh
{
public:
	Mesh() = delete;
	Mesh(const std::vector<Vertex>& _vertices,
		const std::vector<unsigned int>& _indices,
		const std::vector<Texture>& _textures);
	~Mesh() = default;
	void Draw(Shader& shader) const;

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

private:
	unsigned int VAO, VBO, IBO;

	void SetupMesh();
};

Mesh::Mesh(const std::vector<Vertex>& _vertices, 
	const std::vector<unsigned int>& _indices, 
	const std::vector<Texture>& _textures)
{
	this->vertices = _vertices;
	this->indices = _indices;
	this->textures = _textures;

	SetupMesh();
}

// first calculate the N-component per texture type and concatenate it to the texture's type string to get the uniform name. 
// then locate the appropriate sampler, give it the location value to correspond with the currently active texture unit, and bind the texture. 
void Mesh::Draw(Shader& shader) const
{
	// Start from material.diffuse1 or material.specular1
	size_t diffuseNr = 1, specularNr = 1;

	for (size_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); 
		// Get texture number£¨N in diffuse_textureN £©
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);

		shader.SetInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	//glActiveTexture(GL_TEXTURE0);

	// Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::SetupMesh()
{
	// VAO, VBO, and IBO(EBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	// TexCoords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	// Unbind VAO
	glBindVertexArray(0);
}