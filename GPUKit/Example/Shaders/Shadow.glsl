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

#GPUKIT_VERTEX_STAGE
#version 410 core

layout (location = 0) in vec3 position;

struct Geometry {
  mat4 M;
};

uniform Geometry geometry;

void main() {
  gl_Position =  geometry.M * vec4(position, 1.);
}
#GPUKIT_END_STAGE

#GPUKIT_GEOMETRY_STAGE
#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

out vec4 fragPos;

struct Light {
  vec3 position;
  float planeF;
  mat4 PV[6];
};

uniform Light light;

void main() {
  for (int fIdx = 0; fIdx < 6; fIdx++) {
    gl_Layer = fIdx;
    
    for (int vIdx = 0; vIdx < 3; vIdx++) {
      fragPos = gl_in[vIdx].gl_Position;
      gl_Position = light.PV[fIdx] * fragPos;
      EmitVertex();
    }
    
    EndPrimitive();
  }
}
#GPUKIT_END_STAGE

#GPUKIT_FRAGMENT_STAGE
#GPUKIT_ENABLE depth
#version 410 core

in vec4 fragPos;

struct Light {
  vec3 position;
  float planeF;
  mat4 PV[6];
};

uniform Light light;

void main() {
  gl_FragDepth = (distance(fragPos.xyz, light.position)) / light.planeF;
}
#GPUKIT_END_STAGE