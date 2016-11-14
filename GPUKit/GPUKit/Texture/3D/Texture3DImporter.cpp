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

#include "Texture3DImporter.h"

#include "../GPUKit/Logger.h"

//	one per project: #define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <experimental/filesystem>
#include <unordered_map>
#include <cassert>
#include <algorithm>

using namespace OXFEDE::GPUKit;
using namespace std;
using experimental::filesystem::directory_iterator;

static unordered_map<string, Texture3D*> cached;

Texture3D* AssetImporter<Texture3D*>::import(const string& path, Texture3D::Data data, bool flipped) {	
	static AssetImporter<Texture3D*>* logDummy = nullptr;
	
	OXFEDE_LOG(LType::I, GPUKIT::General, logDummy, 
		"-- import: path: %s --",
		path.c_str());

	auto found = cached.find(path);
	if (found != cached.end()) {
		OXFEDE_LOG(LType::I, GPUKIT::General, logDummy, 
			"-- return cached --");
		
		return found->second;
	}

	int width;
	int height;
	int componentCount;

	int i = 0;
	for (auto& entry : directory_iterator(path)) {
		const auto path = entry.path().string();

		stbi_set_flip_vertically_on_load(flipped);
		uint8_t* bytes = stbi_load(path.c_str(), &width, &height, &componentCount, STBI_rgb);

		data.image[i].bytes = bytes;
		data.image[i].size = { width, height };
		i++;

		OXFEDE_LOG(LType::I, GPUKIT::Importer, logDummy, 
			"-- ============ STATS: ============== --");

		OXFEDE_LOG(LType::I, GPUKIT::Importer, logDummy, 
			"-- size: (%i, %i) | flipped: %s --", 
			width, height, flipped ? "yes" : "no");
	}

	OXFEDE_LOG(LType::I, GPUKIT::Importer, logDummy, 
		"-- ------------ END --------------- --");

	Texture3D* texture = new Texture3D(data);
	cached[path] = texture;

	return texture;
}

void AssetImporter<Texture3D*>::release(Texture3D* asset) {
	assert(asset);

	auto found = std::find_if(cached.begin(), cached.end(), [asset](const auto& pair) {
		return pair.second == asset;
	});

	assert(found != cached.end());

	for (Texture::Image& image : asset->data.image) {
		if (image.bytes) {
			delete image.bytes;
		}
	}

	cached.erase(found);
	delete asset;
}
