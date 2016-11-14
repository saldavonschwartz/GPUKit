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

#include "Buffer.h"

#include "../GPUKit/Texture/Texture.h"
#include "../GPUKit/GLWrapper.h"
#include "../GPUKit/Logger.h"

#include <cassert>
#include <string>
#include <vector>
#include <map>

using namespace OXFEDE::GPUKit;
using namespace std;

struct Buffer::_Private {
	struct AttachmentState {
		Texture* currentTexture;
		Texture* previousTexture;
		bool currentWrite;
		bool previousWrite;
	};

	static Buffer* activeBuffer;
	static Buffer* mainBuffer;

	uint32_t id{ 0 };
	Clear targetsClearMask{ Clear::NONE };
	map<Target, AttachmentState> attachments;
};

Buffer* Buffer::_Private::activeBuffer{nullptr};
Buffer* Buffer::_Private::mainBuffer{nullptr};

Buffer::Buffer() :
	_private(new _Private()) {
	glGenFramebuffers(1, &_private->id);

	OXFEDE_LOG(LType::I, GPUKIT::General, this, "-- create framebuffer (id = %i) --", _private->id);
	OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, "glGenFramebuffers(1, _) = %i", _private->id);

	for (int i = 0; i < Target::COUNT - 2; i++) {
		_private->attachments[(Target)(0X8CE0 + i)] = {};
	}

	_private->attachments[Target::DEPTH] = {};
	_private->attachments[Target::STENCIL] = {};
}

Buffer::~Buffer() {
	glDeleteFramebuffers(1, &_private->id);

	OXFEDE_LOG(LType::I, GPUKIT::General, this, "-- delete framebuffer (id = %i) --", _private->id);
	OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, "glDeleteFramebuffers(1, _) = %i", _private->id);
}

Buffer* Buffer::getActive() {
	return _Private::activeBuffer;
}

Buffer* Buffer::getMainBuffer() {
	if (!_Private::mainBuffer) {
		static Buffer mainBuffer;
		_Private::mainBuffer = &mainBuffer;
		glDeleteFramebuffers(1, &mainBuffer._private->id);
		mainBuffer._private->id = 0;
	}

	return _Private::mainBuffer;
}

void Buffer::validate() const {
	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- validate framebuffer (id = %i) --", 
		_private->id);

	GLint logLength = 0;

	glGetProgramiv(_private->id, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar* log = (GLchar*)malloc(sizeof(logLength) * logLength);
		glGetProgramInfoLog(_private->id, logLength, &logLength, log);
		
		OXFEDE_LOG(LType::E, GPUKIT::Buffer, this, log);
		
		free(log);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	bool valid = status == GL_FRAMEBUFFER_COMPLETE;
	
	OXFEDE_LOG(valid ? LType::I : LType::E, GPUKIT::General, this, 
		"-- framebuffer (id = %i) validation: %s --",
		_private->id, valid ? "passed" : "failed");
	
	OXFEDE_LOG(valid ? LType::I : LType::E, GPUKIT::Buffer, this, 
		"glCheckFramebufferStatus(GL_FRAMEBUFFER) = %i", status);
	
	assert(valid);
}

void Buffer::setActive() {
	_Private::activeBuffer = this;
	glBindFramebuffer(GL_FRAMEBUFFER, _private->id);
	
	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- activate framebuffer (id = %i) --", _private->id);
	
	OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
		"glBindFramebuffer(GL_FRAMEBUFFER, %i)", _private->id);
}

uint32_t Buffer::getId() const {
	return _private->id;
}

void Buffer::setAttachment(Target target, Texture* texture) {
	_private->attachments[target].currentTexture = texture;
}

Texture* Buffer::getAttachment(Target target) {
	return _private->attachments[target].currentTexture;
}

void Buffer::setAttachmentWriteEnabled(Target target, bool write) {
	if (write && target != Target::DEPTH) {
		_private->attachments[target].currentWrite = true;
	}
	else {
		_private->attachments[target].currentWrite = false;
	}
}

void Buffer::setClearTargets(Clear targetsClearMask) {
	_private->targetsClearMask = targetsClearMask;
}

Buffer::Clear Buffer::getClearTargets() const {
	return _private->targetsClearMask;
}

void Buffer::commit() {
	OXFEDE_LOG(LType::I, GPUKIT::General, this, 
		"-- commit framebuffer (id = %i) --", 
		_private->id);

	if (getActive() != this) {
		setActive();
	}

	if (this != getMainBuffer()) {
		vector<Target> writes;

		for (auto attachment : _private->attachments) {
			auto target = attachment.first;
			auto state = attachment.second;

			if (state.currentTexture != state.previousTexture) {
				if (state.currentTexture) {
					auto txType = state.currentTexture->getType();
					auto txId = state.currentTexture->getId();

					if (txType == GL_TEXTURE_2D) {
						glFramebufferTexture2D(GL_FRAMEBUFFER, target, txType, txId, 0);
						
						OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
							"glFramebufferTexture2D(GL_FRAMEBUFFER, %i, %i, %i, 0)", 
							target, txType, txId);
					}
					else {
						glFramebufferTexture(GL_FRAMEBUFFER, target, txId, 0);
						
						OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
							"glFramebufferTexture(GL_FRAMEBUFFER, %i, %i, 0)", 
							target, txId);
					}
				}
				else if (state.previousTexture) {
					auto txType = state.previousTexture->getType();

					OXFEDE_LOG(LType::I, GPUKIT::General, this, 
						"unbind framebuffer attachment: Target %i", 
						target);

					if (txType == GL_TEXTURE_2D) {
						glFramebufferTexture2D(GL_FRAMEBUFFER, target, txType, 0, 0);
						
						OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
							"glFramebufferTexture2D(GL_FRAMEBUFFER, %i, %i, 0, 0)", 
							target, txType);
					}
					else {
						glFramebufferTexture(GL_FRAMEBUFFER, target, 0, 0);
						
						OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
							"glFramebufferTexture(GL_FRAMEBUFFER, %i, 0, 0)", 
							target);
					}
				}

				state.previousTexture = state.currentTexture;
			}

			if (state.currentWrite != state.previousWrite) {
				state.previousWrite = state.currentWrite;
				if (state.currentWrite) {
					writes.push_back(target);
				}
			}
		}

		if (!writes.size()) {
			glDrawBuffer(GL_NONE);

			OXFEDE_LOG(LType::I, GPUKIT::General, this, 
				"-- no draw targets --");

			OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
				"glDrawBuffer(GL_NONE)");
		}
		else {
			GLsizei i = 0;
			GLenum* writeTargets = new GLenum[writes.size()];
			for (Target target : writes) {
				writeTargets[i] = target;
				i++;
			}

			string writeIds;
			for (auto target : writes) {
				writeIds += std::to_string(target) + " ";
			}

			glDrawBuffers(i, writeTargets);
			delete[] writeTargets;
		
			OXFEDE_LOG(LType::I, GPUKIT::General, this, 
				"-- %i draw targets: [%s] --", 
				i, writeIds.c_str());

			OXFEDE_LOG(LType::I, GPUKIT::Buffer, this, 
				"glDrawBuffers(%i, [%s])", 
				i, writeIds.c_str());
		}

		OXFEDE_LOG(LType::I, GPUKIT::General, this, 
			"-- depth target %s --", 
			getAttachment(Target::DEPTH) ? "present" : "not present");
	}
	
	if (_private->targetsClearMask != Clear::NONE) {
		glClear(getUnderlyingValue(_private->targetsClearMask));
	}
}




