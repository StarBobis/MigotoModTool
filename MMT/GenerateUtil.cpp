#include "GenerateUtil.h"
#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MigotoFormatUtils.h"
#include "VertexBufferBufFile.h"

//这个才是真正在使用的读取并分割重计算
void SplitUtil::readSplitRecalculate() {
    //从配置文件读取tangent计算方法
    std::wstring TangentAlgorithm = basicConfig.TangentAlgorithm;
    
    this->CategoryList = d3d11GameType.getCategoryList(basicConfig.TmpElementList);
    this->CategoryStrideMap = d3d11GameType.getCategoryStrideMap(basicConfig.TmpElementList);
    //输出查看每个Category的步长
    for (const auto& pair : CategoryStrideMap) {
        const std::string& key = pair.first;
        int value = pair.second;
        LOG.Info(L"Category: " + MMTString_ToWideString(key) + L", CategoryStride: " + std::to_wstring(value));
    }
    LOG.NewLine();

    //查看CategoryList
    LOG.Info(L"CategoryList:");
    for (std::string categoryName: CategoryList) {
        LOG.Info(L"Ordered CategoryName: " + MMTString_ToWideString(categoryName));
    }
    LOG.NewLine();

    //读取vb文件，每个vb文件都按照category分开装载不同category的数据
    int SplitStride = d3d11GameType.getElementListStride(basicConfig.TmpElementList);
    LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));

    for (std::string partName : basicConfig.PartNameList) {
        partNameOffsetMap[MMTString_ToWideString(partName)] = drawNumber;
        LOG.Info(L"Set partName: " + MMTString_ToWideString(partName) + L" offset to drawNumber: " + std::to_wstring(drawNumber));
        std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";

        uint64_t VBFileSize = MMTFile_GetFileSize(splitReadFolder + VBFileName);
        uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
        //添加到drawNumber
        drawNumber = drawNumber + vbFileVertexNumber;
        LOG.Info(L"Processing VB file: " + VBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(vbFileVertexNumber));
        
        VertexBufferBufFile vbBufFile(splitReadFolder + VBFileName,d3d11GameType,basicConfig.TmpElementList);
        partName_VBCategoryDaytaMap[partName] = vbBufFile.CategoryVBDataMap;
    }


    LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
    //将partName_VBCategoryDaytaMap里的内容，放入finalVBCategoryDataMap中组合成一个，供后续使用
    for (std::string partName : basicConfig.PartNameList) {
        std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = partName_VBCategoryDaytaMap[partName];

        for (size_t i = 0; i < CategoryList.size(); ++i) {
            const std::string& category = CategoryList[i];
            std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];

            std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString_ToWideString(category)];
            finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

        }
    }
    LOG.NewLine();
    //上面就是分割的全部内容

    //整个模型上计算 TANGENT重计算
    if (TangentAlgorithm == L"average_normal") {
        std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = finalVBCategoryDataMap;
        //进行重算
        std::unordered_map<std::wstring, std::vector<std::byte>> finalfixTangentVBCategoryDataMap = TANGENT_averageNormal(tmpVBCategoryDataMap,d3d11GameType);
        //重算完成后赋值回去
        finalVBCategoryDataMap = finalfixTangentVBCategoryDataMap;
    }

    //COLOR值重计算
    if (basicConfig.ColorAlgorithm == L"average_normal") {
        std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = finalVBCategoryDataMap;
        //进行重算
        std::unordered_map<std::wstring, std::vector<std::byte>> finalfixTangentVBCategoryDataMap = RecalculateColor(tmpVBCategoryDataMap, basicConfig, CategoryStrideMap);
        //重算完成后赋值回去
        finalVBCategoryDataMap = finalfixTangentVBCategoryDataMap;
        
    }
  
    LOG.Info(std::to_wstring(finalVBCategoryDataMap[L"Position"].size() / 40));
    // 输出查看drawNumber:
    LOG.Info(L"Set draw number to: " + std::to_wstring(drawNumber));
    LOG.NewLine();

    LOG.Info(L"Start to flip NORMAL and TANGENT Values:");
    std::unordered_map<std::wstring, std::vector<std::byte>> finalfixTangentVBCategoryDataMap;
    finalfixTangentVBCategoryDataMap = ReverseNormalTangentValues(finalVBCategoryDataMap, basicConfig);
    finalVBCategoryDataMap = finalfixTangentVBCategoryDataMap;
    LOG.NewLine();

    LOG.Info(L"Start to reset COLOR Values:");
    std::unordered_map<std::wstring, std::vector<std::byte>> finalfixColorVBCategoryDataMap;
    finalfixColorVBCategoryDataMap = ResetColor(finalVBCategoryDataMap, basicConfig, CategoryStrideMap);
    finalVBCategoryDataMap = finalfixColorVBCategoryDataMap;
    LOG.NewLine();
    // 上面就是把每个 vb的数据读取到列表里，然后对每个vb修改Color和Tangent后，分割到buf
}


void SplitUtil::outputModFiles() {
    //(2) 下面开始就是直接输出到各个Buf文件
    LOG.NewLine();

    //先删除上一次生成的 buf文件 和 ib文件 ini文件
    MMTFile_DeleteFilesWithSuffix(splitOutputFolder, L".buf");
    MMTFile_DeleteFilesWithSuffix(splitOutputFolder, L".ib");
    //先删除上一次生成的
    MMTFile_DeleteFilesWithSuffix(splitOutputFolder, L".ini");

    //获取BLENDWEIGHTS的名称
    std::wstring blendElementName;
    if (boost::algorithm::any_of_equal(basicConfig.TmpElementList, "BLENDWEIGHT")) {
        blendElementName = L"BLENDWEIGHT";
    }
    else {
        blendElementName = L"BLENDWEIGHTS";
    }
    blendElementByteWidth = d3d11GameType.ElementNameD3D11ElementMap[MMTString_ToByteString(blendElementName)].ByteWidth;
    int blendIndicesByteWidth = d3d11GameType.ElementNameD3D11ElementMap["BLENDINDICES"].ByteWidth;

    LOG.Info(L"Get BlendWeigths width: " + std::to_wstring(blendElementByteWidth));
    LOG.Info(L"Get BlendIndices width: " + std::to_wstring(blendIndicesByteWidth));

    for (const auto& pair : finalVBCategoryDataMap) {
        const std::wstring& categoryName = pair.first;
        const std::vector<std::byte>& categoryData = pair.second;
        LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));

        //如果没有那就不输出
        if (categoryData.size() == 0) {
            LOG.Info(L"Current category's size is 0, can't output, skip this.");
            continue;
        }

        std::wstring categoryGeneratedName = L"";
        
        categoryGeneratedName = basicConfig.DrawIB + categoryName;
            
        this->categoryUUIDMap[MMTString_ToByteString(categoryName)] = MMTString_ToByteString(categoryGeneratedName);
       

        // 构建输出文件路径
        std::wstring outputDatFilePath = splitOutputFolder + categoryGeneratedName + L".buf";
        // 打开输出文件
        std::ofstream outputFile(MMTString_ToByteString(outputDatFilePath), std::ios::binary);


        //如果缺失BLENDWEIGHTS元素，则需要手动删除BLEND槽位中的BLEND元素
        if (d3d11GameType.PatchBLENDWEIGHTS && categoryName == L"Blend") {

            LOG.Info(L"Detect element: " + blendElementName + L" need to delete , now will delete it.");
            //去除BLENDWEIGHTS后的数据列表
            std::vector<std::byte> newBlendCategoryData;

            //遍历并只把BLENDINDICES添加到元素列表
            int blendWeightCount = 1;
            int blendIndicesCount = 1;
            for (std::byte singleByte : categoryData) {
                //std::cout << "当前blendWeightCount: " << blendWeightCount << std::endl;
                //std::cout << "当前blendIndicesCount: " << blendIndicesCount << std::endl;
                if (blendWeightCount <= blendElementByteWidth) {
                    blendWeightCount += 1;
                }
                else {
                    //然后开始添加到newBlendCategoryData
                    if (blendIndicesCount <= blendIndicesByteWidth) {
                        //std::cout << "执行放入操作" << std::endl;
                        newBlendCategoryData.push_back(singleByte);
                        blendIndicesCount += 1;


                        //放入完之后就需要立刻归零，所以这里判断
                        if ((blendWeightCount == blendElementByteWidth + 1) && (blendIndicesCount == blendIndicesByteWidth + 1)) {
                            blendWeightCount = 1;
                            blendIndicesCount = 1;
                        }
                    }
                    else {
                        blendWeightCount = 1;
                        blendIndicesCount = 1;

                    }
                }


            }
            outputFile.write(reinterpret_cast<const char*>(newBlendCategoryData.data()), newBlendCategoryData.size());
        }
        else {
            // 将std::vecto的内容写入文件
            outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
        }
        outputFile.close();

        LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
    }
    LOG.NewLine();

    //先设置读取ib文件所使用的步长,从fmt文件中自动读取
    //读取body_part0.fmt文件中的Format
    std::wstring readFormatFileName = splitReadFolder + L"1.fmt";
    std::wstring IBReadDxgiFormat = MMTFile_FindMigotoIniAttributeInFile(readFormatFileName, L"format");
    LOG.Info(L"Auto read IB format: " + IBReadDxgiFormat);
    LOG.Info(L"Default output IB format: DXGI_FORMAT_R32_UINT");
    LOG.NewLine();

    //读取ib文件,转换格式后直接写出
    for (std::string partName : basicConfig.PartNameList) {
        uint64_t offset = partNameOffsetMap[MMTString_ToWideString(partName)];
        std::wstring IBFileName = MMTString_ToWideString(partName) + L".ib";
        LOG.Info(L"Processing IB file: " + IBFileName);

        // 指定要读取的字节长度
        int readLength = 2;
        if (IBReadDxgiFormat == L"DXGI_FORMAT_R16_UINT") {
            readLength = 2;
        }
        if (IBReadDxgiFormat == L"DXGI_FORMAT_R32_UINT") {
            readLength = 4;
        }
        //循环读取直到文件结束
        std::ifstream ReadIBFile(MMTString_ToByteString(splitReadFolder + IBFileName), std::ios::binary);

        //R32_UINT几乎可以涵盖所有情况，所以固定使用R16_UINT，方便很多
        std::vector<uint16_t> IBR16DataList = {};
        std::vector<uint32_t> IBR32DataList = {};

        // 创建char变量读取数据
        char* data = new char[readLength];

        if (IBReadDxgiFormat == L"DXGI_FORMAT_R16_UINT") {
            while (ReadIBFile.read(data, readLength)) {
                std::uint16_t value = (static_cast<unsigned char>(data[1]) << 8) |
                    static_cast<unsigned char>(data[0]);
                value = value + static_cast<std::uint16_t>(offset);
                IBR16DataList.push_back(value);
            }
        }
        else if (IBReadDxgiFormat == L"DXGI_FORMAT_R32_UINT") {
            while (ReadIBFile.read(data, readLength)) {
                std::uint32_t value = (static_cast<unsigned char>(data[3]) << 24) |
                    (static_cast<unsigned char>(data[2]) << 16) |
                    (static_cast<unsigned char>(data[1]) << 8) |
                    static_cast<unsigned char>(data[0]);
                value = value + static_cast<std::uint32_t>(offset);
                IBR32DataList.push_back(value);
            }

        }
        else {
            LOG.Error(L"Can't find a valid DXGI FORMAT, current procesing DXGI FORMAT: " + IBReadDxgiFormat);
        }

        // 读取完成关闭文件
        ReadIBFile.close();

        std::wstring partNameGeneratedName = L"";
        
        partNameGeneratedName = this->basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)];
            
        this->partNameUUIDMap[partName] = MMTString_ToByteString(partNameGeneratedName);


        //拼接输出的文件名
        std::wstring outputIBFileName = splitOutputFolder + partNameGeneratedName + L".ib";

        //输出固定为R32_UINT，所以如果读取的是R16_UINT则需要做一个转换
        if (IBReadDxgiFormat == L"DXGI_FORMAT_R16_UINT") {
            //转换之前得初始化长度，不然会报错
            IBR32DataList = std::vector<uint32_t>(IBR16DataList.size());
            std::transform(IBR16DataList.begin(), IBR16DataList.end(), IBR32DataList.begin(),
                [](uint16_t value) { return static_cast<uint32_t>(value); });
        }

        //开始输出
        LOG.Info(L"IB file length: " + std::to_wstring(IBR32DataList.size()));
        std::ofstream file(MMTString_ToByteString(outputIBFileName), std::ios::binary); // 打开文件
        //写入时必须确保是4个字节的长度
        for (const auto& data : IBR32DataList) {
            uint32_t paddedData = data; // 原始数据
            file.write(reinterpret_cast<const char*>(&paddedData), sizeof(uint32_t));
        }
        file.close(); // 关闭文件

        //offset = offset + partNameOffsetMap[partName];
    }
}

SplitUtil::SplitUtil() {

}

SplitUtil::SplitUtil(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType) {
    this->wheelConfig = wheelConfig;
    this->basicConfig = basicConfig;
    this->d3d11GameType = d3d11GameType;
}

