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

#include "Geometry.h"

#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

using namespace OXFEDE::GPUKit;

#define OFFSET(T, M) (GLvoid*)offsetof(T, M)

struct Geometry::_Private {
	static Geometry* activeGeometry;
	uint32_t id{ 0 };
	uint32_t vbo{ 0 };
	uint32_t ebo{ 0 };
};

Geometry* Geometry::_Private::activeGeometry = nullptr;

Geometry* Geometry::getActive() {
	return _Private::activeGeometry;
}

Geometry::Geometry() :
	_private(new _Private()) {
	glGenVertexArrays(1, &_private->id);
	glGenBuffers(1, &_private->vbo);
	glGenBuffers(1, &_private->ebo);

	OXFEDE_LOG(LType::I, LGPK::General, this,
		"-- create geometry: id: %i --", 
		_private->id);

	OXFEDE_LOG(LType::I, LGPK::Geometry, this,
		"glGenVertexArrays(1, _): %i", 
		_private->id);

	OXFEDE_LOG(LType::I, LGPK::Geometry, this,
		"glGenBuffers(1, _): %i (VBO)", 
		_private->vbo);

	OXFEDE_LOG(LType::I, LGPK::Geometry, this,
		"glGenBuffers(1, _): %i (EBO)", 
		_private->ebo);
}

Geometry::Geometry(Data data) : 
	Geometry() {
	this->data = data;
}

Geometry::~Geometry() {
	glDeleteBuffers(1, &_private->ebo);
	glDeleteBuffers(1, &_private->vbo);
	glDeleteVertexArrays(1, &_private->id);
}

uint32_t Geometry::getId() const {
	return _private->id;
}

void Geometry::setActive() {
	glBindVertexArray(_private->id);
	_Private::activeGeometry = this;
}

void Geometry::commit() {
	setActive();

	glBindBuffer(GL_ARRAY_BUFFER, _private->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _private->ebo);

	GLsizei VxASize = sizeof(VxAttribute);
	GLsizei idxSize = sizeof(uint32_t);

	glBufferData(GL_ARRAY_BUFFER, VxASize * data.vertices.size(), &data.vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxSize * data.indices.size(), &data.indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(data.locations.position);
	glVertexAttribPointer(data.locations.position, 3, GL_FLOAT, GL_FALSE, VxASize, OFFSET(VxAttribute, position));

	glEnableVertexAttribArray(data.locations.normal);
	glVertexAttribPointer(data.locations.normal, 3, GL_FLOAT, GL_FALSE, VxASize, OFFSET(VxAttribute, normal));

	glEnableVertexAttribArray(data.locations.tx0);
	glVertexAttribPointer(data.locations.tx0, 2, GL_FLOAT, GL_FALSE, VxASize, OFFSET(VxAttribute, tx0));
}

void Geometry::render() {
	setActive();
	glDrawElements(GL_TRIANGLES, data.indices.size(), GL_UNSIGNED_INT, 0);
}