#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>

class IndexBufferTxtFile {
	public:
		//Ö÷Òª×Ö¶Î
		std::wstring FileName;
		std::wstring Index;
		std::wstring Hash;
		std::wstring FirstIndex = L"";
		std::wstring IndexCount;
		std::wstring Topology;
		std::wstring Format;
		std::wstring ByteOffset;

		uint32_t MaxNumber = 0;
		uint32_t MinNumber = 99999999;

		int ib_read_number = 0;
		//Ô¤Áô×Ö¶Î
		//std::wstring ByteOffset;
		//std::wstring IndexCount;
		//std::wstring Format;
		std::unordered_map<int, std::vector<std::byte>> IBTxtToBufMap;

		IndexBufferTxtFile();

		IndexBufferTxtFile(std::wstring fileName, bool readIBData);
};