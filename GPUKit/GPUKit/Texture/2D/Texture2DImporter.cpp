
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

#include "Texture2DImporter.h"

#include "../GPUKit/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <unordered_map>
#include <cassert>
#include <algorithm>

using namespace OXFEDE::GPUKit;
using namespace std;

static unordered_map<string, Texture2D*> cached;

Texture2D* AssetImporter<Texture2D*>::import(const string& path, Texture2D::Data data, bool flipped) {
	static AssetImporter<Texture2D*>* logDummy = nullptr;
	
	OXFEDE_LOG(LType::I, LGPK::General, logDummy, 
		"-- import: path: %s --", 
		path.c_str());

	auto found = cached.find(path);
	if (found != cached.end()) {
		OXFEDE_LOG(LType::I, LGPK::General, logDummy, 
			"-- return cached asset for path --");
		
		return found->second;
	}

	int width;
	int height;
	int componentCount;

	stbi_set_flip_vertically_on_load(flipped);
	uint8_t* bytes = stbi_load(path.c_str(), &width, &height, &componentCount, STBI_rgb);

	OXFEDE_LOG( bytes ? LType::I : LType::E, LGPK::General, logDummy, 
		"-- bytes = %p --", 
		bytes);
	
	assert(bytes);

	data.image.bytes = bytes;
	data.image.size = { width, height };

	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- ============ STATS: ============== --");
	
	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- size: (%i, %i)\nflipped: %s --", 
		width, height, flipped ? "yes" : "no");
	
	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- ------------ END --------------- --");

	Texture2D* texture = new Texture2D(data);
	cached[path] = texture;

	return texture;
}

void AssetImporter<Texture2D*>::release(Texture2D* asset) {
	assert(asset);

	auto found = std::find_if(cached.begin(), cached.end(), [asset](const auto& pair) {
		return pair.second == asset;
	});

	assert(found != cached.end());

	if (asset->data.image.bytes) {
		delete asset->data.image.bytes;
	}

	cached.erase(found);
	delete asset;
}

