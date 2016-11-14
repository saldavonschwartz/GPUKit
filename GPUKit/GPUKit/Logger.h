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

#include "../Dependencies/Logger/Logger.h"

namespace OXFEDE {

	OXFEDE_FLAG(GPUKIT, uint8_t,) {
		Texture = 1 << 0,
		Program = 1 << 1,
		Buffer = 1 << 2,
		Shader = 1 << 3,
		Uniform = 1 << 4,
		VertexArray = 1 << 5,
		General = 1 << 6,
		Importer = 1 << 7
	};

	class logGPK final : public Logger<GPUKIT> {

	public:
		static logGPK& getInstance() {
			static logGPK logger("GPUKit", "logDefault.log", false);
			return logger;
		}

	protected:
		logGPK(std::string name, std::string path, bool useBackgroundThread) :
			Logger(name, path, useBackgroundThread) {
			fileLog = 
				GPUKIT::Texture
				| GPUKIT::Program
				| GPUKIT::Buffer
				| GPUKIT::Shader
				| GPUKIT::Uniform
				| GPUKIT::VertexArray
				| GPUKIT::General
				| GPUKIT::Importer;
			
			screenLog =
				GPUKIT::Texture
				| GPUKIT::Program
				| GPUKIT::Buffer
				| GPUKIT::Shader
				| GPUKIT::Uniform
				| GPUKIT::VertexArray
				| GPUKIT::General
				| GPUKIT::Importer;
		}

		~logGPK() = default;
	};

#define logGPK logGPK::getInstance()

#define LOG_ENABLED false

#if LOG_ENABLED 
#define OXFEDE_LOG(...) logGPK.log(__VA_ARGS__)
#else
#define OXFEDE_LOG(...) 
#endif 

}