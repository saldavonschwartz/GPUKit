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

#include "../GPUKit/GPUKit.h"
#include "../Example/Scene/HighLevelTypes.h"

#include <glm/ext.hpp>
#include <string>

using namespace OXFEDE::GPUKit;
using glm::uvec2;
using glm::vec3;
using glm::mat4;
using glm::column;
using std::string;

struct ShadowPass : public Pass {
	Node* lightsNode;
	Node* scene;

	ShadowPass() {
		program = AssetImporter<Program*>::import("Example/Shaders/Shadow.glsl");
		program->link();

		buffer = new Buffer;
	}

	~ShadowPass() {
		delete program;
		delete buffer;
	}

	void render() override {
		glEnable(GL_DEPTH_TEST);
		buffer->setAttachmentWriteEnabled(Buffer::COLOR0, false);
		buffer->setClearTargets(Buffer::Clear::DEPTH);
		buffer->commit();
		buffer->validate();

		uvec2 size = ((Texture3D*)getOutput("depth"))->data.image[0].size;
		glViewport(0, 0, size.x, size.y);

		for (Node* lightNode : lightsNode->children) {
			if (!lightNode->light->castsShadows) {
				continue;
			}

			vec3 position = column(lightNode->M, 3);
			mat4 P = glm::perspective(glm::radians(90.), 1., .1, 25.);
			mat4 faces[] = {
				P * glm::lookAt(position, position + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)),
				P * glm::lookAt(position, position + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)),
				P * glm::lookAt(position, position + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0)),
				P * glm::lookAt(position, position + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0)),
				P * glm::lookAt(position, position + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0)),
				P * glm::lookAt(position, position + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0))
			};

			string lightFacePVId;
			for (int fIdx = 0; fIdx < 6; fIdx++) {
				lightFacePVId = "light.PV[" + std::to_string(fIdx) + "]";
				setInput(lightFacePVId, faces[fIdx]);
			}

			setInput("light.position", position);
			setInput("light.planeF", 25.f);
			eval(scene);
			break;
		}
	}

	void eval(Node* node) {
		program->setUniform("geometry.M", node->M);
		program->commit();

		if (!node->light  && node->geometry) {
			node->geometry->render();
		}

		for (auto child : node->children) {
			eval(child);
		}
	}

};