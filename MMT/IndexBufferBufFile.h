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

	//总共有几个索引
	uint32_t NumberCount = -1;

	//总共用到的独立顶点数量
	uint32_t UniqueVertexCount = -1;

	//顶点列表
	std::vector<uint32_t> NumberList;

	//IB文件完整路径，不区分大小写的格式例如DXGI_FORMAT_R32_UINT
	IndexBufferBufFile(std::wstring FileReadPath,std::wstring Format);

	void SelfDivide(int FirstIndex,int IndexCount);

	void SaveToFile_UINT32(std::wstring FileWritePath, uint32_t Offset);
};