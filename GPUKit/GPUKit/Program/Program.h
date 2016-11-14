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
#include "../GPUKit/Texture/Texture.h"
#include "../GPUKit/Texture/2D/Texture2D.h"
#include "../GPUKit/Texture/3D/Texture3D.h"

#include <unordered_map>
#include <typeindex>
#include <string>

namespace OXFEDE { 
	
	namespace GPUKit {

		class VShader;
		class GShader;
		class FShader;

		class Program {

		public:
			static Program* getActive();

			VShader* vStage{ nullptr };
			GShader* gStage{ nullptr };
			FShader* fStage{ nullptr };

			Program();
			~Program();
			uint32_t getId() const;
			void setActive();
			void commit();
			void link();
			bool getLinked() const;
			
			template <class T> 
			void setUniform(const std::string& key, T value) {
				auto found = uniforms.find(key);
				assert(found != uniforms.end());
				auto uniform = static_cast<MappedUniform<T>*>(found->second);
				uniform->setValue(value);
			}

			template <class T> 
			T getUniform(const std::string& key) const {
				auto found = uniforms.find(key);
				assert(found != uniforms.end());
				auto uniform = static_cast<MappedUniform<T>*>(found->second);
				return uniform->getValue();
			}

			struct UniformProxyBase {
				virtual void commit(std::string key, Program* program) = 0;
			};

			template <class T>
			struct UniformProxy : public UniformProxyBase {
				T value {0};

				void commit(std::string key, Program* program) override {
					program->setUniform(key, value);
				}
			};

			struct MappedUniformBase {
				int location{ -1 };
				
				virtual ~MappedUniformBase() = default;
				virtual void commit() = 0;
				virtual UniformProxyBase* createProxy() = 0;

				std::type_index dynamicType() const {
					return OXFEDE_TYPE(*this);
				}
			};

			template <class T>
			class MappedUniform : public MappedUniformBase {

			public:
				static std::type_index staticType() {
					return OXFEDE_TYPE(MappedUniform<T>);
				}

				void setValue(T value) {
					currentValue = value;
				}

				T getValue() const {
					return currentValue;
				}

				void commit() override;

				UniformProxyBase* createProxy() override {
					return new UniformProxy<T>;
				}

			protected:
				T currentValue{ 0 };
				T previousValue{ 0 };
			};

			template <>
			class MappedUniform<Texture*> : public MappedUniformBase {

			public:
				const uint32_t txu;
				
				MappedUniform(uint32_t txu) :
					txu(txu) {
				}

				void setValue(Texture* value) {
					currentValue = value;
				}

				Texture* getValue() const {
					return currentValue;
				}

				void commit() override;

				UniformProxyBase* createProxy() override {
					return new UniformProxy<Texture*>;
				}

			protected:
				Texture* currentValue{ nullptr };
				Texture* previousValue{ nullptr };
			};

			template <>
			class MappedUniform<Texture2D*> : public MappedUniform<Texture*> {

			public:
				MappedUniform(uint32_t txu) :
					MappedUniform<Texture*>(txu) {
				}

				static std::type_index staticType() {
					return OXFEDE_TYPE(MappedUniform<Texture2D*>);
				}

				Texture2D* getValue() const {
					return static_cast<Texture2D*>(currentValue);
				}

				UniformProxyBase* createProxy() override {
					return new UniformProxy<Texture2D*>;
				}
			};

			template <>
			class MappedUniform<Texture3D*> : public MappedUniform<Texture*> {

			public:
				MappedUniform(uint32_t txu) :
					MappedUniform<Texture*>(txu) {
				}

				static std::type_index staticType() {
					return OXFEDE_TYPE(MappedUniform<Texture2D*>);
				}

				Texture3D* getValue() const {
					return static_cast<Texture3D*>(currentValue);
				}

				UniformProxyBase* createProxy() override {
					return new UniformProxy<Texture3D*>;
				}
			};
			
			std::unordered_map<std::string, MappedUniformBase*> uniforms;

			OXFEDE_PRIVATE;
		};

	}

}