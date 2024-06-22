#include "IndexBufferBufFile.h"
#include <boost/algorithm/string.hpp>
#include <istream>
#include <fstream>
#include <set>


IndexBufferBufFile::IndexBufferBufFile(){

}


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
    std::set<uint32_t> uniqueNumberSet;

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

        uniqueNumberSet.insert(number);

        this->NumberList.push_back(number);

        readCount++;
    }
    ReadIBFile.close();

    this->MaxNumber = tmpMaxNumber;
    this->MinNumber = tmpMinNumber;
    this->NumberCount = readCount;
    this->UniqueVertexCount = (uint32_t)uniqueNumberSet.size();
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


void IndexBufferBufFile::SelfDivide(int FirstIndex, int IndexCount) {
    //根据传入的索引和数量，截取出需要其中一部分IB值
    
    std::vector<uint32_t> newNumberList;
    std::set<uint32_t> uniqueNumberSet;
    int subCount = 0;
    uint32_t tmpMinNumber = 999999999;
    //uint32_t不能为负数，否则会变为uint32_t范围内的最大值，所以这里设为0
    uint32_t tmpMaxNumber = 0;
    for (int i = 0; i < this->NumberList.size(); i++) {

        if (i >= FirstIndex) {
            uint32_t number = this->NumberList[i];
            newNumberList.push_back(number);
            uniqueNumberSet.insert(number);
            subCount++;

            if (number < tmpMinNumber) {
                tmpMinNumber = number;
            }
            if (number > tmpMaxNumber) {
                tmpMaxNumber = number;
            }

            //读取指定数量后退出
            if (subCount == IndexCount) {
                break;
            }
        }
        
    }

    //并重新设置MinNumber MaxNumber NumberCount NumberList
    this->MinNumber = tmpMinNumber;
    this->MaxNumber = tmpMaxNumber;
    this->NumberCount = subCount;
    this->UniqueVertexCount = (uint32_t)uniqueNumberSet.size();
    this->NumberList = newNumberList;

}