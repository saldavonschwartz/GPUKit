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

#include "Texture3D.h"

#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

using namespace OXFEDE::GPUKit;

Texture3D::Texture3D() :
	Texture(Type::D3) {
}

Texture3D::Texture3D(Data data) :
	Texture3D() {
	this->data = data;
}

void Texture3D::commit() {
	OXFEDE_LOG(LType::I, LGPK::General, this,
		"-- commit texture: id: %i --", 
		getId());

	if (getActive() != this) {
		setActive();
	}
	
	int type = getType();
	int i = 0;

	for (Image& image : data.image) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, image.targetFormat, image.size.x, image.size.y,
			0, image.sourceformat, image.pixelType, image.bytes);

		OXFEDE_LOG(LType::I, LGPK::Texture, this, 
			"glTexImage2D(%i, %i, %i, %i, %i, %i, %i, %i, %p)", 
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, image.targetFormat, image.size.x, image.size.y,
			0, image.sourceformat, image.pixelType, image.bytes
		);

		i++;
	}

	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, data.filter.minification);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, data.filter.magnification);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, data.wrapMode.S);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, data.wrapMode.T);
	glTexParameteri(type, GL_TEXTURE_WRAP_R, data.wrapMode.R);

	OXFEDE_LOG(LType::I, LGPK::Texture, this, 
		"glTexParameteri(%i, GL_TEXTURE_MIN_FILTER, %i)", 
		type, data.filter.minification);
	
	OXFEDE_LOG(LType::I, LGPK::Texture, this, 
		"glTexParameteri(%i, GL_TEXTURE_MAG_FILTER, %i)", 
		type, data.filter.magnification);
	
	OXFEDE_LOG(LType::I, LGPK::Texture, this, 
		"glTexParameteri(%i, GL_TEXTURE_WRAP_S, %i)", 
		type, data.wrapMode.S);
	
	OXFEDE_LOG(LType::I, LGPK::Texture, this, 
		"glTexParameteri(%i, GL_TEXTURE_WRAP_T, %i)", 
		type, data.wrapMode.T);
	
	OXFEDE_LOG(LType::I, LGPK::Texture, this, 
		"glTexParameteri(%i, GL_TEXTURE_WRAP_R, %i)", 
		type, data.wrapMode.R);

	if (data.mipmaps) {
		glGenerateMipmap(type);
		
		OXFEDE_LOG(LType::I, LGPK::Texture, this, 
			"glGenerateMipmaps()", 
			type);
	}
	else {
		OXFEDE_LOG(LType::I, LGPK::General, this, 
			"no mipmaps", 
			type);
	}
}

void Texture3D::fetch() {
	if (getActive() != this) {
		setActive();
	}

	GLint bytes;
	int i = 0;
	
	for (Image& image : data.image) {
		bytes = image.size.x * image.size.y * 3;

		if (!bytes) {
			continue;
		}

		image.bytes = (uint8_t*)malloc(bytes);
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, image.sourceformat, image.pixelType, image.bytes);
	
		OXFEDE_LOG(LType::I, LGPK::General, this,
			"-- fetch texture: id: %i --", 
			getId());

		OXFEDE_LOG(LType::I, LGPK::Texture, this,
			"glGetTexImage(%i, 0, %i, %i, %p)", 
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			image.sourceformat, image.pixelType, image.bytes);

		i++;
	}
}