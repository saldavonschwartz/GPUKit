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

using namespace OXFEDE::GPUKit;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::transpose;
using glm::inverse;

struct GeometryPass : public Pass {
	Node* cameraNode;
	Node* scene;

	GeometryPass() {
		program = AssetImporter<Program*>::import("Example/Shaders/Geometry.glsl");
		program->link();

		buffer = new Buffer;
		buffer->setClearTargets(Buffer::Clear::ALL_COLORS | Buffer::Clear::DEPTH);
	}

	~GeometryPass() {
		delete program;
		delete buffer;
	}

	void render() override {
		glEnable(GL_DEPTH_TEST);

		buffer->commit();

		uvec2 size = ((Texture2D*)getOutput("outP"))->data.image.size;
		glViewport(0, 0, size.x, size.y);

		setInput("camera.P", cameraNode->camera->P);
		setInput("camera.V", glm::inverse(cameraNode->M));

		eval(scene);
	}

private:
	void eval(Node* node) {
		if (node->geometry) {
			mat3 N = mat3(transpose(inverse(node->M)));

			setInput("geometry.M", node->M);
			setInput("geometry.N", N);
			
			node->material->commit();
			program->commit();

			node->geometry->render();
		}

		for (auto child : node->children) {
			eval(child);
		}
	}
};
