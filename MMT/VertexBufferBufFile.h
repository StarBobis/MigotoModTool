#pragma once
#include <iostream>
#include "D3d11GameType.h"


class VertexBufferBufFile {
public:
	std::unordered_map<std::wstring, std::vector<std::byte>> CategoryVBDataMap;

	VertexBufferBufFile();
	
	VertexBufferBufFile(std::wstring readVBBufFilePath,D3D11GameType d3d11GameType,std::vector<std::string> elementList);
};