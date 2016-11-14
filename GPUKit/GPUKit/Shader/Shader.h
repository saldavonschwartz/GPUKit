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
#include "../GPUKit/Buffer/Buffer.h"

#include <unordered_map>
#include <string>

namespace OXFEDE { 
	
	namespace GPUKit {

		class Program;

		class Shader {

		public:
			enum Type : uint32_t {
				Vertex = 0x8B31,
				Geometry = 0x8DD9,
				Fragment = 0x8B30
			};

			struct OutputBase {
				Buffer::Target location{};
				virtual std::type_index dynamicType() const = 0;
				virtual ~OutputBase() = default;
			};

			template <class T>
			struct Output : public OutputBase {

				static std::type_index staticType() {
					return OXFEDE_TYPE(T);
				}

				std::type_index dynamicType() const override {
					return OXFEDE_TYPE(T);
				}
			};

			struct InputBase {
				int location{};
				virtual std::type_index dynamicType() const = 0;
				virtual ~InputBase() = default;
			};

			template <class T>
			struct Input : public InputBase {

				static std::type_index staticType() {
					return OXFEDE_TYPE(T);
				}

				std::type_index dynamicType() const override {
					return OXFEDE_TYPE(T);
				}
			};

			struct UniformBase {
				virtual std::type_index dynamicType() const = 0;
				virtual ~UniformBase() = default;
			};

			template <class T>
			struct Uniform : public UniformBase {

				static std::type_index staticType() {
					return OXFEDE_TYPE(T);
				}

				std::type_index dynamicType() const override {
					return OXFEDE_TYPE(T);
				}
			};

			std::string source;
			std::unordered_map<std::string, UniformBase*> uniforms;

			Shader(Type type);
			virtual ~Shader() = 0;
			uint32_t getId() const;
			Type getType() const;
			void compile();
			bool getCompiled() const;

			OXFEDE_PRIVATE;
		};

	}

}