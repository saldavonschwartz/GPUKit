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

#pragma once

#include "../GPUKit/Material/Material.h"
#include "../GPUKit/Geometry/Geometry.h"
#include  "../GPUKit/Texture/2D/Texture2D.h"
#include  "../GPUKit/Texture/2D/Texture2DImporter.h"
#include "../GPUKit/Program/Program.h"
#include "../GPUKit/Program/ProgramImporter.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Camera {
	glm::mat4 P;
};

struct Light {
	glm::vec3 color{};
	float intensity;
	float attenuation;
	bool castsShadows;
};

struct Node {
	std::string name{"Default"};
	glm::mat4 M{};
	std::vector<Node*>children;
	Camera* camera{ nullptr };
	Light* light{ nullptr };
	OXFEDE::GPUKit::Geometry* geometry{ nullptr };
	OXFEDE::GPUKit::Material* material{ nullptr };

	~Node() {
		// This is a temp measure until deciding on a 
		// memory ownership / management policy.
		if (light) {
			delete light;
		}

		if (material) {
			using OXFEDE::GPUKit::Program;
			using OXFEDE::GPUKit::Texture2D;
			using OXFEDE::GPUKit::AssetImporter;

			Program* program = material->getProgram();
			AssetImporter<Program*>::release(program);

			Texture2D* tx = material->getAttribute<Texture2D*>("diffuse.map");
			if (tx) {
				AssetImporter<Texture2D*>::release(tx);
			}

			tx = material->getAttribute<Texture2D*>("specular.map");
			if (tx) {
				AssetImporter<Texture2D*>::release(tx);
			}
			
			delete material;
		}

		if (geometry) {
			delete geometry;
		}

		for (Node* child : children) {
			delete child;
		}
	}
};
