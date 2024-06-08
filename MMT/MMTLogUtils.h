#pragma once
#include <iostream>
#include "easylogging++.h"

class MMTLogger {
public:
	std::wstring RunResultJsonPath;

	//日志记录功能
	void Info(std::wstring);
	void Info(std::string);

	void Warning(std::wstring);
	void Warning(std::string);

	void Error(std::wstring);
	void Error(std::string);

	void NewLine();
	void Success();

	void SaveResultJson(std::wstring);
	void SaveResultJson(std::string);

	MMTLogger();
	MMTLogger(std::wstring ApplicationLocation);
};

//全局日志类
extern MMTLogger LOG;
