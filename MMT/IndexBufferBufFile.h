#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

class IndexBufferBufFile {
public:

	//最小的顶点数值
	uint32_t MinNumber = -1;

	//最大的顶点数值
	uint32_t MaxNumber = -1;

	//总共有几个顶点
	uint32_t NumberCount = -1;

	//顶点列表
	std::vector<uint32_t> NumberList;


	IndexBufferBufFile(std::wstring FileReadPath,std::wstring Format);

	void SaveToFile_UINT32(std::wstring FileWritePath, uint32_t Offset);
};