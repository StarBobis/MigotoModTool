#pragma once
#include "FmtData.h"
#include "VertexBufferTxtFile.h"


class SplitUtil {
public:
	GlobalConfigs wheelConfig;
	ExtractConfig basicConfig;
	D3D11GameType d3d11GameType;

	std::wstring splitReadFolder;
	std::wstring splitOutputFolder;

	//读取，然后替换COLOR、TANGENT，然后按类型分割放到这个map里
	std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;

	// TODO 为了KDTree算法，我们需要读取时根据partName先暂时分开读取，在计算完成nearest_original之后，
	// 再依次组合放到finalVBCategoryDataMap里
	std::unordered_map<std::string, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;
	
	//需要一个partName,offset的map供后面ib读取使用
	std::unordered_map<std::wstring, uint64_t> partNameOffsetMap;

	std::vector<std::string> CategoryList;
	std::unordered_map<std::string, int> CategoryStrideMap;

	int blendElementByteWidth = 0;
	uint64_t drawNumber = 0;

	//一个partName，UUID的map供后面ib文件名使用
	std::unordered_map<std::string, std::string> partNameUUIDMap;
	//一个Category，UUID的map供后面buf文件名使用
	std::unordered_map<std::string, std::string> categoryUUIDMap;

	void readSplitRecalculate();

	void outputModFiles();

	SplitUtil();
	SplitUtil(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType);
};