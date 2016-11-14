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

#include "ProgramImporter.h"

#include "Program.h"
#include "../Shader/ShaderImporterCommon.h"
#include "../Shader/VShader/VShaderImporter.h"
#include "../Shader/GShader/GShaderImporter.h"
#include "../Shader/FShader/FShaderImporter.h"
#include "../GPUKit/Logger.h"

#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <cassert>

using namespace OXFEDE::GPUKit;
using namespace std;

static unordered_map<string, Program*> cached;

Program* AssetImporter<Program*>::import(const string& path) {
	static AssetImporter<Program*>* logDummy = nullptr;

	OXFEDE_LOG(LType::I, GPUKIT::General, logDummy, 
		"-- import: path: %s --", path.c_str());

	auto found = cached.find(path);
	if (found != cached.end()) {
		OXFEDE_LOG(LType::I, GPUKIT::General, logDummy, 
			"-- return cached --");

		return found->second;
	}

	Program* program = new Program;
	smatch tokens;

	ifstream file(path);
	assert(file);

	stringstream input;
	input << file.rdbuf();
	file.close();

	string line;
	std::streamoff length = 0;
	for (string line; getline(input, line, '\n');) {
		if (regex_match(line, tokens, SIC::STAGE_START)) {
			auto stage = tokens[1];
			length = tokens[0].length();
			input.seekg(-length, std::ios_base::cur);

			if (stage == "VERTEX") {
				program->vStage = AssetImporter<VShader*>::import(input);
			}
			else if (stage == "GEOMETRY") {
				program->gStage = AssetImporter<GShader*>::import(input);
			}
			else if (stage == "FRAGMENT") {
				program->fStage = AssetImporter<FShader*>::import(input);
			}
			else {
				abort();
			}
		}
	}

	cached[path] = program;
	return program;
}

void AssetImporter<Program*>::release(Program* asset) {
	auto found = std::find_if(cached.begin(), cached.end(), [asset](const auto& pair) {
		return pair.second == asset;
	});

	assert(found != cached.end());

	cached.erase(found);
	delete asset;
}