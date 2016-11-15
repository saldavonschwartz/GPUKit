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

#include "Pass.h"

#include "../GPUKit/Texture/Texture.h"
#include "../GPUKit/Buffer/Buffer.h"
#include "../GPUKit/Program/Program.h"
#include "../GPUKit/Shader/FShader/FShader.h"
#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

using namespace OXFEDE::GPUKit;
using namespace std;

struct Pass::_Private {
	static uint32_t quadVAO;

	_Private() {
		if (!quadVAO) {
			GLfloat quadVertices[] = {
				-1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
				1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			};
			
			glGenVertexArrays(1, &quadVAO);
			
			OXFEDE_LOG(LType::I, LGPK::General, this, 
				"-- create quad pass VAO --", 
				quadVAO);

			OXFEDE_LOG(LType::I, LGPK::Geometry, this, 
				"glGenVertexArrays(1, _): %i", 
				quadVAO);

			glBindVertexArray(quadVAO);
			
			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);

			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		}
	}
};

uint32_t Pass::_Private::quadVAO = 0;

Pass::Pass() :
	_private(new _Private()) {
}

Pass::~Pass() {
}

void Pass::setOutput(const string& key, Texture* value) {
	auto target = program->fStage->outputs[key]->location;
	buffer->setAttachment(target, value);
	buffer->setAttachmentWriteEnabled(target, true);

	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- target %i (%s) <= texture --", 
		target, key.c_str(),  value ? value->getId() : 0);
}

Texture* Pass::getOutput(const string& key) const {
	auto target = program->fStage->outputs[key]->location;
	return buffer->getAttachment(target);
}

void Pass::renderQuad() {
	static Pass* logDummy = nullptr;

	OXFEDE_LOG(LType::I, LGPK::General, logDummy,
		"-- render quad --");
	
	glBindVertexArray(_Private::quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}