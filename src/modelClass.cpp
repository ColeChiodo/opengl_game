#include "modelClass.h"

Model::Model(const char* fileName) {
    std::string text = get_file_contents(fileName);
	JSON = json::parse(text);
    
    Model::file = fileName;
	data = getData();

    traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        glm::mat4 finalTransform = modelMatrix * matricesMeshes[i];
        meshes[i].Draw(shader, camera, finalTransform);
    }
}

void Model::UpdateMatrix() {
    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::scale(modelMatrix, scale);
}

void Model::loadMesh(unsigned int meshIndices) {
	unsigned int posAccessorIndices = JSON["meshes"][meshIndices]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccessorIndices = JSON["meshes"][meshIndices]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int textureAccessorIndices = JSON["meshes"][meshIndices]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccessorIndices = JSON["meshes"][meshIndices]["primitives"][0]["indices"];

	std::vector<float> posVertices = getFloats(JSON["accessors"][posAccessorIndices]);
	std::vector<glm::vec3> positions = groupFloatsVec3(posVertices);
	std::vector<float> normalVertices = getFloats(JSON["accessors"][normalAccessorIndices]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVertices);
	std::vector<float> textureVertices = getFloats(JSON["accessors"][textureAccessorIndices]);
	std::vector<glm::vec2> textureUVs = groupFloatsVec2(textureVertices);

	std::vector<Vertex> vertices = assembleVertices(positions, normals, textureUVs);
	std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccessorIndices]);
	std::vector<Texture> textures = getTextures();

	meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix) {
	json node = JSON["nodes"][nextNode];

	glm::vec3 foundTranslation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end()) {
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++) {
			transValues[i] = (node["translation"][i]);
        }
		foundTranslation = glm::make_vec3(transValues);
	}

	glm::quat foundRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end()) {
		float rotValues[4] = { // GLTF stores xyzw. glm uses wxyz
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		foundRotation = glm::make_quat(rotValues);
	}

	glm::vec3 foundScale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end()) {
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++) {
			scaleValues[i] = (node["scale"][i]);
        }
		foundScale = glm::make_vec3(scaleValues);
	}

	glm::mat4 matrixNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end()) {
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++) {
			matValues[i] = (node["matrix"][i]);
        }
		matrixNode = glm::make_mat4(matValues);
	}

	glm::mat4 translation = glm::mat4(1.0f);
	glm::mat4 rotation = glm::mat4(1.0f);
	glm::mat4 scale = glm::mat4(1.0f);

	translation = glm::translate(translation, foundTranslation);
	rotation = glm::mat4_cast(foundRotation);
	scale = glm::scale(scale, foundScale);

	glm::mat4 matNextNode = matrix * matrixNode * translation * rotation * scale;

	if (node.find("mesh") != node.end()) {
		translationsMeshes.push_back(foundTranslation);
		rotationsMeshes.push_back(foundRotation);
		scalesMeshes.push_back(foundScale);
		matricesMeshes.push_back(matNextNode);

		loadMesh(node["mesh"]);
	}

    // Recursion
	if (node.find("children") != node.end()) {
		for (unsigned int i = 0; i < node["children"].size(); i++) {
			traverseNode(node["children"][i], matNextNode);
        }
	}
}

std::vector<unsigned char> Model::getData() {
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	bytesText = get_file_contents((fileDirectory + uri).c_str());

	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor) {
	std::vector<float> floatVector;

	unsigned int bufferViewIndex = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	json bufferView = JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView["byteOffset"];

	unsigned int floatsPerVertex;
	if (type == "SCALAR") floatsPerVertex = 1;
	else if (type == "VEC2") floatsPerVertex = 2;
	else if (type == "VEC3") floatsPerVertex = 3;
	else if (type == "VEC4") floatsPerVertex = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	unsigned int lengthOfData = count * 4 * floatsPerVertex;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i += 4) {
		unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVector.push_back(value);
	}

	return floatVector;
}

std::vector<GLuint> Model::getIndices(json accessor) {
	std::vector<GLuint> indices;

	unsigned int bufferViewIndex = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accessorByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	json bufferView = JSON["bufferViews"][bufferViewIndex];
	unsigned int byteOffset = bufferView.value("byteOffset", 0);

	unsigned int beginningOfData = byteOffset + accessorByteOffset;
	if (componentType == 5125) { // unsigned int
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 4; i += 4) {
			unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
    } else if (componentType == 5123) { // unsigned short
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i += 2) {
			unsigned char bytes[] = { data[i], data[i + 1] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	} else if (componentType == 5122) { // short
		for (unsigned int i = beginningOfData; i < byteOffset + accessorByteOffset + count * 2; i += 2) {
			unsigned char bytes[] = { data[i], data[i + 1] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}

	return indices;
}

std::vector<Texture> Model::getTextures() {
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	for (unsigned int i = 0; i < JSON["images"].size(); i++) {
		std::string texturePath = JSON["images"][i]["uri"];

		bool textureAlreadyLoaded = false;
		for (unsigned int j = 0; j < loadedTextureName.size(); j++) {
			if (loadedTextureName[j] == texturePath) {
				textures.push_back(loadedTexture[j]);
				textureAlreadyLoaded = true;
				break;
			}
		}

		if (!textureAlreadyLoaded) {
            // Not Ideal
			if (texturePath.find("baseColor") != std::string::npos) { // Load diffuse
				Texture diffuse = Texture((fileDirectory + texturePath).c_str(), "diffuse", loadedTexture.size());
				textures.push_back(diffuse);
				loadedTexture.push_back(diffuse);
				loadedTextureName.push_back(texturePath);
			} else if (texturePath.find("metallicRoughness") != std::string::npos) { // Load specular
				Texture specular = Texture((fileDirectory + texturePath).c_str(), "specular", loadedTexture.size());
				textures.push_back(specular);
				loadedTexture.push_back(specular);
				loadedTextureName.push_back(texturePath);
			}
		}
	}

	return textures;
}

std::vector<Vertex> Model::assembleVertices(std::vector<glm::vec3> positions,std::vector<glm::vec3> normals,std::vector<glm::vec2> textureUVs) {
	std::vector<Vertex> vertices;

	for (int i = 0; i < positions.size(); i++) {
		vertices.push_back ( Vertex {
            positions[i],
            normals[i],
            glm::vec3(1.0f, 1.0f, 1.0f),
            textureUVs[i]
        });
	}

	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVector) {
	const unsigned int floatsPerVector = 2;
	
	std::vector<glm::vec2> vectors;

	for (unsigned int i = 0; i < floatVector.size(); i += floatsPerVector) {
		vectors.push_back(glm::vec2(0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++) {
			vectors.back()[j] = floatVector[i + j];
		}
	}

	return vectors;
}
std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVector) {
	const unsigned int floatsPerVector = 3;

	std::vector<glm::vec3> vectors;

	for (unsigned int i = 0; i < floatVector.size(); i += floatsPerVector) {
		vectors.push_back(glm::vec3(0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++) {
			vectors.back()[j] = floatVector[i + j];
		}
	}

	return vectors;
}
std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVector) {
	const unsigned int floatsPerVector = 4;

	std::vector<glm::vec4> vectors;

	for (unsigned int i = 0; i < floatVector.size(); i += floatsPerVector) {
		vectors.push_back(glm::vec4(0, 0, 0, 0));

		for (unsigned int j = 0; j < floatsPerVector; j++) {
			vectors.back()[j] = floatVector[i + j];
		}
	}
	return vectors;
}

