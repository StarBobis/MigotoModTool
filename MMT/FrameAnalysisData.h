#pragma once
#include <iostream>
#include <vector>

class FrameAnalysisData {
public:
	std::wstring WorkFolder = L"";
	
	std::vector<std::wstring> FrameAnalysisFileNameList;


	FrameAnalysisData(std::wstring workFolder);

	std::vector<std::wstring> FindFrameAnalysisFileNameListWithCondition(std::wstring searchStr, std::wstring endStr);
	std::vector<std::wstring> ReadTrianglelistIndexList(std::wstring drawIB);
	std::vector<std::wstring> ReadPointlistIndexList();

};