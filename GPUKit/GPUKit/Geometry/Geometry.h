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

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace OXFEDE {

	namespace GPUKit {

		class Geometry final {

		public:
			struct VxAttribute {
				glm::vec3 position;
				glm::vec3 normal;
				glm::vec2 tx0;
			};

			struct Data {
				struct Locations {
					int position{ 0 };
					int normal{ 1 };
					int tx0{ 2 };
				} locations;

				std::vector<uint32_t> indices;
				std::vector<VxAttribute> vertices;
			};

			static Geometry* getActive();

			Data data;

			Geometry(Data data);
			Geometry();
			~Geometry();
			void commit();
			uint32_t getId() const;
			void setActive();
			void render();

			OXFEDE_PRIVATE;
		};
	}

}