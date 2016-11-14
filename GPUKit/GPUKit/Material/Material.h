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
#include "../GPUKit/Program/Program.h"

#include <string>
#include <unordered_map>

namespace OXFEDE {

	namespace GPUKit {

		class Program;

		class Material final {

		public:
			std::string name;

			Material();
			Material(Program* program);
			~Material();

			void setProgram(Program* program);
			Program* getProgram() const;

			template <class T>
			void setAttribute(const std::string& key, T value) {
				auto found = attributes.find(key);
				assert(found != attributes.end());

				auto proxy = static_cast<Program::UniformProxy<T>*>(found->second);
				proxy->value = value;
			}

			template <class T>
			T getAttribute(const std::string& key) const {
				auto found = attributes.find(key);
				assert(found != attributes.end());

				auto proxy = static_cast<Program::UniformProxy<T>*>(found->second);
				return proxy->value;
			}

			void commit();

		private:
			std::unordered_map<std::string, Program::UniformProxyBase*> attributes;

			OXFEDE_PRIVATE;
		};

	}

}