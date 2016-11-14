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

#include "Texture.h"

#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

using namespace OXFEDE::GPUKit;

struct Texture::_Private {
	static Texture* active;
	static GLint txuMax;

	uint32_t txu{ GL_TEXTURE0 };
	bool bound{ false };
	uint32_t id{ 0 };
	Type type;
};

Texture* Texture::_Private::active = nullptr;
GLint Texture::_Private::txuMax = 0;

Texture* Texture::getActive() {
	return _Private::active;
}

uint32_t Texture::getTXUMax() {	
	if (!_Private::txuMax) {
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_Private::txuMax);
	}

	return _Private::txuMax;
}

Texture::Texture(Type type) : 
	_private(new _Private()) {
	_private->type = type;
	glGenTextures(1, &_private->id);
	
	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- create texture: id: %i --", _private->id);

	OXFEDE_LOG(LType::I, GPUKIT::Texture, this, 
		"glGenTextures(1, _): %i", _private->id);
}

Texture::~Texture() {
	glDeleteTextures(1, &_private->id);

	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- delete texture: id %i --", _private->id);
	OXFEDE_LOG(LType::I, GPUKIT::Texture, this, 
		"glDeleteTextures(1, %i)", _private->id);
}

Texture::Type Texture::getType() const {
	return _private->type;
}

void Texture::setActive() {
	glActiveTexture(_private->txu);

	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- activate texture: id: %i | TXU: %i (%i)) --", 
		_private->id, _private->txu, _private->txu - GL_TEXTURE0);
	
	OXFEDE_LOG(LType::I, GPUKIT::Texture, this, 
		"glActiveTexture(%i)", _private->txu);

	glBindTexture(_private->type, _private->id);
	
	OXFEDE_LOG(LType::I, GPUKIT::Texture, this, 
		"glBindTexture(%i, %i)", _private->type, _private->id);

	_Private::active = this;
}

uint32_t Texture::getId() const {
	return _private->id;
}

uint32_t Texture::getTXU() const {
	return _private->txu;
}

void Texture::setTXU(uint32_t txu) {
	_private->txu = txu;
}
