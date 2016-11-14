#include "Program.h"

#include "../GPUKit/Shader/Shaders.h"
#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

#include <cassert>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext.hpp>

using namespace OXFEDE::GPUKit;
using namespace std;
using namespace glm;

struct Program::_Private {
	static Program* activeProgram;

	bool linked{ false };
	uint32_t id{ 0 };
	uint32_t txuCount{ 0 };
};

Program* Program::_Private::activeProgram = nullptr;

Program* Program::getActive() {
	return _Private::activeProgram;
}

Program::Program() : 
	_private(new _Private()) {
	_private->id = glCreateProgram();
	
	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- create program: id: %i --", 
		_private->id);

	OXFEDE_LOG(LType::I, LGPK::Program, this, 
		"glCreateProgram(): %i", 
		_private->id);
}

Program::~Program() {
	glDeleteProgram(_private->id);

	for (auto uniform : uniforms) {
		delete uniform.second;
	}

	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- delete program (id = %i) --", 
		_private->id);
	
	OXFEDE_LOG(LType::I, LGPK::Program, this, 
		"glDeleteProgram(%i)", 
		_private->id);
}

uint32_t Program::getId() const {
	return _private->id;
}

bool Program::getLinked() const {
	return _private->linked;
}

void Program::link() {
	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- link program: id: %i --", 
		_private->id);

	if (_private->linked) {
		OXFEDE_LOG(LType::I, LGPK::General, this, 
			"-- already linked --");
		
		return;
	}
	
	vector<Shader*> stages;
	if (vStage) {
		stages.push_back(vStage);
	}

	if (gStage) {
		stages.push_back(gStage);
	}
	
	if (fStage) {
		stages.push_back(fStage);
	}
	
	for (auto stage : stages) {
		if (!stage->getCompiled()) {
			stage->compile();
		}

		glAttachShader(_private->id, stage->getId());

		OXFEDE_LOG(LType::I, LGPK::Program, this, 
			"glAttachShader(%i, %i)", 
			_private->id, stage->getId());
	}

	glLinkProgram(_private->id);

	OXFEDE_LOG(LType::I, LGPK::Program, this, 
		"glLinkProgram(%i)", 
		_private->id);
	
	for (auto stage : stages) {
		glDetachShader(_private->id, stage->getId());

		OXFEDE_LOG(LType::I, LGPK::Program, this, 
			"glDetachShader(%i, %i)", 
			_private->id, stage->getId());
	}

	GLint logLength = 0;
	glGetProgramiv(_private->id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* log = (GLchar*)malloc(sizeof(logLength) * logLength);
		glGetProgramInfoLog(_private->id, logLength, &logLength, log);

		OXFEDE_LOG(LType::W, LGPK::Program, this, 
			log);
		
		free(log);
	}

	GLint status = GL_FALSE;

	glGetProgramiv(_private->id, GL_LINK_STATUS, &status);
	assert(status == GL_TRUE);

	logLength = 0;
	glGetProgramiv(_private->id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* log = (GLchar*)malloc(sizeof(logLength) * logLength);
		glGetProgramInfoLog(_private->id, logLength, &logLength, log);

		OXFEDE_LOG(LType::W, LGPK::Program, this, 
			log);
		
		free(log);
	}

	glGetProgramiv(_private->id, GL_VALIDATE_STATUS, &status);
	assert(status == GL_TRUE);

	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- create mapped uniforms --");

	if (getActive() != this) {
		setActive();
	}

	MappedUniformBase* mappedUniform;

	for (auto stage : stages) {
		for (auto pair : stage->uniforms) {
			Shader::UniformBase* uniform = pair.second;
			auto type = uniform->dynamicType();
			int location = glGetUniformLocation(_private->id, pair.first.c_str());

			string logOut = "glGetUniformLocation("
				+ std::to_string(_private->id)
				+ ", "
				+ pair.first
				+ ") = "
				+ std::to_string(location);

			OXFEDE_LOG(LType::I, LGPK::Program, this, 
				logOut);

			if (location == -1) {
				OXFEDE_LOG(LType::I, LGPK::General, this, 
					"-- skip unused uniform --");
				
				//continue;
			}

			if (type == Shader::Uniform<bool>::staticType()) {
				mappedUniform = new MappedUniform<bool>;
			}
			else if (type == Shader::Uniform<int>::staticType()) {
				mappedUniform = new MappedUniform<int>;
			}
			else if (type == Shader::Uniform<float>::staticType()) {
				mappedUniform = new MappedUniform<float>;
			}
			else if (type == Shader::Uniform<vec2>::staticType()) {
				mappedUniform = new MappedUniform<vec2>;
			}
			else if (type == Shader::Uniform<vec3>::staticType()) {
				mappedUniform = new MappedUniform<vec3>;
			}
			else if (type == Shader::Uniform<vec4>::staticType()) {
				mappedUniform = new MappedUniform<vec4>;
			}
			else if (type == Shader::Uniform<mat3>::staticType()) {
				mappedUniform = new MappedUniform<mat3>;
			}
			else if (type == Shader::Uniform<mat4>::staticType()) {
				mappedUniform = new MappedUniform<mat4>;
			}
			else if (type == Shader::Uniform<Texture2D*>::staticType()) {
				assert(_private->txuCount < Texture::getTXUMax());
				
				mappedUniform = new MappedUniform<Texture2D*>(_private->txuCount);
				
				glUniform1i(location, _private->txuCount);
				
				OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
					"glUniform1i(%i, %i)", 
					location, _private->txuCount);
				
				_private->txuCount++;
			}
			else if (type == Shader::Uniform<Texture3D*>::staticType()) {
				assert(_private->txuCount < Texture::getTXUMax());

				mappedUniform = new MappedUniform<Texture3D*>(_private->txuCount);

				glUniform1i(location, _private->txuCount);

				OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
					"glUniform1i(%i, %i)", 
					location, _private->txuCount);

				_private->txuCount++;
			}
			else {
				abort();
			}

			mappedUniform->location = location;
			uniforms[pair.first] = mappedUniform;
		}
	}

	_private->linked = true;
}

void Program::setActive() {
	_Private::activeProgram = this;
	glUseProgram(_private->id);
	
	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- activate program: id: %i --", 
		_private->id);
	
	OXFEDE_LOG(LType::I, LGPK::Program, this, 
		"glUseProgram(%i)", 
		_private->id);
}

template <> void Program::MappedUniform<bool>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniform1i(location, currentValue);

		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniform1i(%i, %i)", 
			location, currentValue);
	}
}

template <> void Program::MappedUniform<int>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniform1i(location, currentValue);
		
		OXFEDE_LOG(LType::W, LGPK::Uniform, this, 
			"glUniform1i(%i, %i)", 
			location, currentValue);
	}
}

template <> void Program::MappedUniform<float>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniform1fv(location, 1, &currentValue);
		
		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniform1fv(%i, 1, %f)", 
			location, currentValue);
	}
}

template <> void Program::MappedUniform<vec2>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniform2fv(location, 1, value_ptr(currentValue));
		
		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniform2fv(%i, 1, %s)", 
			location, to_string(currentValue).c_str());
	}
}

template <> void Program::MappedUniform<vec3>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniform3fv(location, 1, value_ptr(currentValue));
		
		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniform3fv(%i, 1, %s)", 
			location, to_string(currentValue).c_str());
	}
}

template <> void Program::MappedUniform<vec4>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniform4fv(location, 1, value_ptr(currentValue));
		
		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniform4fv(%i, 1, %s)", 
			location, to_string(currentValue).c_str());
	}
}

template <> void Program::MappedUniform<mat3>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(currentValue));
		
		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniformMatrix3fv(%i, 1, GL_FALSE, %s)", 
			location, to_string(currentValue).c_str());
	}
}

template <> void Program::MappedUniform<mat4>::commit() {
	if (currentValue != previousValue) {
		previousValue = currentValue;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(currentValue));
		
		OXFEDE_LOG(LType::I, LGPK::Uniform, this, 
			"glUniformMatrix4fv(%i, 1, GL_FALSE, %s)", 
			location, to_string(currentValue).c_str());
	}
}

void Program::MappedUniform<Texture*>::commit() {
	if (currentValue) {
		if (currentValue != previousValue) {
			previousValue = currentValue;
			currentValue->setTXU(GL_TEXTURE0 + txu);
		}

		currentValue->setActive();
	}
}

#define GPUKIT_INCLUSIVE_COMMIT true

void Program::commit() {
	OXFEDE_LOG(LType::I, LGPK::General, this, 
		"-- commit program (id = %i) --", 
		_private->id);

	if (getActive() != this) {
		setActive();
	}

	assert(getLinked());

	for (auto uniform : uniforms) {	
		OXFEDE_LOG(LType::I, LGPK::General, this, 
			"-- commit uniform %s --", 
			uniform.first.c_str());
		
		uniform.second->commit();
	}
}
