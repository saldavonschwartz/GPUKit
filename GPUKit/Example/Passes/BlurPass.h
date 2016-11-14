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

using namespace OXFEDE::GPUKit;

struct BlurPass : public Pass {

	int iterations{ 3 };
	Texture2D pingPong[2];

	BlurPass() {
		program = AssetImporter<Program*>::import("Example/Shaders/Blur.glsl");
		program->link();

		int w;
		int h;
		glfwGetFramebufferSize(window, &w, &h);

		Texture2D::Data data;
		data.image.sourceformat = GL_RGB16F;
		data.image.targetFormat = GL_RGB;
		data.image.pixelType = GL_FLOAT;
		data.image.size = { w / 2., h / 2. };

		pingPong[0].data = data;
		pingPong[0].commit();

		pingPong[1].data = data;
		pingPong[1].commit();

		buffer = new Buffer;
	}

	~BlurPass() {
		delete program;
		delete buffer;
	}

	void render() override {
		glDisable(GL_DEPTH_TEST);

		Texture2D* in = getInput<Texture2D*>("source");
		Texture2D* out = (Texture2D*)getOutput("outC");

		setInput("iteration", 0.);
		program->commit();

		setOutput("outC", &pingPong[0]);
		buffer->commit();

		uvec2 txSize = pingPong[0].data.image.size;
		glViewport(0, 0, txSize.x, txSize.y);

		renderQuad();

		int i = 0;
		for (; i < iterations - 1; i++) {
			setInput("source", &pingPong[i % 2]);
			setInput("iteration", (float)(i + 1));
			program->commit();

			setOutput("outC", &pingPong[(i + 1) % 2]);
			buffer->commit();

			renderQuad();
		}

		setInput("source", &pingPong[i % 2]);
		setInput("iteration", (float)i);
		program->commit();

		setOutput("outC", out);
		buffer->commit();

		txSize = out->data.image.size;
		glViewport(0, 0, txSize.x, txSize.y);

		renderQuad();

		setInput("source", in);
	}
};
