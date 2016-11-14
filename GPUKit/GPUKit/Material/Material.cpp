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

#include "Material.h"

#include "../GPUKit/Logger.h"

#include <cassert>
#include <string>

using namespace OXFEDE::GPUKit;
using namespace std;

struct Material::_Private {
	Program* program;
};

Material::Material() :
	_private(new _Private()) {
}

Material::Material(Program* program) :
	Material() {
	setProgram(program);
}

Material::~Material() {
	for (auto attribute : attributes) {
		delete attribute.second;
	}
}

void Material::setProgram(Program* program) {
	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- material %s: assign program: %i --", 
		name.c_str(), program->getId());

	assert(program && program->getLinked());

	_private->program = program;

	for (auto attribute : attributes) {
		delete attribute.second;
	}

	attributes.clear();

	for (auto uniform : program->uniforms) {
		auto prefix = uniform.first.find("material.");
		if (prefix != string::npos) {
			auto proxyName = uniform.first.substr(9, uniform.first.length() - 1);
			attributes[proxyName] = uniform.second->createProxy();

			OXFEDE_LOG(LType::I, GPUKIT::General, this, 
				"-- create material attributes %s --", 
				proxyName.c_str());
		}
	}
}

Program* Material::getProgram() const {
	return _private->program;
}

void Material::commit() {
	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- commit material %s --", 
		name.c_str());

	for (auto attribute : attributes) {
		attribute.second->commit("material." + attribute.first, _private->program);
	}
}