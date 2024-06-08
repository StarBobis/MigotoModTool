#pragma once
#include "GlobalConfigs.h"
#include "D3d11GameType.h"

class FmtFileData {
public:
	int Stride = 0;
	std::wstring Topology = L"";

	std::vector<D3D11Element> d3d11ElementList;


	//下面这三个属性是如果你要手动拼接一个FMT文件并输出时必须设置的东西
	std::wstring Format = L"";
	D3D11GameType d3d11GameType;
	std::vector<std::string> ElementNameList;


	FmtFileData();
	FmtFileData(std::wstring readFmtPath);
	void OutputFmtFile(std::wstring OutputFmtPath);
};


