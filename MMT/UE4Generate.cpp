//#include "GlobalConfigs.h"
//#include <chrono>
//#include "D3D11ElementAttributeClass.h"
//#include <boost/algorithm/string.hpp>
//#include <boost/date_time/gregorian/gregorian.hpp>
//#include "MigotoIniFile.h"
//#include "GenerateUtil.h"
//#include "MMTStringUtils.h"
//#include "MMTFormatUtils.h"
//#include "MMTFileUtils.h"
//
//
//void UE4Split(GlobalConfigs& wheelConfig) {
//    for (const auto& pair : wheelConfig.drawIBBasicConfigMap) {
//        std::wstring drawIB = pair.first;
//        ExtractConfig basicConfig = pair.second;
//        basicConfig.GameType = MMTString_ToWideString(basicConfig.WorkGameType);
//        D3D11ElementAttributeClass D3D11ElementAttribute(basicConfig.GameType);
//
//        std::wstring timeStr = MMTString_GetFormattedDateTimeForFilename().substr(0, 10);
//        std::wstring splitReadFolder = wheelConfig.OutputFolder + drawIB + L"/";
//        std::wstring splitOutputFolder = wheelConfig.OutputFolder + timeStr + L"/" + drawIB + L"/";
//        std::filesystem::create_directories(splitOutputFolder);
//
//        //根据PartName创建对应的输出文件夹，后面就能直接用
//        for (std::string partName : basicConfig.PartNameList) {
//            std::wstring partNameFolder = splitOutputFolder + MMTString_ToWideString(partName) + L"/";
//            std::filesystem::create_directories(partNameFolder);
//        }
//
//        bool findValidFile = false;
//        for (std::string partName : basicConfig.PartNameList) {
//            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
//            if (std::filesystem::exists(splitReadFolder + VBFileName)) {
//                findValidFile = true;
//                break;
//            }
//        }
//        if (!findValidFile) {
//            LOG.Info(L"Detect didn't export vb file for DrawIB: " + drawIB + L" , so skip this drawIB generate.");
//            continue;
//        }
//
//        // 下面要读取并分割好，这里需要统计TmpElementList的总长度用来分割,只统计提取的
//        int SplitStride = 0;
//        for (std::string elementName : basicConfig.TmpElementList) {
//            D3D11Element elementObject = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//            if (elementObject.Extract) {
//                SplitStride = SplitStride + elementObject.ByteWidth;
//            }
//        }
//        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
//
//
//        //获取BLENDWEIGHTS的名称
//        std::wstring blendElementName;
//        if (boost::algorithm::any_of_equal(basicConfig.TmpElementList, "BLENDWEIGHT")) {
//            blendElementName = L"BLENDWEIGHT";
//        }
//        else {
//            blendElementName = L"BLENDWEIGHTS";
//        }
//        int blendElementByteWidth = D3D11ElementAttribute.ElementNameD3D11ElementMap[blendElementName].ByteWidth;
//        int blendIndicesByteWidth = D3D11ElementAttribute.ElementNameD3D11ElementMap[L"BLENDINDICES"].ByteWidth;
//        LOG.Info(L"Get BlendWeigths width: " + std::to_wstring(blendElementByteWidth));
//        LOG.Info(L"Get BlendIndices width: " + std::to_wstring(blendIndicesByteWidth));
//
//        // 这里的CategoryList必须是按照d3d11Element的有序列表来排序的，需要现场计算
//        // 我们的tmpElementList肯定是正确的顺序，因为在merge的时候排序过了
//        // 接下来是收集所有vb文件中的数据，每个都得按照category进行分类，所以需要提前计算出每个category的步长
//        std::unordered_map<std::wstring, int> CategoryStrideMap;
//        std::vector<std::wstring> CategoryList;
//        for (std::string elementName : basicConfig.TmpElementList) {
//            D3D11Element elementObject = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//            int byteWidth = elementObject.ByteWidth;
//            std::wstring elementCategory = elementObject.Category;
//            int categoryStride = CategoryStrideMap[elementCategory];
//            if (categoryStride == NULL) {
//                categoryStride = byteWidth;
//            }
//            else {
//                categoryStride = categoryStride + byteWidth;
//            }
//            CategoryStrideMap[elementCategory] = categoryStride;
//
//            if (!boost::algorithm::any_of_equal(CategoryList, elementCategory)) {
//                CategoryList.push_back(elementCategory);
//            }
//
//        }
//
//        //输出查看每个Category的步长
//        for (const auto& pair : CategoryStrideMap) {
//            const std::wstring& key = pair.first;
//            int value = pair.second;
//            LOG.Info(L"Category: " + key + L", CategoryStride: " + std::to_wstring(value));
//        }
//        LOG.NewLine();
//
//        //查看CategoryList
//        LOG.Info(L"CategoryList:");
//        for (std::wstring categoryName : CategoryList) {
//            LOG.Info(L"Ordered CategoryName: " + categoryName);
//        }
//        LOG.NewLine();
//
//        //(1) 转换并输出每个IB文件
//        //设置每个PartName对应的Offset，用于IB文件的Offset设置。
//        //先设置读取ib文件所使用的Format,从1.fmt文件中自动读取
//        std::wstring IBReadDxgiFormat = MMTFile_FindMigotoIniAttributeInFile(splitReadFolder + L"1.fmt", L"format");
//        uint64_t IBOffset = 0;
//        for (std::string partName : basicConfig.PartNameList) {
//            std::wstring IBFileName = MMTString_ToWideString(partName) + L".ib";
//            std::wstring readIBFileName = splitReadFolder + IBFileName;
//            std::wstring writeIBFileName = splitOutputFolder + MMTString_ToWideString(partName) + L"/" + IBFileName;
//            LOG.Info(L"Converting IB file: " + IBFileName);
//            //TODO 既然每一个IB的index都是一个单独的Mod，那么有没有可能每个都是单独的从0开始的呢？
//            MMTFile_ConvertIndexBufferFileToR32_UINT(readIBFileName, writeIBFileName, 0, IBReadDxgiFormat);
//
//            LOG.Info(L"Set partName: " + MMTString_ToWideString(partName) + L" offset to: " + std::to_wstring(IBOffset));
//            std::wstring readVBFileName = MMTString_ToWideString(partName) + L".vb";
//            std::ifstream readVBFile(MMTString_ToByteString(splitReadFolder + readVBFileName), std::ios::binary);
//            readVBFile.seekg(0, std::ios::end);
//            std::streampos VBFileSize = readVBFile.tellg();
//            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
//            IBOffset = IBOffset + vbFileVertexNumber;
//        }
//        LOG.Info(L"Output ib file over");
//        LOG.NewLine();
//
//
//
//        //(2)读取每个partName对应的vb文件，分割成各个Category文件后输出
//        for (std::string partName : basicConfig.PartNameList) {
//            std::unordered_map<std::wstring, std::vector<std::byte>> finalVBElementDataMap;
//            std::wstring readVBFileName = MMTString_ToWideString(partName) + L".vb";
//            std::ifstream readVBFile(MMTString_ToByteString(splitReadFolder + readVBFileName), std::ios::binary);
//            readVBFile.seekg(0, std::ios::end);
//            std::streampos VBFileSize = readVBFile.tellg();
//            uint64_t drawNumber = VBFileSize / SplitStride;
//            LOG.Info(L"Processing VB file: " + readVBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(drawNumber));
//
//            // 将文件指针定位到文件开头,然后再读取
//            readVBFile.seekg(0);
//            std::vector<std::byte> readVBFileBuffer(VBFileSize);
//            readVBFile.read(reinterpret_cast<char*>(readVBFileBuffer.data()), VBFileSize);
//            readVBFile.close();
//
//            int offset = 0;
//            while (offset < readVBFileBuffer.size()) {
//                for (std::string elementName: basicConfig.TmpElementList) {
//                    D3D11Element elementObject = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//
//                    std::vector<std::byte> elementDataPatch;
//                    if (elementObject.Extract) {
//                        elementDataPatch.reserve(elementObject.ByteWidth);
//                        std::copy(readVBFileBuffer.begin() + offset, readVBFileBuffer.begin() + offset + elementObject.ByteWidth, std::back_inserter(elementDataPatch));
//                    }
//                    else {
//                        elementDataPatch = std::vector<std::byte>(elementObject.ByteWidth, std::byte{ 0x00 });
//                    }
//
//                    // 预先分配足够的空间
//                    std::vector<std::byte>& elementData = finalVBElementDataMap[MMTString_ToWideString(elementName)];
//                    // 将categoryDataPatch的数据直接插入categoryData
//                    elementData.insert(elementData.end(), elementDataPatch.begin(), elementDataPatch.end());
//
//                    if (elementObject.Extract) {
//                        offset += elementObject.ByteWidth;
//                    }
//                }
//
//            }
//
//            LOG.Info(L"Size Check:");
//            for (const auto& pair : finalVBElementDataMap) {
//                LOG.Info(L"ElementName:" + pair.first + L" " + std::to_wstring(pair.second.size()));
//            }
//            LOG.NewLine();
//
//            //再转换为finalVBCategoryDataMap
//            std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;
//            std::unordered_map<std::wstring, std::vector<std::byte>> categoryDataCache;
//
//            for (int i = 0; i < drawNumber; i++) {
//                for (const std::string& elementName : basicConfig.TmpElementList) {
//                    const D3D11Element& d3d11Element = D3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//                    const std::vector<std::byte>& elementData = finalVBElementDataMap[MMTString_ToWideString(elementName)];
//                    int offset = i * d3d11Element.ByteWidth;
//                    const std::vector<std::byte>& patchData = MMTFormat_GetRange_Byte(elementData, offset, offset + d3d11Element.ByteWidth);
//
//                    std::vector<std::byte>& categoryData = categoryDataCache[d3d11Element.Category];
//                    if (categoryData.empty()) {
//                        // 预先分配足够的内存空间
//                        categoryData.reserve(drawNumber * d3d11Element.ByteWidth);
//                    }
//                    categoryData.insert(categoryData.end(), patchData.begin(), patchData.end());
//                }
//            }
//
//            // 将缓存的结果插入到 finalVBCategoryDataMap 中
//            for (const auto& entry : categoryDataCache) {
//                finalVBCategoryDataMap.emplace(entry.first, entry.second);
//            }
//
//            LOG.Info(L"Start to patch 1 to NORMAL value in UE4.");
//            // ===================NORMAL补1===================
//            std::wstring NormalCategoryName = L"Normal";
//            std::vector<std::byte> NormalCategoryValues = finalVBCategoryDataMap[NormalCategoryName];
//            std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
//            std::vector<std::byte> patchValue = MMTFormat_PackNumberOneByte(0x7F);
//
//            std::vector<std::byte> newNormalCategoryValues;
//            int NormalCategoryStride = CategoryStrideMap[NormalCategoryName]; //7
//            int NORMAL_ElementStride = D3D11ElementAttribute.ElementNameD3D11ElementMap[L"NORMAL"].ByteWidth;//3
//            int TANGENT_ElementStride = D3D11ElementAttribute.ElementNameD3D11ElementMap[L"TANGENT"].ByteWidth;//4
//            LOG.Info(L"NormalCategoryStride: " + std::to_wstring(NormalCategoryStride));
//            LOG.Info(L"NORMAL_ElementStride: " + std::to_wstring(NORMAL_ElementStride));
//            LOG.Info(L"TANGENT_ElementStride: " + std::to_wstring(TANGENT_ElementStride));
//
//            for (std::size_t i = 0; i < NormalCategoryValues.size(); i = i + NormalCategoryStride)
//            {
//                std::vector<std::byte> NormalValues = MMTFormat_GetRange_Byte(NormalCategoryValues, i, i + NORMAL_ElementStride);
//                NormalValues.insert(NormalValues.end(), patchValue.begin(), patchValue.end());
//                std::vector<std::byte> TangentValues = MMTFormat_GetRange_Byte(NormalCategoryValues, i + NORMAL_ElementStride, i + NORMAL_ElementStride + TANGENT_ElementStride);
//                //新的Position分类的值
//                std::vector<std::byte> new_NORMAL_TANGENT_Values;
//                new_NORMAL_TANGENT_Values.insert(new_NORMAL_TANGENT_Values.end(), NormalValues.begin(), NormalValues.end());
//                new_NORMAL_TANGENT_Values.insert(new_NORMAL_TANGENT_Values.end(), TangentValues.begin(), TangentValues.end());
//
//                //追加到新的Normal里
//                newNormalCategoryValues.insert(newNormalCategoryValues.end(), new_NORMAL_TANGENT_Values.begin(), new_NORMAL_TANGENT_Values.end());
//            }
//            newVBCategoryDataMap = finalVBCategoryDataMap;
//            newVBCategoryDataMap[NormalCategoryName] = newNormalCategoryValues;
//            LOG.Info(L"newNormalCategoryValues Size: " + std::to_wstring(newNormalCategoryValues.size()));
//
//            finalVBCategoryDataMap = newVBCategoryDataMap;
//            LOG.Info(L"Patch 1 to NORMAL value in UE4 over.");
//            LOG.NewLine();
//
//
//            // =========================翻转NORMAL或TANGENT================================
//            LOG.Info(L"Start to flip NORMAL and TANGENT values.");
//            newVBCategoryDataMap.clear();
//            NormalCategoryValues = finalVBCategoryDataMap[NormalCategoryName];
//            newNormalCategoryValues.clear();
//
//            //这里 NormalCategoryStride + 1 因为我们新加了一个float进去啊
//            for (std::size_t i = 0; i < NormalCategoryValues.size(); i = i + NormalCategoryStride + 1)
//            {
//                //获取Position和Tangent值
//                std::vector<std::byte> NormalValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 0, i + 1);
//                std::vector<std::byte> NormalValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 1, i + 2);
//                std::vector<std::byte> NormalValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 2, i + 3);
//                std::vector<std::byte> NormalValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 3, i + 4);
//
//                std::vector<std::byte> TangentValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 4, i + 5);
//                std::vector<std::byte> TangentValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 5, i + 6);
//                std::vector<std::byte> TangentValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 6, i + 7);
//                std::vector<std::byte> TangentValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 7, i + 8);
//
//                //根据参数进行翻转
//                std::vector<std::byte> NormalValueX_Reversed;
//                std::vector<std::byte> NormalValueY_Reversed;
//                std::vector<std::byte> NormalValueZ_Reversed;
//                if (basicConfig.NormalReverseX) {
//                    NormalValueX_Reversed = MMTFormat_ReverseSNORMValue(NormalValueX);
//                }
//                else {
//                    NormalValueX_Reversed = NormalValueX;
//                }
//
//                if (basicConfig.NormalReverseY) {
//                    NormalValueY_Reversed = MMTFormat_ReverseSNORMValue(NormalValueY);
//
//                }
//                else {
//                    NormalValueY_Reversed = NormalValueY;
//                }
//
//                if (basicConfig.NormalReverseZ) {
//                    NormalValueZ_Reversed = MMTFormat_ReverseSNORMValue(NormalValueZ);
//
//                }
//                else {
//                    NormalValueZ_Reversed = NormalValueZ;
//                }
//
//                //放回Normal槽位
//                std::vector<std::byte> FlipNormalValues;
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueX_Reversed.begin(), NormalValueX_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueY_Reversed.begin(), NormalValueY_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueZ_Reversed.begin(), NormalValueZ_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueW.begin(), NormalValueW.end());
//
//                std::vector<std::byte> TangentValueX_Reversed;
//                std::vector<std::byte> TangentValueY_Reversed;
//                std::vector<std::byte> TangentValueZ_Reversed;
//                std::vector<std::byte> TangentValueW_Reversed;
//                if (basicConfig.TangentReverseX) {
//                    TangentValueX_Reversed = MMTFormat_ReverseSNORMValue(TangentValueX);
//                }
//                else {
//                    TangentValueX_Reversed = TangentValueX;
//                }
//                if (basicConfig.TangentReverseY) {
//                    TangentValueY_Reversed = MMTFormat_ReverseSNORMValue(TangentValueY);
//                }
//                else {
//                    TangentValueY_Reversed = TangentValueY;
//                }
//                if (basicConfig.TangentReverseZ) {
//                    TangentValueZ_Reversed = MMTFormat_ReverseSNORMValue(TangentValueZ);
//                }
//                else {
//                    TangentValueZ_Reversed = TangentValueZ;
//                }
//                if (basicConfig.TangentReverseW) {
//                    TangentValueW_Reversed = MMTFormat_ReverseSNORMValue(TangentValueW);
//                }
//                else {
//                    TangentValueW_Reversed = TangentValueW;
//                }
//
//                //放回Tangent槽位
//                std::vector<std::byte> FlipTangentValues;
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueX_Reversed.begin(), TangentValueX_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueY_Reversed.begin(), TangentValueY_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueZ_Reversed.begin(), TangentValueZ_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueW_Reversed.begin(), TangentValueW_Reversed.end());
//
//
//                //新的Position分类的值
//                std::vector<std::byte> newPositionValues;
//                newPositionValues.insert(newPositionValues.end(), FlipNormalValues.begin(), FlipNormalValues.end());
//                newPositionValues.insert(newPositionValues.end(), FlipTangentValues.begin(), FlipTangentValues.end());
//                newNormalCategoryValues.insert(newNormalCategoryValues.end(), newPositionValues.begin(), newPositionValues.end());
//            }
//
//            newVBCategoryDataMap = finalVBCategoryDataMap;
//            newVBCategoryDataMap[NormalCategoryName] = newNormalCategoryValues;
//            finalVBCategoryDataMap = newVBCategoryDataMap;
//            LOG.Info(L"Flip NORMAL and TANGENT values over.");
//            LOG.NewLine();
//
//            // TODO 由于Blender插件的导出设置，我们需要修正NORMAL和TANGENT的方向
//            // 但是不知道具体怎么修正方向，只是通过结果观察到了规律：
//            newVBCategoryDataMap.clear();
//            NormalCategoryValues = finalVBCategoryDataMap[NormalCategoryName];
//            newNormalCategoryValues.clear();
//
//            //这里 NormalCategoryStride + 1 因为我们新加了一个float进去啊
//            for (std::size_t i = 0; i < NormalCategoryValues.size(); i = i + NormalCategoryStride + 1)
//            {
//                //获取Position和Tangent值
//                std::vector<std::byte> NormalValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 0, i + 1);
//                std::vector<std::byte> NormalValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 1, i + 2);
//                std::vector<std::byte> NormalValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 2, i + 3);
//                std::vector<std::byte> NormalValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 3, i + 4);
//                std::vector<std::byte> TangentValueX = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 4, i + 5);
//                std::vector<std::byte> TangentValueY = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 5, i + 6);
//                std::vector<std::byte> TangentValueZ = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 6, i + 7);
//                std::vector<std::byte> TangentValueW = MMTFormat_GetRange_Byte(NormalCategoryValues, i + 7, i + 8);
//
//                // 翻转NORMAL的前三位，并将其放到TANGENT的x,y,z，翻转TANGENT的W
//                // TANGENT的前三位直接放到NORMAL的前三位
//                std::vector<std::byte> NormalValueX_Reversed;
//                std::vector<std::byte> NormalValueY_Reversed;
//                std::vector<std::byte> NormalValueZ_Reversed;
//                std::vector<std::byte> NormalValueW_Reversed;
//                std::vector<std::byte> TangentValueX_Reversed;
//                std::vector<std::byte> TangentValueY_Reversed;
//                std::vector<std::byte> TangentValueZ_Reversed;
//                std::vector<std::byte> TangentValueW_Reversed;
//
//                NormalValueX_Reversed = TangentValueX;
//                NormalValueY_Reversed = TangentValueY;
//                NormalValueZ_Reversed = TangentValueZ;
//
//                //根本读取不到这个NORMAL的W分量
//                NormalValueW_Reversed = NormalValueW;
//                TangentValueX_Reversed = MMTFormat_ReverseSNORMValue(NormalValueX);
//                TangentValueY_Reversed = MMTFormat_ReverseSNORMValue(NormalValueY);
//                TangentValueZ_Reversed = MMTFormat_ReverseSNORMValue(NormalValueZ);
//                TangentValueW_Reversed = TangentValueW;
//
//                //放回Normal槽位
//                std::vector<std::byte> FlipNormalValues;
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueX_Reversed.begin(), NormalValueX_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueY_Reversed.begin(), NormalValueY_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueZ_Reversed.begin(), NormalValueZ_Reversed.end());
//                FlipNormalValues.insert(FlipNormalValues.end(), NormalValueW_Reversed.begin(), NormalValueW_Reversed.end());
//                std::vector<std::byte> FlipTangentValues;
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueX_Reversed.begin(), TangentValueX_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueY_Reversed.begin(), TangentValueY_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueZ_Reversed.begin(), TangentValueZ_Reversed.end());
//                FlipTangentValues.insert(FlipTangentValues.end(), TangentValueW_Reversed.begin(), TangentValueW_Reversed.end());
//
//                //新的Position分类的值
//                std::vector<std::byte> newPositionValues;
//                //newPositionValues.insert(newPositionValues.end(), NORMAL_TANGENT_Values.begin(), NORMAL_TANGENT_Values.end());
//                newPositionValues.insert(newPositionValues.end(), FlipNormalValues.begin(), FlipNormalValues.end());
//                newPositionValues.insert(newPositionValues.end(), FlipTangentValues.begin(), FlipTangentValues.end());
//
//                newNormalCategoryValues.insert(newNormalCategoryValues.end(), newPositionValues.begin(), newPositionValues.end());
//            }
//
//            newVBCategoryDataMap = finalVBCategoryDataMap;
//            newVBCategoryDataMap[NormalCategoryName] = newNormalCategoryValues;
//            finalVBCategoryDataMap = newVBCategoryDataMap;
//            LOG.Info(L"Flip NORMAL and TANGENT values over.");
//            LOG.NewLine();
//
//
//            //(2) 下面开始就是直接输出到各个Buf文件
//            LOG.NewLine();
//            for (const auto& pair : finalVBCategoryDataMap) {
//                const std::wstring& categoryName = pair.first;
//                const std::vector<std::byte>& categoryData = pair.second;
//                LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));
//
//                //如果没有那就不输出
//                if (categoryData.size() == 0) {
//                    LOG.Warning(L"Current category's size is 0, can't output, skip this.");
//                    continue;
//                }
//
//                // 构建输出文件路径
//                std::wstring outputDatFilePath = splitOutputFolder + MMTString_ToWideString(partName)+ L"/" + MMTString_ToWideString(partName) + L"_" + basicConfig.DrawIB + L"_" + categoryName + L".buf";
//                // 打开输出文件
//                std::ofstream outputFile(MMTString_ToByteString(outputDatFilePath), std::ios::binary);
//
//
//                //如果缺失BLENDWEIGHTS元素，则需要手动删除BLEND槽位中的BLEND元素
//                if (D3D11ElementAttribute.PatchBLENDWEIGHTS && categoryName == L"Blend") {
//
//                    LOG.Info(L"Detect element: " + blendElementName + L" need to delete , now will delete it.");
//                    //去除BLENDWEIGHTS后的数据列表
//                    std::vector<std::byte> newBlendCategoryData;
//
//                    //遍历并只把BLENDINDICES添加到元素列表
//                    int blendWeightCount = 1;
//                    int blendIndicesCount = 1;
//                    for (std::byte singleByte : categoryData) {
//                        //std::cout << "当前blendWeightCount: " << blendWeightCount << std::endl;
//                        //std::cout << "当前blendIndicesCount: " << blendIndicesCount << std::endl;
//                        if (blendWeightCount <= blendElementByteWidth) {
//                            blendWeightCount += 1;
//                        }
//                        else {
//                            //然后开始添加到newBlendCategoryData
//                            if (blendIndicesCount <= blendIndicesByteWidth) {
//                                //std::cout << "执行放入操作" << std::endl;
//                                newBlendCategoryData.push_back(singleByte);
//                                blendIndicesCount += 1;
//
//
//                                //放入完之后就需要立刻归零，所以这里判断
//                                if ((blendWeightCount == blendElementByteWidth + 1) && (blendIndicesCount == blendIndicesByteWidth + 1)) {
//                                    blendWeightCount = 1;
//                                    blendIndicesCount = 1;
//                                }
//                            }
//                            else {
//                                blendWeightCount = 1;
//                                blendIndicesCount = 1;
//
//                            }
//                        }
//
//
//                    }
//                    outputFile.write(reinterpret_cast<const char*>(newBlendCategoryData.data()), newBlendCategoryData.size());
//                }
//                else {
//                    // 将std::vecto的内容写入文件
//                    outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
//                }
//                outputFile.close();
//
//                LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
//            }
//            LOG.NewLine();
//
//
//        }
//        LOG.Info(L"Read and split vb file over.");
//        LOG.NewLine();
//       
//
//        //(3) 调用生成Wheel格式的ini文件
//        // TODO 我感觉不需要VertexLimitRaise了，或者说每个vb槽位都需要VertexLimitRaise
//        // 生成一个总的ini文件来控制分开的ini文件 
//        std::wstring outputIniFileName = splitOutputFolder + basicConfig.DrawIB  + L".ini";
//        std::wofstream outputIniFile(outputIniFileName);
//
//        //IB SKIP部分
//        outputIniFile << L"[TextureOverride_" + basicConfig.DrawIB + L"_IB_SKIP]" << std::endl;
//        outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
//        outputIniFile << "handling = skip" << std::endl;
//        outputIniFile << std::endl;
//
//        //vb0的VertexLimitRaise
//         //3.VertexLimitRaise部分
//        //std::string VertexLimitVB = basicConfig.VertexLimitVB;
//        ////步长，drawNumber，实现动态步长
//        //outputIniFile << L"[TextureOverride_VB_" + basicConfig.DrawIB + L"_" + std::to_wstring(CategoryStrideMap[L"Position"]) + L"_" + std::to_wstring(IBOffset) + L"_VertexLimitRaise]" << std::endl;
//        //outputIniFile << L"hash = " + MMTString_ToWideString(VertexLimitVB) << std::endl << std::endl;
//        //outputIniFile << std::endl;
//        outputIniFile.close();
//        
//        //这里要给每一个都生成单独的ini文件
//        for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
//            std::string partName = basicConfig.PartNameList[i];
//            LOG.Info(L"Start to output UE4 ini file.");
//            std::wstring outputIniFileName = splitOutputFolder + MMTString_ToWideString(partName) + L"/" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L".ini";
//            std::wofstream outputIniFile(outputIniFileName);
//
//            //按键开关支持
//            bool generateSwitchKey = false;
//            std::wstring activateFlagName = L"ActiveFlag_" + basicConfig.DrawIB;
//            std::wstring switchVarName = L"SwitchVar_" + basicConfig.DrawIB;
//            std::wstring replace_prefix = L"";
//            if (basicConfig.SwitchKey != L"") {
//                generateSwitchKey = true;
//                replace_prefix = L"  ";
//                //添加对应Constants和KeySwitch部分
//                outputIniFile << "[Constants]" << std::endl;
//                outputIniFile << "global persist $" << switchVarName << " = 1" << std::endl;
//                outputIniFile << "global $" << activateFlagName << " = 0" << std::endl;
//                outputIniFile << std::endl;
//
//                outputIniFile << "[Key" << switchVarName << "]" << std::endl;
//                outputIniFile << "condition = $" << activateFlagName << " == 1" << std::endl;
//                outputIniFile << "key = " << basicConfig.SwitchKey << std::endl;
//                outputIniFile << "type = cycle" << std::endl;
//                outputIniFile << "$" << switchVarName << " = 0,1" << std::endl;
//                outputIniFile << std::endl;
//
//                outputIniFile << "[Present]" << std::endl;
//                outputIniFile << "post $" << activateFlagName << " = 0" << std::endl;
//                outputIniFile << std::endl;
//            }
//            LOG.Info(L"Generate Switch Key ini :" + std::to_wstring(generateSwitchKey));
//
//            //4.IBOverride部分
//            std::string IBFirstIndex = basicConfig.MatchFirstIndexList[i];
//            outputIniFile << L"[Resource_BakIB]"<< std::endl;
//            outputIniFile << L"[TextureOverride_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
//            outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
//            outputIniFile << L"Resource_BakIB = ref ib" << std::endl;
//            outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
//            if (generateSwitchKey) {
//                outputIniFile << L"if $" + switchVarName + L" == 1" << std::endl;
//            }
//            outputIniFile << replace_prefix << L"ib = Resource_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) << std::endl;
//            //6.TextureOverride VB部分
//            for (std::wstring categoryName : CategoryList) {
//                std::string categoryHash = basicConfig.CategoryHashMap[MMTString_ToByteString(categoryName)];
//                std::string categorySlot = D3D11ElementAttribute.CategorySlotMap[MMTString_ToByteString(categoryName)];
//                LOG.Info(L"categoryName: " + categoryName + L" categorySlot: " + MMTString_ToWideString(categorySlot));
//                if (categoryName == L"Position") {
//                    outputIniFile << MMTString_ToWideString(categorySlot) + L" = copy Resource_VB_" + categoryName << std::endl;
//                }
//                else {
//                    outputIniFile << MMTString_ToWideString(categorySlot) + L" = copy Resource_VB_" + categoryName << std::endl;
//                }
//            }
//            
//            outputIniFile << replace_prefix << "drawindexed = auto" << std::endl;
//            outputIniFile << L"ib = Resource_BakIB" << std::endl;
//
//            if (generateSwitchKey) {
//                outputIniFile << "endif" << std::endl;
//            }
//            outputIniFile << std::endl;
//
//
//            //1.写出VBResource部分
//            for (std::wstring categoryName : CategoryList) {
//                std::wstring fileName = MMTString_ToWideString(partName) + L"_" + basicConfig.DrawIB + L"_" + categoryName + L".buf";
//                std::wstring filePath = splitOutputFolder + MMTString_ToWideString(partName) + L"/" + fileName;
//                int fileSize = MMTFile_GetFileSize(filePath);
//                
//                outputIniFile << L"[Resource_VB_" + categoryName + L"]" << std::endl;
//                outputIniFile << L"type = Buffer" << std::endl;
//                outputIniFile << L"byte_width = " << std::to_wstring(fileSize) << std::endl;
//
//                if (categoryName == L"Blend" && D3D11ElementAttribute.PatchBLENDWEIGHTS) {
//                    int finalBlendStride = CategoryStrideMap[categoryName] - blendElementByteWidth;
//                    outputIniFile << "stride = " << std::to_wstring(finalBlendStride) << std::endl;
//
//                }
//                //这里Normal槽位需要+1，因为我们配置里写的NORMAL长度为3，比游戏里的4少了一个1
//                else if (categoryName == L"Normal") {
//                    outputIniFile << "stride = " << CategoryStrideMap[categoryName] + 1 << std::endl;
//                }
//                else {
//                    outputIniFile << "stride = " << CategoryStrideMap[categoryName] << std::endl;
//
//                }
//
//                outputIniFile << "filename = " << fileName << std::endl << std::endl;
//            }
//
//            //2.写出IBResource部分
//            outputIniFile << L"[Resource_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
//            outputIniFile << "type = Buffer" << std::endl;
//            outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
//            outputIniFile << "filename = " << MMTString_ToWideString(partName) + L".ib" << std::endl << std::endl;
//
//            //写完关闭文件
//            outputIniFile.close();
//        
//        }
//
//        //VertexShaderCheck部分
//        LOG.Info(L"Start to generate vertex shader check.");
//        //generate basic_check lines if this game need use basic_check technique.
//        if (wheelConfig.VertexShaderCheck) {
//            Analysis_ModifyShaderCheckByDrawIB(basicConfig.DrawIB, wheelConfig);
//        }
//        LOG.NewLine();
//
//        LOG.NewLine();
//        LOG.Info(L"Generate mod completed!");
//        LOG.NewLine();
//
//    }
//
//}
//
