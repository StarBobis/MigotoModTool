#pragma once
#include <cstdint>
#include <map>
#include <iostream>

class ConstantBufferValue {
public:
	uint32_t X;
	uint32_t Y;
	uint32_t Z;
	uint32_t W;
};


class ConstantBufferBufFile {
public:
	std::map<int, ConstantBufferValue> lineCBValueMap;

	ConstantBufferBufFile();
	ConstantBufferBufFile(std::wstring CBFilePath);
};