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
#pragma debug(on)

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tx0;

struct VStage {
  vec3 position;
  vec3 normal;
  vec2 tx0;
};

out VStage vStage;

struct Geometry {
  mat4 M;
  mat3 N;
};

uniform Geometry geometry;

struct Camera {
  mat4 P;
  mat4 V;
  vec3 position;
};

uniform Camera camera;

void main() {
  vec4 worldPosition = geometry.M * vec4(position, 1.);
  
  vStage.position = worldPosition.xyz;
  vStage.normal = geometry.N * normal;
  vStage.tx0 = tx0;
  
  gl_Position = camera.P * camera.V * worldPosition;
}
#GPUKIT_END_STAGE

#GPUKIT_FRAGMENT_STAGE
#GPUKIT_ENABLE depth
#version 410 core
#pragma debug(on)

struct VStage {
  vec3 position;
  vec3 normal;
  vec2 tx0;
};

in VStage vStage;

layout (location = 0) out vec3 outP;
layout (location = 1) out vec3 outN;
layout (location = 2) out vec4 outC;
layout (location = 3) out vec4 outE;


struct MProperty3 {
  sampler2D map;
  vec3 color;
  bool usesMap;
};

struct MProperty1 {
  sampler2D map;
  float value;
  bool usesMap;
};

struct Material {
  MProperty3 emission;
  MProperty3 diffuse;
  MProperty1 specular;
};

uniform Material material;

void main() {
  vec3 diffuse = material.diffuse.color;
  if(material.diffuse.usesMap) {
    diffuse *= texture(material.diffuse.map, vStage.tx0).rgb;
  }
  
  float specular = material.specular.value;
  if (material.specular.usesMap) {
    specular *= texture(material.diffuse.map, vStage.tx0).r;
  }
  
  vec4 emission = vec4(material.emission.color, 1.);
  if (material.emission.usesMap) {
    emission.a = texture(material.emission.map, vStage.tx0).r;
  }
  
  vec4 diffuseSpecular = vec4(diffuse, specular);
  
  outP = vStage.position;
  outN = normalize(vStage.normal);
  outC = diffuseSpecular;
  outE = emission;
}
#GPUKIT_END_STAGE
