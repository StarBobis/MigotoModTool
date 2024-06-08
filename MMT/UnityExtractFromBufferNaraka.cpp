//#include "GlobalConfigs.h"
//#include "VertexBufferFileData.h"
//#include "IndexBufferTxtFile.h"
//#include "D3D11ElementAttributeClass.h"
//#include <set>
//#include "VertexBufferTxtFileDetect.h"
//
//
//void UnityExtractFromBufferNaraka(GlobalConfigs& wheelConfigs,ExtractConfig basicConfig) {
//    D3D11ElementAttributeClass d3D11ElementAttribute(basicConfig.GameType);
//
//    LOG.Info(L"Start to merge for DrawIB:" + basicConfig.DrawIB);
//    std::wstring ModOutputFolder = wheelConfigs.OutputFolder + basicConfig.DrawIB + L"/";
//    LOG.Info(L"ModOutputFolder: " + ModOutputFolder);
//    std::filesystem::create_directories(ModOutputFolder);
//
//    //获取所有FrameAnalysisFolder下的文件
//    std::vector<std::wstring> FrameAnalyseFileNameList = MMTFile_ReadFrameAnalysisFileList(wheelConfigs.WorkFolder);
//    //根据DrawIB，获取所有的Trianglelist Index
//    std::vector<std::wstring> trianglelistIndexList = MMTFile_ReadTrianglelistIndexList(wheelConfigs.WorkFolder, basicConfig.DrawIB);
//
//    
//    //首先根据DrawIB，获取其vb0文件的show vertex number
//    int maxTrianglelistVertexCountNumber = 0;
//    for (std::wstring trianglelistIndex : trianglelistIndexList) {
//        //根据Index获取所有的VB文件的列表
//        std::wstring searchStr = trianglelistIndex + L"-vb0";
//        std::wstring suffixStr = L".txt";
//        std::vector<std::wstring> trianglelistFileNameList;
//        for (std::wstring fileName : FrameAnalyseFileNameList) {
//            if (fileName.find(searchStr) != std::string::npos && fileName.ends_with(suffixStr)) {
//                trianglelistFileNameList.push_back(fileName);
//            }
//        }
//
//        if (trianglelistFileNameList.size() == 0) {
//            LOG.Info(L"Can't find vb0 file for index: " + trianglelistIndex);
//            continue;
//        }
//
//        std::wstring trianglelistVB0FileName = trianglelistFileNameList[0];
//        LOG.Info(L"CurrentProcessing: " + trianglelistVB0FileName);
//        std::wstring trignalelistVB1FilePath = wheelConfigs.WorkFolder + trianglelistVB0FileName;
//        VertexBufferDetect vertexBufferDetect(trignalelistVB1FilePath);
//        LOG.Info(L"Buffer VertexNumber: " + std::to_wstring(vertexBufferDetect.fileBufferVertexCount));
//
//        if (vertexBufferDetect.fileBufferVertexCount >= maxTrianglelistVertexCountNumber) {
//            maxTrianglelistVertexCountNumber = vertexBufferDetect.fileBufferVertexCount;
//        }
//    }
//    LOG.NewLine();
//    LOG.Info(L"Max VB0 VertexNumber: " + std::to_wstring(maxTrianglelistVertexCountNumber));
//
//    std::wstring TexcoordFileIndex = L"";
//    std::wstring TexcoordSlot = d3D11ElementAttribute.ElementNameD3D11ElementMap[L"TEXCOORD"].ExtractSlot;
//    for (std::wstring trianglelistIndex : trianglelistIndexList) {
//        //根据Index获取所有的VB文件的列表
//        std::wstring searchStr = trianglelistIndex + L"-" + TexcoordSlot;
//        std::wstring suffixStr = L".txt";
//        std::vector<std::wstring> trianglelistFileNameList;
//        for (std::wstring fileName : FrameAnalyseFileNameList) {
//            if (fileName.find(searchStr) != std::string::npos && fileName.ends_with(suffixStr)) {
//                trianglelistFileNameList.push_back(fileName);
//            }
//        }
//
//        if (trianglelistFileNameList.size() == 0) {
//            LOG.Info(L"Can't find vb0 file for index: " + trianglelistIndex);
//            continue;
//        }
//
//        std::wstring trianglelistVB1FileName = trianglelistFileNameList[0];
//        LOG.Info(L"CurrentProcessing: " + trianglelistVB1FileName);
//        std::wstring trignalelistVB1FilePath = wheelConfigs.WorkFolder + trianglelistVB1FileName;
//        VertexBufferDetect vertexBufferDetect(trignalelistVB1FilePath);
//        LOG.Info(L"Buf VertexNumber: " + std::to_wstring(vertexBufferDetect.fileBufferVertexCount));
//
//        if (vertexBufferDetect.fileBufferVertexCount == maxTrianglelistVertexCountNumber) {
//
//            // 这里因为出现了步长为确实16，但是所有txt都只显示了Texcoord,而没有显示TEXCOORD1的情况。
//            // 尝试直接按原始IB进行处理所以无需匹配真实步长是否相等，因为txt里的数据可能是假的。
//            // 实际测试通过，buf文件中才是真实的数据，txt里展示的确实是假数据。
//            TexcoordFileIndex = trianglelistVB1FileName.substr(0, 6);
//            break;
//        }
//    }
//    
//    if (TexcoordFileIndex == L"") {
//        LOG.Error(L"Can't find texcoord file index!");
//    }
//    else {
//        LOG.Info(L"Texcoord FileIndex: " + TexcoordFileIndex);
//    }
//    
//    basicConfig.VertexLimitVB = MMTString_ToByteString(MMTFile_FindFrameAnalysisFileNameListWithCondition(wheelConfigs.WorkFolder,TexcoordFileIndex + L"-vb0", L".txt")[0].substr(11, 8));
//
//    std::wstring TexcoordVBSlot = d3D11ElementAttribute.ElementNameD3D11ElementMap[L"TEXCOORD"].ExtractSlot;
//    std::wstring TexcoordBufFileName = L"";
//    std::wstring IBBufFileName = L"";
//    for (std::wstring fileName : FrameAnalyseFileNameList) {
//        //只需要buf文件
//        if (!boost::algorithm::ends_with(fileName, L".buf")) {
//            continue;
//        }
//        //必须包含cs-t1
//        if (fileName.find(TexcoordFileIndex) != std::string::npos && fileName.find(L"-" + TexcoordVBSlot) != std::string::npos) {
//            TexcoordBufFileName = fileName;
//        }
//
//        if (fileName.find(TexcoordFileIndex) != std::string::npos && fileName.find(L"-ib=") != std::string::npos) {
//            IBBufFileName = fileName;
//        }
//
//        if (TexcoordBufFileName != L"" && IBBufFileName != L"") {
//            break;
//        }
//    }
//    LOG.NewLine();
//
//    //遍历FrameAnalysisFolder，查找使用CS 09d91b2eee7e232f 的cs-t*文件，其中cs-t0是Position文件，cs-t1是Blend文件，cs-t2是BoneMatrix文件
//    std::wstring computeIndex = L"";
//    //查找VertexNumber为当前最大值的那个cs-t0
//    for (std::wstring fileName : FrameAnalyseFileNameList) {
//        //只需要buf文件
//        if (!boost::algorithm::ends_with(fileName, L".buf")) {
//            continue;
//        }
//
//        //必须包含cs-t1
//        if (fileName.find(L"cs-t0") != std::string::npos && fileName.find(d3D11ElementAttribute.RootComputeShaderHash) != std::string::npos) {
//        //if (fileName.find(L"cs-t0") != std::string::npos ) {
//            LOG.Info(fileName);
//            try {
//                std::uintmax_t fileSize = std::filesystem::file_size(wheelConfigs.WorkFolder + fileName);
//
//                int vertexNumber = int(fileSize / 40);
//                LOG.Info(L"Vertex Number: " + std::to_wstring(vertexNumber));
//                if (vertexNumber == maxTrianglelistVertexCountNumber) {
//                    LOG.Info(L"Find ComputeCS index!");
//                    computeIndex = fileName.substr(0, 6);
//                }
//            }
//            catch (const std::filesystem::filesystem_error& /*ex*/) {
//                LOG.Info(L"Can't get file size." );
//            }
//        }
//    }
//    LOG.NewLine();
//
//    if (computeIndex != L"") {
//        LOG.Info(L"Find ComputeIndex: " + computeIndex);
//    }
//    else {
//        LOG.Error(L"Can't find ComputeIndex!");
//    }
//
//    //分别获取Position和Blend文件
//    std::wstring PositionBufFileName = L"";
//    std::wstring BlendBufFileName = L"";
//    for (std::wstring fileName : FrameAnalyseFileNameList) {
//        //只需要buf文件
//        if (!boost::algorithm::ends_with(fileName, L".buf")) {
//            continue;
//        }
//
//        //必须包含computeIndex
//        if (fileName.find(computeIndex) != std::string::npos && fileName.find(d3D11ElementAttribute.RootComputeShaderHash) != std::string::npos) {
//        //if (fileName.find(computeIndex) != std::string::npos) {
//            if (fileName.find(L"cs-t0") != std::string::npos) {
//                PositionBufFileName = fileName;
//            }
//            if (fileName.find(L"cs-t1") != std::string::npos) {
//                BlendBufFileName = fileName;
//            }
//        }
//
//        if (PositionBufFileName != L"" && BlendBufFileName != L"") {
//
//        }
//    }
//    LOG.Info(L"IBBufFileName: " + IBBufFileName);
//    LOG.Info(L"PositionBufFileName: " + PositionBufFileName);
//    LOG.Info(L"TexcoordBufFileName: " + TexcoordBufFileName);
//    LOG.Info(L"BlendBufFileName: " + BlendBufFileName);
//
//    //得到CategoryHash
//    basicConfig.CategoryHashMap["Position"] = MMTString_ToByteString(PositionBufFileName.substr(13, 8));
//    basicConfig.CategoryHashMap["Blend"] = MMTString_ToByteString(BlendBufFileName.substr(13, 8));
//    basicConfig.CategoryHashMap["Texcoord"] = MMTString_ToByteString(TexcoordBufFileName.substr(11, 8));
//
//    //得到MatchFirstIndex
//    std::vector<std::string> firstIndexList;
//    firstIndexList.push_back("0");
//    basicConfig.MatchFirstIndexList = firstIndexList;
//    LOG.NewLine();
//
//    //拼接Position Texcoord Blend文件为最终的vb文件
//
//    std::unordered_map<int, std::vector<std::byte>> PositionBufMap = MMTFile_ReadBufMapFromFile(wheelConfigs.WorkFolder + PositionBufFileName, maxTrianglelistVertexCountNumber);
//    std::unordered_map<int, std::vector<std::byte>> TexcoordBufMap = MMTFile_ReadBufMapFromFile(wheelConfigs.WorkFolder + TexcoordBufFileName, maxTrianglelistVertexCountNumber);
//    std::unordered_map<int, std::vector<std::byte>> BlendBufMap = MMTFile_ReadBufMapFromFile(wheelConfigs.WorkFolder + BlendBufFileName, maxTrianglelistVertexCountNumber);
//    LOG.Info(L"read finalvb0 buf success.");
//
//    std::vector<std::byte> finalVB0Buf;
//    for (int i = 0; i < maxTrianglelistVertexCountNumber; i++) {
//        std::vector<std::byte> positionTmpBuf = PositionBufMap[i];
//        finalVB0Buf.insert(finalVB0Buf.end(), positionTmpBuf.begin(), positionTmpBuf.end());
//
//        std::vector<std::byte> texcoordTmpBuf = TexcoordBufMap[i];
//        finalVB0Buf.insert(finalVB0Buf.end(), texcoordTmpBuf.begin(), texcoordTmpBuf.end());
//
//        std::vector<std::byte> blendTmpBuf = BlendBufMap[i];
//        finalVB0Buf.insert(finalVB0Buf.end(), blendTmpBuf.begin(), blendTmpBuf.end());
//    }
//
//    int PositionBufStride = int(PositionBufMap[0].size());
//    LOG.Info(L"PositionBufStride: " + std::to_wstring(PositionBufStride));
//    int TexcoordBufStride = int(TexcoordBufMap[0].size());
//    LOG.Info(L"TexcoordBufStride: " + std::to_wstring(TexcoordBufStride));
//    int BlendBufStride = int(BlendBufMap[0].size());
//    LOG.Info(L"BlendBufStride: " + std::to_wstring(BlendBufStride));
//
//    int AllStride = PositionBufStride + TexcoordBufStride + BlendBufStride;
//
//    //TODO 这里写死了是1，但是后面要考虑完善IB里有多个match_first_index的情况
//    std::ofstream outputVBFile(ModOutputFolder + basicConfig.DrawIB + L"-1" + L".vb", std::ofstream::binary);
//    outputVBFile.write(reinterpret_cast<const char*>(finalVB0Buf.data()), finalVB0Buf.size());
//    outputVBFile.close();
//    LOG.Info(L"VBFile Output success! all stride: " + std::to_wstring(AllStride));
//
//    //把IB文件复制一份过去
//    std::filesystem::copy_file(wheelConfigs.WorkFolder + IBBufFileName, ModOutputFolder + basicConfig.DrawIB + L"-1" + L".ib", std::filesystem::copy_options::overwrite_existing);
//
//    //输出一份固定的D3D11Element的fmt文件
//    std::wstring outputFmtFileName = ModOutputFolder + basicConfig.DrawIB + L"-1" + L".fmt";
//    std::wofstream outputFmtFile(outputFmtFileName);
//
//    std::vector<std::string> orderedBlenderElementList;
//    std::vector<std::string> orderedRealElementList;
//
//    if (PositionBufStride == 40) {
//        orderedBlenderElementList.push_back("POSITION");
//        orderedBlenderElementList.push_back("NORMAL");
//        orderedBlenderElementList.push_back("TANGENT");
//    }
//    else {
//        LOG.Error(L"Unsupported PositionBufStride: " + std::to_wstring(PositionBufStride));
//    }
//
//    if (TexcoordBufStride == 8) {
//        orderedBlenderElementList.push_back("TEXCOORD");
//    }
//    else if (TexcoordBufStride == 12) {
//        orderedBlenderElementList.push_back("COLOR");
//        orderedBlenderElementList.push_back("TEXCOORD");
//    }
//    else if (TexcoordBufStride == 16) {
//        orderedBlenderElementList.push_back("TEXCOORD");
//        orderedBlenderElementList.push_back("TEXCOORD1");
//    }
//    else if (TexcoordBufStride == 20) {
//        orderedBlenderElementList.push_back("COLOR");
//        orderedBlenderElementList.push_back("TEXCOORD");
//        orderedBlenderElementList.push_back("TEXCOORD1");
//    }
//    else {
//        LOG.Error(L"Unsupported TexcoordBufStride: " + std::to_wstring(TexcoordBufStride));
//    }
//
//    if (BlendBufStride == 32) {
//        orderedBlenderElementList.push_back("BLENDWEIGHTS");
//        orderedBlenderElementList.push_back("BLENDINDICES");
//    }
//    else if (BlendBufStride == 20) {
//        orderedBlenderElementList.push_back("BLENDWEIGHTS");
//        orderedBlenderElementList.push_back("BLENDINDICES");
//    }
//    else if (BlendBufStride == 16) {
//        orderedBlenderElementList.push_back("BLENDWEIGHTS");
//        orderedBlenderElementList.push_back("BLENDINDICES");
//    }
//    else {
//        LOG.Error(L"Unsupported BlendBufStride: " + std::to_wstring(BlendBufStride));
//    }
//
//
//    outputFmtFile << "stride: " << std::to_wstring(AllStride) << std::endl;
//    outputFmtFile << "topology: trianglelist" << std::endl;
//    outputFmtFile << "format: DXGI_FORMAT_R16_UINT" << std::endl;
//
//    int elementNumber = 0;
//    int alignedByteOffset = 0;
//    for (std::string elementName : orderedBlenderElementList) {
//        D3D11Element elementObject = d3D11ElementAttribute.ElementNameD3D11ElementMap[MMTString_ToWideString(elementName)];
//        outputFmtFile << "element[" << elementNumber << "]:" << std::endl;
//        outputFmtFile << "  SemanticName: " << elementObject.SemanticName << std::endl;
//        outputFmtFile << "  SemanticIndex: " << elementObject.SemanticIndex << std::endl;
//        outputFmtFile << "  Format: " << elementObject.Format << std::endl;
//        outputFmtFile << "  InputSlot: " << elementObject.InputSlot << std::endl;
//        outputFmtFile << "  AlignedByteOffset: " << alignedByteOffset << std::endl;
//        alignedByteOffset = alignedByteOffset + elementObject.ByteWidth;
//        outputFmtFile << "  InputSlotClass: " << elementObject.InputSlotClass << std::endl;
//        outputFmtFile << "  InstanceDataStepRate: " << elementObject.InstanceDataStepRate << std::endl;
//        elementNumber = elementNumber + 1;
//    }
//
//    outputFmtFile.close();
//    LOG.Info(L"output fmt file over");
//
//    std::vector<std::string> partNameList;
//    partNameList.push_back("1");
//    basicConfig.PartNameList = partNameList;
//
//    //移动所有贴图文件到对应文件夹
//    MMTFile_MoveAllTextures(wheelConfigs.WorkFolder, basicConfig.DrawIB, ModOutputFolder);
//
//    //save config json
//    basicConfig.TmpElementList = orderedBlenderElementList;
//    basicConfig.saveTmpConfigs(wheelConfigs.OutputFolder);
//
//
//    LOG.NewLine();
//
//}
//
//
//
