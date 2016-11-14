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

#include "../GPUKit/Pass/Pass.h"
#include "../GPUKit/Texture/Textures.h"
#include "../GPUKit/Buffer/Buffer.h"
#include "../GPUKit/Shader/Shaders.h"
#include "../GPUKit/Program/Program.h"
#include "../GPUKit/Geometry/Geometry.h"
#include "../GPUKit/Importers.h"

#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

#include <glm/glm.hpp>
#include <cassert>

/*	To use something other than GLFW, include your GL wrapper lib in GLWrapper.h and
	modify GPUKit::init and GPUKit::shutdown with the appropriate implementation.*/

namespace OXFEDE { 

	using namespace glm;

	namespace GPUKit {

		class GPUKitFramework;

		struct GLVersion {
			int major;
			int minor;
		};

		static GLFWwindow* window;
		static GPUKitFramework* logDummy{ nullptr };

		void init(glm::uvec2 windowSize, GLVersion glVersion) {
			OXFEDE_LOG(LType::I, GPUKIT::General, logDummy,
				"-- GPUKit init (window = %p) --",
				window);

			assert(!window);

			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glVersion.major);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glVersion.minor);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
			glfwWindowHint(GLFW_SAMPLES, 0);
			glfwWindowHint(GLFW_STENCIL_BITS, 8);

			window = glfwCreateWindow(windowSize.x, windowSize.y, "", nullptr, nullptr);
			if (!window) {
				cout << "Failed to create GLFW window\n";
				abort();
			}

			glfwMakeContextCurrent(window);
			glewInit();
		}

		void shutDown() {
			OXFEDE_LOG(LType::I, GPUKIT::General, logDummy,
				"-- GPUKit shutdown --");

			assert(window);

			glfwDestroyWindow(window);
			window = nullptr;
		}

		void swapBuffers() {
			OXFEDE_LOG(LType::I, GPUKIT::General, logDummy,
				"glfwSwapBuffers((window %p))",
				window);

			glfwSwapBuffers(window);
		}

		void drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices) {
			OXFEDE_LOG(LType::I, GPUKIT::General, logDummy,
				"glDrawElements(%i, %i, %i, %p)",
				mode, count, type, indices);

			glDrawElements(mode, count, type, indices);
		}

		void drawArrays(GLenum mode, GLint first, GLsizei count) {
			OXFEDE_LOG(LType::I, GPUKIT::General, logDummy,
				"glDrawArrays(%i, %i, %i)",
				mode, first, count);

			glDrawArrays(mode, first, count);
		}
	}
}
