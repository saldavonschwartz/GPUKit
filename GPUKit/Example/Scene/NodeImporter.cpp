//    The MIT License (MIT) 
// 
//    Copyright (c) 2016 Federico Saldarini 
//    https://www.linkedin.com/in/federicosaldarini 
//    https://github.com/saldavonschwartz 
//    http://0xfede.io 
// 
// 
//    Permission is hereby granted, free of charge, to any person obtaining a copy 
//    of this software and associated documentation files (the "Software"), to deal 
//    in the Software without restriction, including without limitation the rights 
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
//    copies of the Software, and to permit persons to whom the Software is 
//    furnished to do so, subject to the following conditions: 
// 
//    The above copyright notice and this permission notice shall be included in all 
//    copies or substantial portions of the Software. 
// 
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
//    SOFTWARE. 

#include "NodeImporter.h"

#include "../GPUKit/Material/Material.h"
#include "../GPUKit/Program/Program.h"
#include "../GPUKit/Program/ProgramImporter.h"
#include "../GPUKit/Texture/2D/Texture2D.h"
#include "../GPUKit/Texture/2D/Texture2DImporter.h"
#include "../GPUKit/Geometry/Geometry.h"
#include "../GPUKit/GLWrapper.h"

#include "../Example/Scene/HighLevelTypes.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace OXFEDE::GPUKit;
using namespace Assimp;
using namespace std;
using namespace glm;

Node* newNode(const aiNode* aNode, const aiScene* aScene, const string& path) {
	Node* node = new Node;
	node->name = aNode->mName.C_Str();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			node->M[i][j] = aNode->mTransformation[j][i];
		}
	}

	if (aNode->mNumMeshes) {
		const aiMesh* aMesh = aScene->mMeshes[aNode->mMeshes[0]];	
		Geometry::Data data;

		for (uint32_t i = 0; i < aMesh->mNumVertices; i++) {
			auto position = aMesh->mVertices[i];
			auto normal = aMesh->mNormals[i];

			vec2 tx0;
			if (aMesh->mTextureCoords[0]) {
				tx0.x = aMesh->mTextureCoords[0][i].x;
				tx0.y = aMesh->mTextureCoords[0][i].y;
			}

			data.vertices.push_back({
				{ position.x, position.y, position.z },
				{ normal.x, normal.y, normal.z },
				tx0
			});
		}

		for (uint32_t i = 0; i < aMesh->mNumFaces; i++) {
			auto face = aMesh->mFaces[i];
			data.indices.push_back(face.mIndices[0]);
			data.indices.push_back(face.mIndices[1]);
			data.indices.push_back(face.mIndices[2]);
		}

		node->geometry = new Geometry(data);
		node->geometry->commit();

		Program* program = AssetImporter<Program*>::import("Example/Shaders/Geometry.glsl");
		program->link();
		
		Material* material = new Material(program);
		
		Texture2D* txDiffuse{};
		Texture2D* txSpecular{};
		string materialName = "Default Instance";

		if (aMesh->mMaterialIndex) {
			const aiMaterial* aMaterial = aScene->mMaterials[aMesh->mMaterialIndex];

			Texture2D::Data data;
			data.filter.magnification = GL_LINEAR_MIPMAP_LINEAR;
			data.filter.minification = GL_LINEAR_MIPMAP_LINEAR;
			data.wrapMode.S = GL_REPEAT;
			data.wrapMode.T = GL_REPEAT;
			data.mipmaps = true;
			
			aiString txFileName;
			
			aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &txFileName);
			if (txFileName.length) {
				string txPath = path.substr(0, path.find_last_of("\\/") + 1) + txFileName.C_Str();
				txDiffuse = AssetImporter<Texture2D*>::import(txPath, data, true);
				txDiffuse->commit();
			}

			aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &txFileName);
			if (txFileName.length) {
				string txPath = path.substr(0, path.find_last_of("\\/") + 1) + txFileName.C_Str();
				txSpecular = AssetImporter<Texture2D*>::import(txPath, data, true);
				txDiffuse->commit();
			}

			aiString aMaterialName;
			aMaterial->Get(AI_MATKEY_NAME, aMaterialName);
			materialName = aMaterialName.C_Str();
		}

		material->name = materialName;
		material->setAttribute("diffuse.color", vec3{ 1. });
		material->setAttribute("diffuse.map", txDiffuse);
		material->setAttribute("diffuse.usesMap", txDiffuse != nullptr);
		material->setAttribute("specular.value", txSpecular ? 10.f : 1.f);
		material->setAttribute("specular.map", txSpecular);
		material->setAttribute("specular.usesMap", txSpecular != nullptr);
		material->setAttribute("emission.usesMap", false);
		
		node->material = material;
	}

	for (uint32_t i = 0; i < aNode->mNumChildren; i++) {
		const aiNode* child = aNode->mChildren[i];
		node->children.push_back(newNode(child, aScene, path));
	}

	return node;
}

Node* AssetImporter<Node*>::import(const string& path) {
	Importer assimp;
	uint32_t flags =
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals;

	const aiScene* aScene = assimp.ReadFile(path, flags);

	if (!aScene || aScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !aScene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << assimp.GetErrorString() << "\n";
		return nullptr;
	}

	const aiNode* aNode = aScene->mRootNode;
	Node* node = newNode(aNode, aScene, path);
	return node;
}

