#include "IndexBufferBufFile.h"
#include <boost/algorithm/string.hpp>
#include <istream>
#include <fstream>


IndexBufferBufFile::IndexBufferBufFile(std::wstring FileReadPath, std::wstring Format) {
    std::wstring lowerIBReadDxgiFormat = boost::algorithm::to_lower_copy(Format);
    int readLength = -1;
    if (lowerIBReadDxgiFormat == L"dxgi_format_r16_uint") {
        readLength = 2;
    }
    else if (lowerIBReadDxgiFormat == L"dxgi_format_r32_uint") {
        readLength = 4;
    }
    std::ifstream ReadIBFile(FileReadPath, std::ios::binary);

    char* data = new char[readLength];

    int tmpMinNumber = 9999999;
    int tmpMaxNumber = 0;
    
    int readCount = 0;
    while (ReadIBFile.read(data, readLength)) {
        int number = 0;
        if (lowerIBReadDxgiFormat == L"dxgi_format_r16_uint") {
            std::uint16_t value = (static_cast<unsigned char>(data[1]) << 8) |
                static_cast<unsigned char>(data[0]);
            number = value;
        }

        if (lowerIBReadDxgiFormat == L"dxgi_format_r32_uint") {
            std::uint32_t value = (static_cast<unsigned char>(data[3]) << 24) |
                (static_cast<unsigned char>(data[2]) << 16) |
                (static_cast<unsigned char>(data[1]) << 8) |
                static_cast<unsigned char>(data[0]);
            number = value;
        }

        if (number < tmpMinNumber) {
            tmpMinNumber = number;
        }

        if (number > tmpMaxNumber) {
            tmpMaxNumber = number;
        }

        this->NumberList.push_back(number);

        readCount++;
    }
    ReadIBFile.close();

    this->MaxNumber = tmpMaxNumber;
    this->MinNumber = tmpMinNumber;
    this->NumberCount = readCount;
}


void IndexBufferBufFile::SaveToFile_UINT32(std::wstring FileWritePath, uint32_t Offset) {
    std::ofstream file(FileWritePath, std::ios::binary);
    for (const auto& data : this->NumberList) {
        uint32_t paddedData = data;
        paddedData = data + Offset;
        file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint32_t));
    }
    file.close();
}