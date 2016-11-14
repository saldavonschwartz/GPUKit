#GPUKIT_VERTEX_STAGE
#version 410 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 tx0;

struct VStage {
  vec2 tx0;
};

out VStage vStage;

void main() {
  vStage.tx0 = tx0;
  gl_Position = vec4(position.xy, 0., 1.);
}
#GPUKIT_END_STAGE

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

#GPUKIT_FRAGMENT_STAGE
#version 410 core

struct VStage {
  vec2 tx0;
};

in VStage vStage;

out vec4 outC;

uniform sampler2D wet;
uniform sampler2D dry;

void main() {
  vec3 wetC = texture(wet, vStage.tx0).rgb;
  vec3 dryC = texture(dry, vStage.tx0).rgb;
  vec3 color = wetC + dryC;

  outC = vec4(color, 1.);
}
#GPUKIT_END_STAGE