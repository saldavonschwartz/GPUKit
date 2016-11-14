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

#include "../Dependencies/Defines.h"
#include "../GPUKit/GLWrapper.h"

#include <glm/vec2.hpp>

namespace OXFEDE { 
	
	namespace GPUKit {

		class Texture {

		public:
			enum Type : uint32_t {
				D2 = 0x0DE1,
				D3 = 0x8513
			};

			struct Image {
				glm::uvec2 size;
				uint32_t targetFormat{ GL_RGB };
				uint32_t sourceformat{ GL_RGB };
				uint32_t pixelType{ GL_UNSIGNED_BYTE };
				uint8_t* bytes{ nullptr };
			};

			struct WrapMode {
				uint32_t S{ GL_CLAMP_TO_EDGE };
				uint32_t T{ GL_CLAMP_TO_EDGE };
			};

			struct Filter {
				uint32_t minification{ GL_NEAREST };
				uint32_t magnification{ GL_NEAREST };
			};

			struct Data {
				bool mipmaps{ false };
			};

			static Texture* getActive();
			static uint32_t getTXUMax();

			Texture(Type type);
			virtual ~Texture();			
			void setActive();
			Type getType() const;
			uint32_t getTXU() const;
			void setTXU(uint32_t txu);
			uint32_t getId() const;
			virtual void commit() = 0;
			virtual void fetch() = 0;

			OXFEDE_PRIVATE;
		};

	}

}
