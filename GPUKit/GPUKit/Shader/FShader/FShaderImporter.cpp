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

#include "FShaderImporter.h"

#include "../GPUKit/Shader/ShaderImporterCommon.h"
#include "../GPUKit/Shader/FShader/FShader.h"
#include "../GPUKit/Logger.h"

#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <cassert>

using namespace OXFEDE::GPUKit;
using namespace std;

static unordered_map<string, FShader*> cached;

FShader* AssetImporter<FShader*>::import(const string& path) {
	static AssetImporter<FShader>* logDummy = nullptr;
	
	OXFEDE_LOG(LType::I, LGPK::General, logDummy, 
		"-- import: path: %s --", path.c_str());

	auto found = cached.find(path);
	if (found != cached.end()) {
		OXFEDE_LOG(LType::I, LGPK::General, logDummy, 
			"-- return cached --");
		
		return found->second;
	}

	ifstream file(path);
	assert(file);

	stringstream input;
	input << file.rdbuf();
	file.close();

	auto shader = import(input);
	cached[path] = shader;
	return shader;
}

FShader* AssetImporter<FShader*>::import(stringstream& input) {
	FShader* shader = new FShader;
	SIC::StructMap structs;
	vector<string> structIds;
	smatch tokens;

	string line;
	getline(input, line, '\n');
	if (regex_match(line, tokens, SIC::STAGE_START)) {
		auto stage = tokens[1];
		assert(stage == "FRAGMENT");
	}

	for (string line; getline(input, line, '\n');) {
		if (regex_match(line, tokens, SIC::STAGE_END)) {
			break;
		}
		else if (regex_match(line, tokens, SIC::FLAG)) {
			if (tokens[1] == "depth") {
				SIC::AttrDescriptor descriptor = { Buffer::Target::DEPTH, SIC::AttrDescriptor::Type::Out, "int", tokens[1] };
				SIC::attributeFromDescriptor(descriptor, shader, structs);
			}

			continue;
		}

		shader->source += line + "\n";

		if (regex_match(line, tokens, SIC::STRUCT_START)) {
			structIds.push_back(tokens[1]);
		}
		else if (structIds.size() && regex_match(line, tokens, SIC::STRUCT_MEMBER)) {
			string countString = tokens[3];
			int count = countString.length() ? std::stoi(countString) : 0;

			if (count) {
				for (int i = 0; i < count; i++) {
					string name = tokens[2];
					name += "[" + std::to_string(i) + "]";
					structs[structIds.back()].push_back({ tokens[1], name });
				}
			}
			else {
				structs[structIds.back()].push_back({ tokens[1], tokens[2] });
			}
		}
		else if (structIds.size() && regex_match(line, tokens, SIC::STRUCT_END)) {
			structIds.pop_back();
		}
		else if (regex_match(line, tokens, SIC::UNIFORM)) {
			string countString = tokens[3];
			int count = countString.length() ? std::stoi(countString) : 0;

			if (count) {
				for (int i = 0; i < count; i++) {
					string name = tokens[2];
					name += "[" + std::to_string(i) + "]";
					SIC::AttrDescriptor descriptor = { -1, SIC::AttrDescriptor::Type::Uniform, tokens[1], name };
					SIC::attributeFromDescriptor(descriptor, shader, structs);
				}
			}
			else {
				SIC::AttrDescriptor descriptor = { -1, SIC::AttrDescriptor::Type::Uniform, tokens[1], tokens[2] };
				SIC::attributeFromDescriptor(descriptor, shader, structs);
			}
		}
		else if (regex_match(line, tokens, SIC::OUT)) {
			string MRTOffset = tokens[1];
			int location;

			if (MRTOffset.length()) {
				location = Buffer::Target::COLOR0 + std::stoi(MRTOffset);
			}
			else {
				location = Buffer::Target::COLOR0;
			}

			SIC::AttrDescriptor descriptor = { location, SIC::AttrDescriptor::Type::Out, tokens[2], tokens[3] };
			SIC::attributeFromDescriptor(descriptor, shader, structs);
		}
	}

	static AssetImporter<FShader>* logDummy = nullptr;
	
	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- ============ STATS: ============== --");
	
	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- outputs: --");
	
	for (auto output : shader->outputs) {
		OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
			"%s: %i", output.first.c_str(), output.second->location);
	}

	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"");
	
	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- uniforms: --");
	
	for (auto uniform : shader->uniforms) {
		OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
			"%s", uniform.first.c_str());
	}

	OXFEDE_LOG(LType::I, LGPK::Importer, logDummy, 
		"-- ------------ END --------------- --");

	return shader;
}

void AssetImporter<FShader*>::release(FShader* asset) {
	auto found = std::find_if(cached.begin(), cached.end(), [asset](const auto& pair) {
		return pair.second == asset;
	});

	assert(found != cached.end());

	cached.erase(found);
	delete asset;
}