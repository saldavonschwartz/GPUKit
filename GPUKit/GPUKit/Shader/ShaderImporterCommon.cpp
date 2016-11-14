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

#include "ShaderImporterCommon.h"

#include "../GPUKit/Shader/VShader/VShader.h"
#include "../GPUKit/Shader/FShader/FShader.h"
#include "../GPUKit/Shader/GShader/GShader.h"
#include "../GPUKit/Texture/2D/Texture2D.h"
#include "../GPUKit/Texture/3D/Texture3D.h"

#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <sstream>
#include <fstream>

using namespace OXFEDE::GPUKit;
using namespace std;
using namespace glm;

regex ShaderImporterCommon::STRUCT_START(R"F(struct\s+(\w+)\s*\{)F");
regex ShaderImporterCommon::STRUCT_MEMBER(R"F(\s*([\w\d]+)\s+([\w\d]+)(?:\[(\d+)\])?;)F");
regex ShaderImporterCommon::STRUCT_END(R"F(\};)F");
regex ShaderImporterCommon::OUT(R"F((?:layout \(location = (\d+)\))?\s*out\s*([\w\d]+)\s+([\w\d]+);)F");
regex ShaderImporterCommon::IN(R"F((?:layout \(location = (\d+)\))?\s*in\s*([\w\d]+)\s+([\w\d]+);)F");
regex ShaderImporterCommon::UNIFORM(R"F(.*\s*uniform\s*([\w\d]+)\s+([\w\d]+)(?:\[(\d+)\])?;)F");
regex ShaderImporterCommon::FLAG(R"F(#GPUKIT_ENABLE\s+(.*))F");
regex ShaderImporterCommon::STAGE_START(R"F(\#GPUKIT_(VERTEX|GEOMETRY|FRAGMENT)_STAGE)F");
regex ShaderImporterCommon::STAGE_END(R"F(\#GPUKIT_END_STAGE)F");

using OutputBase = Shader::OutputBase;
using InputBase = Shader::InputBase;
using UniformBase = Shader::UniformBase;
template <class T> using Input = Shader::Input<T>;
template <class T> using Output = Shader::Output<T>;
template <class T> using Uniform = Shader::Uniform<T>;

template <class T, template <class T3> class T2> 
T* attributeFromType(string& valueType) {
	T* baseTypePtr;

	if (valueType == "bool") {
		baseTypePtr = new T2<bool>;
	}
	else if (valueType == "int") {
		baseTypePtr = new T2<int>;
	}
	else if (valueType == "float") {
		baseTypePtr = new T2<float>;
	}
	else if (valueType == "vec2") {
		baseTypePtr = new T2<vec2>;
	}
	else if (valueType == "vec3") {
		baseTypePtr = new T2<vec3>;
	}
	else if (valueType == "vec4") {
		baseTypePtr = new T2<vec4>;
	}
	else if (valueType == "mat3") {
		baseTypePtr = new T2<mat3>;
	}
	else if (valueType == "mat4") {
		baseTypePtr = new T2<mat4>;
	}
	else if (valueType == "sampler2D") {
		baseTypePtr = new T2<Texture2D*>;
	}
	else if (valueType == "samplerCube") {
		baseTypePtr = new T2<Texture3D*>;
	}
	else {
		abort();
	}

	return baseTypePtr;
}

void ShaderImporterCommon::attributeFromDescriptor(AttrDescriptor descriptor, Shader* target, const StructMap& structs) {
	auto structType = structs.find(descriptor.valueType);
	if (structType != structs.end()) {
		for (auto structMember : structType->second) {
			AttrDescriptor newDescriptor = {
				descriptor.location,
				descriptor.type,
				structMember[0],
				descriptor.identifier + "." + structMember[1]
			};

			attributeFromDescriptor(newDescriptor, target, structs);
		}
	}
	else {
		if (descriptor.type == AttrDescriptor::Type::In) {
			InputBase* attribute = attributeFromType<InputBase, Input>(descriptor.valueType);
			attribute->location = descriptor.location;
			VShader* shader = static_cast<VShader*>(target);
			shader->inputs[descriptor.identifier] = attribute;
		}
		else if (descriptor.type == AttrDescriptor::Type::Out) {
			OutputBase* attribute = attributeFromType<OutputBase, Output>(descriptor.valueType);
			attribute->location = (Buffer::Target)descriptor.location;
			FShader* shader = static_cast<FShader*>(target);
			shader->outputs[descriptor.identifier] = attribute;
		}
		else {
			UniformBase* attribute = attributeFromType<UniformBase, Uniform>(descriptor.valueType);
			target->uniforms[descriptor.identifier] = attribute;
		}
	}
}