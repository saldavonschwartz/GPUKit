#include "../GPUKit/GPUKit.h"
#include "../Example/Scene/NodeImporter.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext.hpp>
#include <iostream>

#include "../GPUKit/Logger.h"
#include "../Example/Scene/HighLevelTypes.h"
#include "../Example/Passes/GeometryPass.h"
#include "../Example/Passes/LightPass.h"
#include "../Example/Passes/BlurPass.h"
#include "../Example/Passes/SkyboxPass.h"
#include "../Example/Passes/ShadowPass.h"
#include "../Example/Passes/BlendPass.h"

using namespace OXFEDE;
using namespace OXFEDE::GPUKit;
using namespace std;


void scaleNode(Node* node, mat4 scale) {
	node->M *= scale;
	for (Node* child : node->children) {
		scaleNode(child, scale);
	}
}

void computeWorldSpaceAndGetMonkeys(Node* node, mat4 M, Node*& monkeyR, Node*& monkeyL) {
	M *= node->M;
	node->M = M;

	if (node->name == "MonkeySmooth") {
		monkeyR = node;
	}
	if (node->name == "MonkeyFlat") {
		monkeyL = node;
	}

	for (Node* child : node->children) {
		computeWorldSpaceAndGetMonkeys(child, M, monkeyR, monkeyL);
	}
}

int main() {
	uvec2 screenSize = { 1024, 768 };
	GPUKit::init(screenSize, { 4, 1 });

	{
		int w;
		int h;
		glfwGetFramebufferSize(GPUKit::window, &w, &h);
		glEnable(GL_CULL_FACE);

		glClearColor(0.f, 0.f, 0.f, 1.f);

		Node* scene = AssetImporter<Node*>::import("Example/Models/Scene.fbx");
		Node* lightsNode{};
		Texture2D* txFloor = nullptr;

		for (Node* node : scene->children[0]->children) {
			if (node->name == "Lights") {
				lightsNode = node;
			}
			else if (node->name == "Plane") {
				Texture2D::Data data;
				txFloor = AssetImporter<Texture2D*>::import("Example/Models/skybox/badomen5.tga", data, false);
				txFloor->commit();

				node->material->setAttribute("diffuse.usesMap", txFloor);
				node->material->setAttribute("diffuse.map", txFloor);
			}
		}

		Node* suit = AssetImporter<Node*>::import("Example/Models/nanosuit/nanosuit.obj");
		scaleNode(suit, glm::scale(mat4{}, { .2, .2, .2 }));
		suit->M = glm::translate(vec3{ 0., 0., -1. });
		scene->children.push_back(suit);

		Camera camera;
		camera.P = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 1000.f);

		Node cameraNode;
		cameraNode.M = glm::translate(mat4{}, { 0., 2.4, 7. });
		cameraNode.camera = &camera;

		Node* lightNode = lightsNode->children[0];
		Light* lightR = new Light;
		lightNode->light = lightR;
		lightR->color = vec3{ 1., 0.22, 0.1 };
		lightR->attenuation = 1.;
		lightR->intensity = 4.f;
		lightNode->material->setAttribute("emission.color", lightR->color * 4.f);
		lightNode->M = translate(lightNode->M, { 0., 0., 5. });

		lightNode = lightsNode->children[1];
		Light* lightL = new Light;
		lightNode->light = lightL;
		lightL->color = vec3{ .2, 1., 0.5 };
		lightL->attenuation = 1.;
		lightL->intensity = 2.f;
		lightNode->material->setAttribute("emission.color", lightL->color);

		Node* monkeyR = nullptr;
		Node* monkeyL = nullptr;
		computeWorldSpaceAndGetMonkeys(scene, {}, monkeyR, monkeyL);

		Texture2D::Data data;

		data.image.sourceformat = GL_RGB16F;
		data.image.targetFormat = GL_RGB;
		data.image.pixelType = GL_FLOAT;
		data.image.size = screenSize;

		Texture2D* txPosition = new Texture2D(data);
		txPosition->commit();

		Texture2D* txNormal = new Texture2D(data);
		txNormal->commit();

		data.image.sourceformat = GL_RGBA;
		data.image.targetFormat = GL_RGBA;
		data.image.pixelType = GL_UNSIGNED_BYTE;

		Texture2D* txColor = new Texture2D(data);
		txColor->commit();

		Texture2D* txEmission = new Texture2D(data);
		txEmission->commit();

		Texture2D* txTemp1FullRes = new Texture2D(data);
		txTemp1FullRes->commit();

		Texture2D* txTemp2FullRes = new Texture2D(data);
		txTemp2FullRes->commit();

		data.image.sourceformat = GL_DEPTH_COMPONENT32F;
		data.image.targetFormat = GL_DEPTH_COMPONENT;
		data.image.pixelType = GL_FLOAT;
		data.image.size = screenSize;

		Texture2D* txDepth2D = new Texture2D(data);
		txDepth2D->commit();

		Texture3D::Data data3D;

		data.image.sourceformat = GL_DEPTH_COMPONENT;
		data.image.targetFormat = GL_DEPTH_COMPONENT;
		data.image.pixelType = GL_UNSIGNED_BYTE;

		for (int i = 0; i < 6; i++) {
			data3D.image[i] = data.image;
			data3D.image[i].size = { 1024, 1024 };
		}

		Texture3D* txDepth3D = new Texture3D(data3D);
		txDepth3D->commit();

		data.image.sourceformat = GL_RGB;
		data.image.targetFormat = GL_RGB;
		data.image.pixelType = GL_UNSIGNED_BYTE;

		for (int i = 0; i < 6; i++) {
			data3D.image[i] = data.image;
		}

		Texture3D* txSkybox = AssetImporter<Texture3D*>::import("Example/Models/skybox/", data3D, false);
		txSkybox->commit();

		ShadowPass shadowPass;
		shadowPass.setOutput("depth", txDepth3D);
		shadowPass.scene = scene;
		shadowPass.lightsNode = lightsNode;

		GeometryPass geometryPass;
		geometryPass.scene = scene;
		geometryPass.cameraNode = &cameraNode;
		geometryPass.setOutput("outP", txPosition);
		geometryPass.setOutput("outN", txNormal);
		geometryPass.setOutput("outC", txColor);
		geometryPass.setOutput("outE", txEmission);
		geometryPass.setOutput("depth", txDepth2D);

		LightPass lightPass;
		lightPass.cameraNode = &cameraNode;
		lightPass.lightsNode = lightsNode;
		lightPass.setInput("inP", txPosition);
		lightPass.setInput("inN", txNormal);
		lightPass.setInput("inC", txColor);
		lightPass.setInput("inE", txEmission);
		lightPass.setInput("shadowMap", txDepth3D);
		lightPass.setOutput("outC", txTemp1FullRes);
		lightPass.setOutput("outBrightness", txTemp2FullRes);

		BlurPass blurPass;
		blurPass.iterations = 8;
		blurPass.setInput("source", txTemp2FullRes);
		blurPass.setOutput("outC", txTemp2FullRes);

		SkyboxPass skyboxPass;
		skyboxPass.texture = txSkybox;
		skyboxPass.cameraNode = &cameraNode;
		skyboxPass.setOutput("outC", txTemp1FullRes);
		skyboxPass.setOutput("depth", txDepth2D);

		BlendPass blendPass;
		blendPass.setInput("dry", txTemp1FullRes);
		blendPass.setInput("wet", txTemp2FullRes);

		Node* lightRNode = lightsNode->children[0];
		Node* lightLNode = lightsNode->children[1];

		double lastTime = glfwGetTime();
		double currentTime = 0;
		double deltaTime = 0;

		double rightBound = lightRNode->M[3][0];
		double leftBound = lightLNode->M[3][0];
		bool done = false;
		float angle = 0.;

		while (true) {
			// compute time:
			currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			// translate a light:
			if (!done && lightRNode->M[3][0] > leftBound) {
				lightRNode->M[3][0] += -0.65f * (float)deltaTime;
			}
			else {
				done = true;
				if (done && lightRNode->M[3][0] < rightBound) {
					lightRNode->M[3][0] += 0.65f * (float)deltaTime;
				}
				else {
					done = false;
				}
			}

			// rotate monkey heads.
			angle = 360.f * 0.005f * (float)deltaTime;
			if (angle >= 360.) {
				angle = 0.;
			}

			monkeyR->M = glm::rotate(monkeyR->M, angle, vec3{ 0., 0., 1. });
			monkeyL->M = glm::rotate(monkeyL->M, angle, vec3{ 0., -1., 1. });

			// render
			shadowPass.render();
			geometryPass.render();
			lightPass.render();
			skyboxPass.render();
			blurPass.render();
			blendPass.render();
			GPUKit::swapBuffers();
			//break;
		}

		delete scene;
		delete txPosition;
		delete txNormal;
		delete txColor;
		delete txEmission;
		delete txDepth2D;
		delete txDepth3D;
		delete txTemp1FullRes;
		delete txTemp2FullRes;
		AssetImporter<Texture3D*>::release(txSkybox);
	}

	shutDown();
	return 0;
}