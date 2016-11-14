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

namespace OXFEDE { 

	namespace GPUKit {

		class Texture;

		class Buffer {

		public:
			enum Target : uint32_t {
				COLOR0 = 0X8CE0,
				COLOR1 = 0X8CE1,
				COLOR2 = 0X8CE2,
				COLOR3 = 0X8CE3,
				COLOR4 = 0X8CE4,
				COLOR5 = 0X8CE5,
				COLOR6 = 0X8CE6,
				COLOR7 = 0X8CE7,
				COLOR8 = 0X8CE8,
				COLOR9 = 0X8CE9,
				COLOR10 = 0X8CEA,
				COLOR11 = 0X8CEB,
				COLOR12 = 0X8CEC,
				COLOR13 = 0X8CED,
				COLOR14 = 0X8CEE,
				COLOR15 = 0X8CEF,
				DEPTH = 0X8D00,
				STENCIL = 0X8D20,
				COUNT = 18
			};

			OXFEDE_FLAG(Clear, uint32_t, friend) {
				NONE = 0,
				ALL_COLORS = 0x00004000,
				DEPTH = 0x00000100,
				STENCIL = 0x00000400
			};

			static Buffer* getActive();
			static Buffer* getMainBuffer();

			Buffer();
			~Buffer();
			uint32_t getId() const;
			void setActive();
			void validate() const;
			void setAttachment(Target target, Texture* texture);
			Texture* getAttachment(Target target);
			void setAttachmentWriteEnabled(Target target, bool write);
			void setClearTargets(Clear targetsClearMask);
			Clear getClearTargets() const;
			void commit();

			OXFEDE_PRIVATE;
		};

	}

}
