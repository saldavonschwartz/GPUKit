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

#include "Shader.h"

#include "../GPUKit/Texture/Texture.h"
#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

#include <cassert>

using namespace OXFEDE::GPUKit;

struct Shader::_Private{
	Type type;
	uint32_t id{ 0 };
	bool compiled{ false };
};

Shader::Shader(Type type) :
	_private(new _Private()) {
	_private->type = type;
	_private->id = glCreateShader(type);
	
	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- create shader: id: %i --", 
		_private->id);

	OXFEDE_LOG(LType::I, LGPK::Shader, this, 
		"glCreateShader(): %i", 
		_private->id);
}

Shader::~Shader() {
	glDeleteShader(_private->id);

	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- delete shader: id: %i --", 
		_private->id);
	
	OXFEDE_LOG(LType::I, LGPK::Shader, this, 
		"glDeleteShader(%i)", 
		_private->id);

	for (auto uniform : uniforms) {
		delete uniform.second;
	}
}

void Shader::compile() {
	if (_private->compiled) {
		return;
	}

	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- compile shader: id: %i --", 
		_private->id);

	const GLchar* sourceCString = source.c_str();
	uint32_t id = _private->id;

	glShaderSource(id, 1, &sourceCString, nullptr);

	OXFEDE_LOG(LType::I, LGPK::Shader, this, 
		"glShaderSource(%i, %i, <source>, NULL)", 
		id, 1);

	glCompileShader(id);
	
	OXFEDE_LOG(LType::I, LGPK::Shader, this, 
		"glCompileShader(%i)", 
		id);

	GLint logLength = 0;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* log = (GLchar*)malloc(sizeof(logLength) * logLength);
		glGetShaderInfoLog(id, logLength, &logLength, log);
		
		OXFEDE_LOG(LType::W, LGPK::Shader, 
			this, log);
		
		free(log);
	}

	GLint status = GL_FALSE;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	OXFEDE_LOG(LType::I, LGPK::Shader, this, 
		"glGetShaderiv(%i, GL_COMPILE_STATUS, _) = %i", 
		id, status);
	
	assert(status == GL_TRUE);

	_private->compiled = true;
}

bool Shader::getCompiled() const {
	return _private->compiled;
}

uint32_t Shader::getId() const {
	return _private->id;
}

Shader::Type Shader::getType() const {
	return _private->type;
}