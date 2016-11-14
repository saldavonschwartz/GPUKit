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

#include <iostream>
using std::cout;

#include <typeindex>
#define OXFEDE_TYPE(X) std::type_index(typeid(X))

#include <memory>
#define OXFEDE_PRIVATE \
private:\
struct _Private; \
std::unique_ptr<_Private> _private

#define OXFEDE_PRIVATE_STATIC \
private:\
struct _Private; \
static std::unique_ptr<_Private> _private

#define OXFEDE_FLAG(A, T, F)\
enum class A : T;\
F inline A operator|(A lhs, A rhs) {\
return static_cast<A>(static_cast<T>(lhs) | static_cast<T>(rhs));\
}\
F inline A& operator|=(A& lhs, A rhs) {\
return (lhs = lhs | rhs);\
}\
F inline A operator&(A lhs, A rhs) {\
return static_cast<A>(static_cast<T>(lhs) & static_cast<T>(rhs));\
}\
F inline A& operator&=(A& lhs, A rhs) {\
return (lhs = lhs & rhs);\
}\
F inline bool isSet(A lhs, A rhs) {\
return (bool)(static_cast<T>(lhs & rhs) != 0);\
}\
F inline A operator~(A target) {\
return static_cast<A>(~static_cast<T>(target));\
}\
F inline T getUnderlyingValue(A target) {\
return static_cast<T>(target);\
}\
enum class A : T
