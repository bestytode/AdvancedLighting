#pragma once

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif 

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>

#include "mesh.h"
#include "shader.h"

unsigned int TextureFromFile(const char* path, const std::string& directory);

class Model
{
public:
	Model() = delete;

	Model(const std::string& _filePath) {
		LoadModel(_filePath);
	}

	void Draw(Shader& _shader) 
	{
		for (size_t i = 0; i < meshes.size(); i++)
			meshes[i].Draw(_shader);
	}

private:
	void LoadModel(const std::string& _filePath);

	void ProcessNode(aiNode* node, const aiScene* scene);

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
		std::string typeName);

private:
	std::vector<Mesh>meshes;
	std::vector<Texture>textures_loaded;
	std::string directory;
};

// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::LoadModel(const std::string& _filePath)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(_filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

#ifdef _DEBUG
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
#endif 

	directory = _filePath.substr(0, _filePath.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);
}

// Iterate through all Node, from scene->mRootNode
inline void Model::ProcessNode(aiNode* currentNode, const aiScene* scene)
{
	for (size_t i = 0; i < currentNode->mNumMeshes; i++) {
		// mMeshes in node store the index,
		// where mMeshes in scene hold the actual objects
		aiMesh* mesh = scene->mMeshes[currentNode->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (size_t i = 0; i < currentNode->mNumChildren; i++) {
		ProcessNode(currentNode->mChildren[i], scene);
	}
}

// Retriving information from aiMesh and aiScene, converting all to our own Mesh
inline Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex>vertices;
	std::vector<unsigned int>indices;
	std::vector<Texture>textures;

	// vertices
	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		// we declare a placeholder vector since assimp uses its own vector class 
		// that doesn't directly convert to glm's vec3 class.
		glm::vec3 vector(1.0f);
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
			vertex.texCoords = glm::vec2(0.0f);

		vertices.push_back(vertex);
	}

	// indices
	// 
	// Each mesh has an array of faces.
	// Each face represents a single primitive(triangle here)
	// A face contains the indices of the vertices.
	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// textures
	// 
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// 1. diffuse maps
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// 2. specular maps
		std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// 3. normal maps
		std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// 4. height maps
		std::vector<Texture> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}
	return Mesh(vertices, indices, textures);
}

// Return a vector contains Texture, retriving texture information from aiMaterial to our own textures and textures_loaded
inline std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;

	// Iterate all textures, i stands for the number of texture counts of current texture type
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {

		// Get the (i)th texture of current type
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (size_t j = 0; j < textures_loaded.size(); j++) {
			// Iterate through textures_loaded vector, to check if it already exisits.
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				// Use the located texture directly since we've already loaded
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		// If the texture has not been loaded yet, add it
		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();

			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

// Load a texture and return the actual id.
unsigned int TextureFromFile(const char* path, const std::string& directory)
{
	// Directory + filepath 
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) format = GL_RED;
		if (nrComponents == 3) format = GL_RGB;
		if (nrComponents == 4) format = GL_RGBA;

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
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}