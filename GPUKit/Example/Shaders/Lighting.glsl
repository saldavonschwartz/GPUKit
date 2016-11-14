#GPUKIT_VERTEX_STAGE
#version 410 core
#pragma debug(on)

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 tx0;

struct VStage {
  vec3 position;
  vec3 normal;
  vec2 tx0;
};

out VStage vStage;

void main() {
  vStage.tx0 = tx0;
  gl_Position = vec4(position, 1.);
}
#GPUKIT_END_STAGE

#GPUKIT_FRAGMENT_STAGE
#version 410 core
#pragma debug(on)

struct VStage {
  vec3 position;
  vec3 normal;
  vec2 tx0;
};

in VStage vStage;

layout (location = 0) out vec3 outC;
layout (location = 1) out vec3 outBrightness;

uniform sampler2D inP;
uniform sampler2D inN;
uniform sampler2D inC;
uniform sampler2D inE;
uniform samplerCube shadowMap;

struct Camera {
  vec3 position;
};

uniform Camera camera;

struct Light {
  vec3 position;
  float intensity;
  vec3 color;
  float attenuation;
  float farPlane;
};

const int LCount = 2;

uniform Light light[2];

struct SurfacePoint {
  vec3 position;
  vec3 normal;
  vec4 color;
};

vec3 diffuseI(SurfacePoint s,  Light l) {
  vec3 directionL = normalize(l.position - s.position);
  
  vec3 colorL = l.color * l.intensity;
  return max(0., dot(directionL, s.normal)) * s.color.rgb * colorL;
}

vec3 specularI(SurfacePoint s,  Light l) {
  vec3 directionL = normalize(l.position - s.position);
  vec3 directionC = normalize(camera.position - s.position);
  vec3 reflectionL = normalize(directionL + directionC);
  float sI = max(0., dot(s.normal, reflectionL));
  
  vec3 colorL = l.color * l.intensity;
  return pow(sI, 100.) * s.color.a * colorL;
}

float lightAttenuation(SurfacePoint s, Light l) {
  float lDistance = length(l.position - s.position);
  return 1. / (1. + l.attenuation * lDistance * lDistance);
}

float shadow(SurfacePoint s, Light l, samplerCube map, bool soft) {
  vec3 directionS = s.position - l.position;
  float distanceLtoS = length(directionS);
  float shadowBias = .1;
  float shadow = 0.;
  
  if (!soft) {
    float nearestPoint = texture(map, directionS).r * l.farPlane;
    shadow += 0.7 * step(distanceLtoS, nearestPoint + shadowBias);
    return shadow;
  }
  
  float samples = 4.;
  float offset = .05;
  float sampleStep = offset / (samples * .5);
  
  for(float x = -offset; x < offset; x += sampleStep) {
    for(float y = -offset; y < offset; y += sampleStep) {
      float nearestPoint = texture(map, directionS + vec3(x, y, 0.)).r * l.farPlane;
      shadow += 0.7 * step(distanceLtoS, nearestPoint + shadowBias);
    }
  }
  
  shadow /= (samples * samples);
  return shadow;
}

vec3 gammaCorrect(vec3 source, float gamma) {
  return pow(source, vec3(1. / gamma));
}

vec3 toneMapping(vec3 source, float exposure) {
  return vec3(1.) - exp(-source * exposure);
}

void main() {
  vec3 position = texture(inP, vStage.tx0).rgb;
  vec3 normal = normalize(texture(inN, vStage.tx0).rgb);
  vec4 color = texture(inC, vStage.tx0);
  vec4 emission = texture(inE, vStage.tx0);
  
  SurfacePoint surface;
  surface.position = position;
  surface.normal = normal;
  surface.color = color;
 
  vec3 I;
  for (int lIdx = 0; lIdx < LCount; lIdx++) {
    I += lightAttenuation(surface, light[lIdx]) * (diffuseI(surface, light[lIdx]) + specularI(surface, light[lIdx]));
  }
  
  float shadow = shadow(surface, light[0], shadowMap, true) + 0.2;
  
  I += emission.rgb * emission.a;
  I *= shadow;
  
  I = toneMapping(I, 1.2);
  I = gammaCorrect(I, 2.2);
  
  outC = I;
  
  float brightness = dot(I, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > .2) {
    outBrightness = I;
  }
}
#GPUKIT_END_STAGE
