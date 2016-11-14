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
#pragma warning(disable:4996)

#include "../Dependencies/Defines.h"

#include <cstdio>
#include <string>
#include <ctime>
#include <mutex>
#include <functional>
#include <chrono>
#include <cassert>
#include <deque>

namespace OXFEDE {

	OXFEDE_FLAG(LType, uint8_t,) {
		I = 1 << 0,
		W = 1 << 1,
		E = 1 << 2
	};

	template <class SubT>
	class Logger;

	class LoggerTaskManager {
	
		template <class SubT>
		friend class Logger;

	protected:
		static LoggerTaskManager& getInstance() {
			static LoggerTaskManager manager;
			return manager;
		}

		std::thread* thread;
		std::mutex logLock;
		std::deque<std::function<FILE*()>> logQueue;

		LoggerTaskManager();
		virtual ~LoggerTaskManager();
		void enqueue(std::function<FILE*()>&& task);
	};

	template <class SubT>
	class Logger {

	public:
		std::string name;
		bool enabled{ true };
		bool logTime{ true };
		LType logTypes{ LType::I | LType::W | LType::E };
		SubT fileLog;
		SubT screenLog;
		bool useBackgroundThread;

		Logger(std::string name, std::string logFileName, bool useBackgroundThread) {
			auto error = fopen_s(&file, logFileName.c_str(), "w");
			assert(!error);
			this->name = name;
			this->useBackgroundThread = useBackgroundThread;
		}

		~Logger() {
			fclose(file);
			file = nullptr;
		}

		template <class T = void, class... ParamTs>
		void log(LType logType, SubT subType, T* sender, std::string str, ParamTs... args) {
			auto eventTime = getTimeString();

			if (!enabled) {
				return;
			}

			std::string logTypeString;

			bool logTypeEnabled = isSet(logTypes, logType);
			if (logTypeEnabled) {
				logTypeString = logType == LType::I ? "[I]" : logType == LType::W ? "[W]" : "[E]";
			}
			else {
				return;
			}

			bool logToFile = isSet(fileLog, subType);
			bool logToScreen = isSet(screenLog, subType);

			if (!logToFile && !logToScreen) {
				return;
			}

			std::string logString = "\n";

			if (name.length()) {
				logString += name + "::";
			}

			logString += logTypeString + "[" + getSenderTypeString<T>() + ": %p" + "]";

			if (logTime) {
				logString += "<" + eventTime + ">";
			}

			logString += " " + str;

			char buffer[1024];
			snprintf(buffer, 1024, logString.c_str(), sender, args...);
			logString = std::string(buffer);

			auto logTask = [=, this]() {
				if (logToFile) {
					fprintf(stderr, logString.c_str());
				}

				if (logToScreen) {
					fprintf(file, logString.c_str());
				}

				return file;
			};

			if (useBackgroundThread) {
				LoggerTaskManager::getInstance().enqueue(logTask);
			}
			else {
				fflush(logTask());
			}
		}

	private:
		FILE* file{ nullptr };
	
		std::string getTimeString() {
			std::time_t rawtime;
			struct tm * timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			std::string time(asctime(timeinfo));
			time = time.substr(0, time.length() - 1);

			return time;
		}

		template <class T>
		std::string getSenderTypeString() {
			std::string senderType = typeid(T).name();
			removeAllOcurrences(senderType, "class ");
			removeAllOcurrences(senderType, "struct ");
			removeAllOcurrences(senderType, "__cdecl");
			return senderType;
		}

		void removeAllOcurrences(std::string& target, std::string pattern) {
			auto i = target.find(pattern);
			while (i != std::string::npos) {
				target.erase(i, pattern.length());
				i = target.find(pattern, i);
			}
		}
	};

}