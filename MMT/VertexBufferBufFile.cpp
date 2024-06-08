#include "VertexBufferBufFile.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"
#include <fstream>

VertexBufferBufFile::VertexBufferBufFile() {

}


VertexBufferBufFile::VertexBufferBufFile(std::wstring readVBBufFilePath, D3D11GameType d3d11GameType, std::vector<std::string> elementList) {

    int SplitStride = d3d11GameType.getElementListStride(elementList);
    std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(elementList);
    std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(elementList);

    uint64_t VBFileSize = MMTFile_GetFileSize(readVBBufFilePath);
    uint64_t vbFileVertexNumber = VBFileSize / SplitStride;

    //读取数据
    std::vector<std::byte> buffer(VBFileSize);
    std::ifstream VBFile(MMTString_ToByteString(readVBBufFilePath), std::ios::binary);
    VBFile.read(reinterpret_cast<char*>(buffer.data()), VBFileSize);
    VBFile.close();

    //通过类型，获取读取长度，这里还是有序的
    std::vector<int> readLengths;
    for (const std::string& category : CategoryList) {
        readLengths.push_back(CategoryStrideMap[category]);
    }

    int offset = 0;
    while (offset < buffer.size()) {
        for (size_t i = 0; i < CategoryList.size(); ++i) {
            const std::string& category = CategoryList[i];
            // 指定要读取的字节长度
            int readLength = readLengths[i];
            std::vector<std::byte> categoryDataPatch;
            // 预分配足够的内存空间
            categoryDataPatch.reserve(readLength);
            // 使用std::copy而不是循环来提高效率
            std::copy(buffer.begin() + offset, buffer.begin() + offset + readLength, std::back_inserter(categoryDataPatch));

            // 找到tmp的这个
            std::unordered_map<std::wstring, std::vector<std::byte>>& tmpVBCategoryDataMap = this->CategoryVBDataMap;

            // 找到tmp的这个对应的category的数据
            std::vector<std::byte>& categoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];


            // 将categoryDataPatch的数据直接插入categoryData
            categoryData.insert(categoryData.end(), categoryDataPatch.begin(), categoryDataPatch.end());

            offset += readLength;
        }

    }



};