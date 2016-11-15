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
using glm::mat4;
using glm::column;
using std::string;

struct LightPass : public Pass {
	Node* cameraNode;
	Node* lightsNode;

	LightPass() {
		program = AssetImporter<Program*>::import("Example/Shaders/Lighting.glsl");
		program->link();

		buffer = new Buffer;
		buffer->setClearTargets(Buffer::Clear::ALL_COLORS);
	}

	~LightPass() {
		delete program;
		delete buffer;
	}

	void render() override {
		glDisable(GL_DEPTH_TEST);
		buffer->commit();

		string lId;
		int lIdx = 0;

		for (Node* lightNode : lightsNode->children) {
			lId = "light[" + std::to_string(lIdx++) + "]";
			setInput(lId + ".position", column(lightNode->M, 3));
			setInput(lId + ".color", lightNode->light->color);
			setInput(lId + ".intensity", lightNode->light->intensity);
			setInput(lId + ".attenuation", lightNode->light->attenuation);
			setInput(lId + ".farPlane", 25.f);
		}

		setInput("camera.position", column(cameraNode->M, 3));
		program->commit();

		renderQuad();
	}
};