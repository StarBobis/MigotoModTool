#include "IniParser.h"
#include "MMTStringUtils.h"
#include "MMTFormatUtils.h"
#include "MMTFileUtils.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"

std::vector<TextureOverrideIB> parseTextureOverrideIBList(std::vector<std::wstring> lines) {
    std::vector<TextureOverrideIB> TextureOverrideIBList;
    bool matchTextureOverride = false;
    bool matchTextureOverrideIB = false;
    bool matchFirstIndex = false;
    TextureOverrideIB tmpTextureOverrideIB;

    for (std::wstring line : lines) {
        boost::algorithm::trim(line);
        std::wstring lowerLine = boost::algorithm::to_lower_copy(line);

        if (boost::algorithm::starts_with(lowerLine, ";")) {
            continue;
        }

        if (lowerLine.empty()) {
            continue;
        }

        if (boost::algorithm::starts_with(lowerLine, "[textureoverride")) {
            matchTextureOverride = true;
            tmpTextureOverrideIB = TextureOverrideIB();
            tmpTextureOverrideIB.SectionName = MMTString_RemoveSquareBrackets(lowerLine);
            continue;
        }

        if (matchTextureOverride && boost::algorithm::starts_with(lowerLine, "hash")) {
            KeyValuePair kvp(line);
            std::wstring hashValue = kvp.value;

            tmpTextureOverrideIB.HashValue = hashValue;
            continue;
        }

        if (matchTextureOverride && boost::algorithm::starts_with(lowerLine, "vb")) {
            KeyValuePair kvp(line);
            std::wstring vbSlot = kvp.key;
            std::wstring vbResourceName = kvp.value;

            continue;
        }


        if (matchTextureOverride && boost::algorithm::starts_with(lowerLine, "match_first_index")) {
            KeyValuePair kvp(line);
            std::wstring matchFirstIndex = kvp.value;


            try {
                int num = boost::lexical_cast<int>(matchFirstIndex);
                tmpTextureOverrideIB.MatchFirstIndex = std::stoi(matchFirstIndex);
                matchFirstIndex = true;
            }
            catch (const boost::bad_lexical_cast&) {

                continue;
            }

            continue;

        }

        if (boost::algorithm::starts_with(lowerLine, "ib")) {
            KeyValuePair kvp(line);
            std::wstring ibResourceName = kvp.value;
            tmpTextureOverrideIB.IBResourceName = ibResourceName;

            if (matchFirstIndex) {
                matchFirstIndex = false;
            }
            matchTextureOverride = false;

            if (ibResourceName != L"null" && ibResourceName != L"") {
                TextureOverrideIBList.push_back(tmpTextureOverrideIB);
            }

            continue;

        }
    }

    return TextureOverrideIBList;
}


std::vector<ModResource> parseResourceBufferList(std::vector<std::wstring> lines, std::wstring reverseIniFolderPath) {
    std::vector<ModResource> ResourceVBList;

    bool matchResource = false;
    bool matchResourceVB = false;
    bool matchResourceIB = false;

    ModResource tmpResource;

    for (std::wstring line : lines) {
        boost::algorithm::trim(line);
        std::wstring lowerLine = boost::algorithm::to_lower_copy(line);

        if (boost::algorithm::starts_with(lowerLine, ";")) {
            continue;
        }

        if (lowerLine.empty()) {
            continue;
        }

        if (boost::algorithm::starts_with(lowerLine, "[resource")) {
            matchResource = true;
            tmpResource = ModResource();
            tmpResource.SectionName = MMTString_RemoveSquareBrackets(line);
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "stride")) {
            KeyValuePair kvp(line);
            tmpResource.Stride = std::stoi(kvp.value);
            matchResourceVB = true;
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "format")) {
            KeyValuePair kvp(line);
            tmpResource.Format = kvp.value;
            matchResourceIB = true;
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "filename")) {
            KeyValuePair kvp(line);
            if (boost::algorithm::starts_with(kvp.value, L".")) {
                std::wstring relativePath = kvp.value.substr(1, kvp.value.length() - 1);
                boost::algorithm::replace_all(relativePath, L"/", L"\\");
                kvp.value = reverseIniFolderPath + relativePath;
            }
            else {
                std::wstring relativePath = kvp.value;
                boost::algorithm::replace_all(relativePath, L"/", L"\\");
                kvp.value = reverseIniFolderPath + L"\\" + relativePath;
            }
            tmpResource.FileName = kvp.value;
        }

        if (matchResourceVB && !tmpResource.FileName.empty() && tmpResource.FileName != L"") {
            matchResourceVB = false;
            ResourceVBList.push_back(tmpResource);
        }


    }
    return ResourceVBList;
}


std::vector<ModResource> parseResourceIBList(std::vector<std::wstring> lines,std::wstring reverseIniFolderPath) {
    std::vector<ModResource> ResourceIBList;

    bool matchResource = false;
    bool matchResourceVB = false;
    bool matchResourceIB = false;
    TextureOverrideIB tmpTextureOverrideIB;
    ModResource tmpResource;

    for (std::wstring line : lines) {
        boost::algorithm::trim(line);
        std::wstring lowerLine = boost::algorithm::to_lower_copy(line);

        if (boost::algorithm::starts_with(lowerLine, ";")) {
            continue;
        }

        if (lowerLine.empty()) {
            continue;
        }
        //LOG.LogOutput(L"Current line: " + line);

        if (boost::algorithm::starts_with(lowerLine, "[resource")) {
            matchResource = true;
            tmpResource = ModResource();
            tmpResource.SectionName = MMTString_RemoveSquareBrackets(line);
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "stride")) {
            KeyValuePair kvp(line);
            tmpResource.Stride = std::stoi(kvp.value);
            matchResourceVB = true;
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "format")) {
            KeyValuePair kvp(line);
            tmpResource.Format = kvp.value;
            matchResourceIB = true;
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "filename")) {
            KeyValuePair kvp(line);
            if (boost::algorithm::starts_with(kvp.value, L".")) {
                std::wstring relativePath = kvp.value.substr(1, kvp.value.length() - 1);
                boost::algorithm::replace_all(relativePath, L"/", L"\\");
                kvp.value = reverseIniFolderPath + relativePath;
            }
            else {
                std::wstring relativePath = kvp.value;
                boost::algorithm::replace_all(relativePath, L"/", L"\\");
                kvp.value = reverseIniFolderPath + L"\\" + relativePath;
            }
            tmpResource.FileName = kvp.value;
        }

        if (matchResourceIB && !tmpResource.FileName.empty() && tmpResource.FileName != L"") {
            matchResourceIB = false;
            ResourceIBList.push_back(tmpResource);
        }

    }
    return ResourceIBList;
}


std::vector<ModResource> parseResourceAllList(std::vector<std::wstring> lines, std::wstring reverseIniFolderPath) {
    std::vector<ModResource> ResourceList;

    bool matchResource = false;
    ModResource tmpResource;

    for (std::wstring line : lines) {
        boost::algorithm::trim(line);
        std::wstring lowerLine = boost::algorithm::to_lower_copy(line);

        if (boost::algorithm::starts_with(lowerLine, ";")) {
            continue;
        }
        if (lowerLine.empty()) {
            continue;
        }

        if (boost::algorithm::starts_with(lowerLine, "[resource")) {
            matchResource = true;
            tmpResource = ModResource();
            tmpResource.SectionName = MMTString_RemoveSquareBrackets(line);
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "stride")) {
            KeyValuePair kvp(line);
            tmpResource.Stride = std::stoi(kvp.value);
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "format")) {
            KeyValuePair kvp(line);
            tmpResource.Format = kvp.value;
        }

        if (matchResource && boost::algorithm::starts_with(lowerLine, "filename")) {
            KeyValuePair kvp(line);
            if (boost::algorithm::starts_with(kvp.value, L".")) {
                std::wstring relativePath = kvp.value.substr(1, kvp.value.length() - 1);
                boost::algorithm::replace_all(relativePath, L"/", L"\\");
                kvp.value = reverseIniFolderPath + relativePath;
            }
            else {
                std::wstring relativePath = kvp.value;
                boost::algorithm::replace_all(relativePath, L"/", L"\\");
                kvp.value = reverseIniFolderPath + L"\\" + relativePath;
            }
            tmpResource.FileName = kvp.value;
        }

        if (!tmpResource.FileName.empty() && tmpResource.FileName != L"") {
            ResourceList.push_back(tmpResource);
        }

    }
    return ResourceList;
}


std::vector<DrawIBMod> parseDrawIBModList(
    std::vector<TextureOverrideIB> TextureOverrideIBList,
    std::vector<ModResource> ResourceVBList,
    std::vector<ModResource> ResourceIBList) {

    std::vector<DrawIBMod> drawIBModList;

    //给TextureOverrideIB赋予对应的FileName和Format
    for (TextureOverrideIB& textureOverrideIB : TextureOverrideIBList) {
        for (ModResource resourceIB : ResourceIBList) {
            std::wstring ibResourceNameLower = boost::algorithm::to_lower_copy(textureOverrideIB.IBResourceName);
            std::wstring ibResourceSectionNameLower = boost::algorithm::to_lower_copy(resourceIB.SectionName);

            if (ibResourceNameLower == ibResourceSectionNameLower) {
                textureOverrideIB.IBFileFormat = resourceIB.Format;
                textureOverrideIB.IBFileName = resourceIB.FileName;
                break;
            }
        }
    }
    //LOG.LogOutput(L"TextureOverrideIBList:");
    //for (TextureOverrideIB obj : TextureOverrideIBList) {
    //    obj.show();
    //}
    //LOG.LogOutputSplitStr();


    std::unordered_map<std::wstring, std::vector<TextureOverrideIB>> hashTextureOverrideIBListMap;
    for (TextureOverrideIB obj : TextureOverrideIBList) {
        std::vector<TextureOverrideIB> tmpTextureOverrideIBList = hashTextureOverrideIBListMap[obj.HashValue];
        tmpTextureOverrideIBList.push_back(obj);
        hashTextureOverrideIBListMap[obj.HashValue] = tmpTextureOverrideIBList;
    }
    LOG.NewLine();

    LOG.Info(L"Start to generate order TextureOverrideIB, make sure from lower to bigger.");
    std::unordered_map<std::wstring, std::vector<TextureOverrideIB>> orderedHashTextureOverrideIBListMap;
    for (const auto& pair : hashTextureOverrideIBListMap) {
        std::wstring hashValue = pair.first;
        std::vector<TextureOverrideIB> tmpTextureOverrideIBList = pair.second;
        LOG.Info(L"Current Processing HaseValue: " + hashValue);

        std::map<int, std::wstring> textureOverrideSectionNameMap;
        std::map<std::wstring, TextureOverrideIB> SectionNameTextureOverrideMap;

        for (TextureOverrideIB ibObject : tmpTextureOverrideIBList) {
            std::wstring sectionName = ibObject.SectionName;
            IndexBufferBufFile ibBufFile(ibObject.IBFileName, ibObject.IBFileFormat);

            ibObject.minNumber = ibBufFile.MinNumber;
            ibObject.maxNumber = ibBufFile.MaxNumber;
            ibObject.indexCount = ibBufFile.NumberCount;
            ibObject.vertexCount = ibBufFile.UniqueVertexCount;
            SectionNameTextureOverrideMap[sectionName] = ibObject;

            LOG.Info(L"sectionName: " + sectionName);
            LOG.Info(L"minNumber: " + std::to_wstring(ibBufFile.MinNumber));
            LOG.Info(L"indexCount: " + std::to_wstring(ibBufFile.NumberCount));
            LOG.Info(L"vertexCount: " + std::to_wstring(ibBufFile.UniqueVertexCount));
            textureOverrideSectionNameMap[ibBufFile.MinNumber] = sectionName;
        }

        std::vector<TextureOverrideIB> TextureOverrideIBListWithFileNameOrdered;

        for (std::map<int, std::wstring>::iterator it = textureOverrideSectionNameMap.begin(); it != textureOverrideSectionNameMap.end(); ++it) {
            int minNumber = it->first;
            std::wstring sectionName = it->second;
            TextureOverrideIB ibObject = SectionNameTextureOverrideMap[sectionName];
            TextureOverrideIBListWithFileNameOrdered.push_back(ibObject);
        }

        orderedHashTextureOverrideIBListMap[hashValue] = TextureOverrideIBListWithFileNameOrdered;
    }
    hashTextureOverrideIBListMap = orderedHashTextureOverrideIBListMap;


    LOG.Info(L"Output the final TextureOverrideIBList:");
    for (const auto& pair : hashTextureOverrideIBListMap) {
        std::wstring hashValue = pair.first;
        LOG.Info(L"Current HaseValue:" + hashValue);
        std::vector<TextureOverrideIB> tmpTextureOverrideIBList = pair.second;
        for (TextureOverrideIB obj : tmpTextureOverrideIBList) {
            obj.show();
        }
        LOG.NewLine();
    }


    LOG.NewLine();


    //根据Resource的顶点数来区分
    LOG.Info(L"Split ResourceVB based on different size.");
    std::unordered_map<int, std::vector<ModResource>> sizeResourceVBListMap;
    for (ModResource resourceVB : ResourceVBList) {
        std::wstring ResourceBufFilePath = resourceVB.FileName;
        int fileSize = MMTFile_GetFileSize(ResourceBufFilePath);
        int resourceVBVertexNumber = fileSize / resourceVB.Stride;

        LOG.Info(L"FilePath: " + ResourceBufFilePath + L"Vertex Number:" + std::to_wstring(resourceVBVertexNumber));

        std::vector<ModResource> tmpResourceVBList = sizeResourceVBListMap[resourceVBVertexNumber];
        tmpResourceVBList.push_back(resourceVB);
        sizeResourceVBListMap[resourceVBVertexNumber] = tmpResourceVBList;

    }
    LOG.NewLine();
    //LOG.LogOutput(L"Output and see the result:");
    //for (const auto& pair : sizeResourceVBListMap) {
    //    int size = pair.first;
    //    std::vector<ModResource> tmpResourceVBList = pair.second;
    //    LOG.LogOutput(L"Current size: " + std::to_wstring(size));
    //    for (ModResource tmpResource : tmpResourceVBList) {
    //        tmpResource.show();
    //    }
    //    LOG.LogOutputSplitStr();
    //}
    //LOG.LogOutputSplitStr();

    LOG.Info(L"Start to combine DrawIBModList:");
    LOG.Info(L"hashTextureOverrideIBListMap size: " + std::to_wstring(hashTextureOverrideIBListMap.size()));
    LOG.Info(L"Use VertexNumber to match between TextureOverrideIB and ResourceVB");
    int shouldMatchCount = 0;
    for (const auto& pair : hashTextureOverrideIBListMap) {
        std::wstring hashValue = pair.first;
        LOG.Info(L"Current HaseValue:" + hashValue);
        std::vector<TextureOverrideIB> tmpTextureOverrideIBList = pair.second;

        //这里使用多个IB文件中唯一的数值的总数之和作为此Mod总顶点数
        int TotalVertexCount = 0;
        for (TextureOverrideIB obj : tmpTextureOverrideIBList) {
            TotalVertexCount = TotalVertexCount + obj.vertexCount;
        }
        LOG.Info(L"DrawIB Vertex Count number: " + std::to_wstring(TotalVertexCount));
        LOG.NewLine();
        DrawIBMod drawIBMod;
        drawIBMod.hash = hashValue;
        drawIBMod.textureOverrideIBList = tmpTextureOverrideIBList;
        for (TextureOverrideIB obj : tmpTextureOverrideIBList) {
            obj.show();
        }

        drawIBMod.resourceVBList = sizeResourceVBListMap[TotalVertexCount];
        for (ModResource obj : drawIBMod.resourceVBList) {
            obj.show();
        }

        if (drawIBMod.resourceVBList.size() > 0) {
            drawIBModList.push_back(drawIBMod);
            shouldMatchCount++;
        }
        else {
            LOG.Warning(L"Can't find any match ResourceVB for hash: " + hashValue);
        }
    }
    LOG.Info(L"Combine DrawIBModList success.");
    LOG.NewLine();
    return drawIBModList;
}


ReverseModelSingle::ReverseModelSingle(std::wstring reverseFilePath) {
    this->iniFilePath = reverseFilePath;
    std::wstring reverseIniFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath);
    this->iniFolderPath = reverseIniFolderPath;
    this->outputFolderPath = reverseIniFolderPath + L"\\reverse\\";
    std::filesystem::create_directories(outputFolderPath);
    LOG.Info(L"Start to reverse Mod: " + reverseFilePath);
    LOG.Info(L"Reverse mod folder: " + reverseIniFolderPath);
    LOG.Info(L"Reverse output folder: " + outputFolderPath);
    std::vector<std::wstring> lines = MMTFile_ReadAllLinesW(reverseFilePath);

    std::vector<TextureOverrideIB> TextureOverrideIBList = parseTextureOverrideIBList(lines);
    std::vector<ModResource> ResourceVBList = parseResourceBufferList(lines,reverseIniFolderPath);
    std::vector<ModResource> ResourceIBList = parseResourceIBList(lines,reverseIniFolderPath);

    this->drawIBModList = parseDrawIBModList(
        TextureOverrideIBList,
        ResourceVBList,
        ResourceIBList
    );
    
    LOG.NewLine();

}


void ReverseModelSingle::reverseAndOutput() {

    std::vector<ReversedObject> matchReversedObjectList = this->reversedObjectList;
    int reverseCounts = 0;
    for (ReversedObject reversedObject : matchReversedObjectList) {
        reverseCounts++;
        std::wstring outputPath = reversedObject.outputFolderGameTypePath + std::to_wstring(reverseCounts) + L"\\";
        std::filesystem::create_directories(outputPath);
        LOG.Info(L"Create output folder:" + outputPath);

        //输出IB文件
        int count = 0;
        for (TextureOverrideIB ibObject : reversedObject.textureOverrideIBList) {
            std::wstring outputIBFileName = outputPath + L"body_part" + std::to_wstring(count) + L".ib";
            IndexBufferBufFile ibBufFile(ibObject.IBFileName, ibObject.IBFileFormat);
            ibBufFile.SaveToFile_UINT32(outputIBFileName, ibBufFile.MinNumber * -1);
            count = count + 1;
        }
        LOG.NewLine();


        //重新排序ElementList
        std::vector<std::string> orderedElementList = reversedObject.d3d11GameType.getReorderedElementList(reversedObject.elementList);
        //输出FMT文件
        for (int i = 0; i < count; i++) {
            FmtFileData fmtFileData;
            fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
            fmtFileData.ElementNameList = orderedElementList;
            fmtFileData.d3d11GameType = reversedObject.d3d11GameType;
            fmtFileData.OutputFmtFile(outputPath + L"body_part" + std::to_wstring(i) + L".fmt");
        }
        LOG.Info(L"output fmt file over");
        LOG.NewLine();

        //组合最终的VB0文件
        LOG.Info(L"read vb file ,combine final file");
        std::unordered_map<int, std::vector<byte>> vb0IndexNumberBytes;
        for (ModResource resourceVBObject : reversedObject.resourceVBList) {
            LOG.Info(L"Show the resourceVBObject before read and output to vb file.");
            resourceVBObject.show();
            if (resourceVBObject.FileName == L"" || resourceVBObject.FileName.empty()) {
                LOG.Info(L"Can't find any FileNam in this ResourceVB.");
                continue;
            }
            LOG.Info(L"current processing vb file: " + resourceVBObject.FileName);
            std::wstring resourceVBFileName = resourceVBObject.FileName;
            int resourceVBStride = resourceVBObject.Stride;
            std::wstring resourceVBFilePath = resourceVBFileName;

            LOG.Info(L"current processing vb file path: " + resourceVBFilePath);

            std::ifstream resourceVBFile(resourceVBFilePath, std::ios::binary);
            resourceVBFile.seekg(0, std::ios::end);
            std::streampos resourceVBFileSize = resourceVBFile.tellg();
            resourceVBFile.seekg(0, std::ios::beg);
            LOG.Info(L"Current processing vb file size: " + std::to_wstring(resourceVBFileSize));

            int resourceVBVertexCount = (int)resourceVBFileSize / resourceVBStride;
            LOG.Info(L"Get vertex count number: " + std::to_wstring(resourceVBVertexCount));

            for (int i = 1; i <= resourceVBVertexCount; i++) {
                std::vector<byte> bytes(resourceVBStride);
                resourceVBFile.read(reinterpret_cast<char*>(bytes.data()), resourceVBStride);
                vb0IndexNumberBytes[i].insert(vb0IndexNumberBytes[i].end(), bytes.begin(), bytes.end());
            }
            resourceVBFile.close();
        }
        LOG.Info(L"Read vb file finished, combined to final format.");
        LOG.NewLine();


        //计算stride长度
        int stride = reversedObject.d3d11GameType.getElementListStride(reversedObject.elementList);
        //核心：根据IB文件的最大值和最小值拆分输出VB0文件，为了让顶点索引能够匹配到正确位置的顶点
        LOG.NewLine();
        for (int i = 0; i < count; i++) {
            std::wstring ibFileName = reversedObject.textureOverrideIBList[i].IBFileName;
            int ibMaxNumber = reversedObject.textureOverrideIBList[i].maxNumber;
            int ibMinNumber = reversedObject.textureOverrideIBList[i].minNumber;
            int readNumberOffset = ibMinNumber + 1;
            
            LOG.Info(L"ibFileName: " + ibFileName + L"  ibMaxNumber: " + std::to_wstring(ibMaxNumber));
            LOG.Info(L"ibFileName: " + ibFileName + L"  ibMinNumber: " + std::to_wstring(ibMinNumber));

            std::vector<byte> outputBytes;
            for (readNumberOffset; readNumberOffset <= ibMaxNumber + 1; readNumberOffset++) {
                std::vector<byte> indexBytes = vb0IndexNumberBytes[readNumberOffset];
                outputBytes.insert(outputBytes.end(), indexBytes.begin(), indexBytes.end());
            }

            std::wstring outputVBFilePath = outputPath + L"body_part" + std::to_wstring(i) + L".vb";
            LOG.Info(L"vbFileName: " + outputVBFilePath);
            LOG.Info(L"vbFileSize: " + std::to_wstring(outputBytes.size() / stride));

            if (outputBytes.size() == 0) {
                LOG.Info(L"Can't output because size is 0.");
            }
            else {
                std::ofstream outputVBFile(outputVBFilePath, std::ios::binary);
                LOG.Info(L"Start to output to folder.");
                outputVBFile.write(reinterpret_cast<const char*>(outputBytes.data()), outputBytes.size());
                outputVBFile.close();
            }
        }
        LOG.NewLine();
    }
    LOG.Info(L"Reverse completed!");
}


void ReverseModelSingle::parseElementList(std::wstring gameName) {
    LOG.Info(L"Start to parse ElementList, first initialize config");
    std::vector<D3D11GameType> d3d11ElementAttributeList = G.CurrentD3d11GameTypeList;
    LOG.Info(L"Initailize D3D11ElementList completed，d3d11ElementAttribute size: " + std::to_wstring(d3d11ElementAttributeList.size()));
    LOG.NewLine();

    for (DrawIBMod drawIBMod : drawIBModList) {
        std::wstring hashValue = drawIBMod.hash;
        std::vector<TextureOverrideIB> textureOverrideIBList = drawIBMod.textureOverrideIBList;
        std::vector<ModResource> ResourceVBList = drawIBMod.resourceVBList;
        LOG.Info(L"Current processing DrawIBMod Hash value: " + hashValue);
        LOG.Info(L"textureOverrideIBList size: " + std::to_wstring(textureOverrideIBList.size()));
        LOG.Info(L"ResourceVBList size: " + std::to_wstring(ResourceVBList.size()));
        if (ResourceVBList.size() == 0) {
            LOG.Warning(L"Can't find any ResourceVB in this DrawIB hash.");
        }
        LOG.NewLine();

        std::vector<ReversedObject> matchReversedObjectList;

        int possibleModelNumber = 0;
        for (D3D11GameType d3d11GameType : d3d11ElementAttributeList) {
            if (d3d11GameType.CategorySlotMap.size() != ResourceVBList.size()) {
                LOG.Info("Try to parse with type:" + d3d11GameType.GameType + " but can't match category size.");
                continue;
            }
            else {
                possibleModelNumber++;
            }
            LOG.Info("Current detecting type: " + d3d11GameType.GameType);
            std::wstring outputFolderGameTypePath = outputFolderPath + hashValue + L"\\" + MMTString_ToWideString(d3d11GameType.GameType) + L"_";

            if (ResourceVBList.size() == 3) {

                ModResource ResourcePosition;
                ModResource ResourceTexcoord;
                ModResource ResourceBlend;

                std::vector<std::string> elementList;
                std::vector<ModResource> vbResourceOrderedList;

                LOG.Info(L"Detect classic Position,Texcoord,Blend mod.");

                std::unordered_map<std::string, int> categoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
                int positionStrie = categoryStrideMap["Position"];

                std::vector <std::string> positionElementListTmp = d3d11GameType.getCategoryElementList(d3d11GameType.OrderedFullElementList,"Position");
            
                LOG.Info(L"Start to load Position category's element list:");
                LOG.Info(L"Position stride:" + std::to_wstring(positionStrie));

                for (ModResource resourceVB : ResourceVBList) {
                    int stride = resourceVB.Stride;
                    LOG.Info(L"stride:" + std::to_wstring(stride));
                    std::wstring resourceFileName = resourceVB.FileName;
                    LOG.Info(L"resourceFileName:" + resourceFileName);

                    //POSITION
                    if (stride == positionStrie) {
                        ResourcePosition = resourceVB;
                        /*elementList.push_back(L"POSITION");
                        elementList.push_back(L"NORMAL");
                        elementList.push_back(L"TANGENT");*/
                        elementList.insert(elementList.end(), positionElementListTmp.begin(), positionElementListTmp.end());
                        break;

                    }
                }

                int colorByteWidth = d3d11GameType.ElementNameD3D11ElementMap["COLOR"].ByteWidth;
                int texcoordByteWidth = d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD"].ByteWidth;
                int texcoord1ByteWidth = d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD1"].ByteWidth;
                LOG.Info(L"Start to load TEXCOORD category's element list:");

                for (ModResource resourceVB : ResourceVBList) {
                    int stride = resourceVB.Stride;
                    std::wstring resourceFileName = resourceVB.FileName;
                    //TEXCOORD
                    if (stride == texcoordByteWidth) {
                        ResourceTexcoord = resourceVB;
                        elementList.push_back("TEXCOORD");
                        break;

                    }
                    if (stride == colorByteWidth + texcoordByteWidth) {
                        ResourceTexcoord = resourceVB;
                        elementList.push_back("COLOR");
                        elementList.push_back("TEXCOORD");
                        break;

                    }
                    if (stride == colorByteWidth + texcoordByteWidth + texcoord1ByteWidth) {
                        ResourceTexcoord = resourceVB;
                        elementList.push_back("COLOR");
                        elementList.push_back("TEXCOORD");
                        elementList.push_back("TEXCOORD1");
                        break;

                    }
                }
                LOG.NewLine();

                LOG.Info(L"Start to load Blend category's element list:");

                for (ModResource resourceVB : ResourceVBList) {
                    LOG.Info(L"Current processing ResourceVB: " + resourceVB.FileName);

                    int stride = resourceVB.Stride;
                    std::wstring resourceFileName = resourceVB.FileName;

                    std::vector<std::wstring> blendElementList;

                    for (const auto& pair : d3d11GameType.ElementNameD3D11ElementMap) {
                        std::string elementName = pair.first;
                        D3D11Element d3d11Element = pair.second;
                        if (d3d11Element.Category == "Blend") {
                            blendElementList.push_back(MMTString_ToWideString(elementName));
                        }
                    }

                    int blendStride = 0;
                    for (std::wstring blendElementName : blendElementList) {
                        int byteWidth = d3d11GameType.ElementNameD3D11ElementMap[MMTString_ToByteString(blendElementName)].ByteWidth;
                        blendStride += byteWidth;
                    }
                    LOG.Info(L"Blend stride:" + std::to_wstring(blendStride));

                    //BLEND
                    if (stride == blendStride) {
                        ResourceBlend = resourceVB;

                        if (boost::algorithm::any_of_equal(blendElementList, L"BLENDWEIGHT")) {
                            elementList.push_back("BLENDWEIGHT");
                        }
                        if (boost::algorithm::any_of_equal(blendElementList, L"BLENDWEIGHTS")) {
                            elementList.push_back("BLENDWEIGHTS");
                        }
                        elementList.push_back("BLENDINDICES");
                        break;
                    }
                }
                LOG.NewLine();


                /*LOG.LogOutput(L"current Element list detected");
                for (std::wstring elementName : elementList) {
                    LOG.LogOutput(elementName);
                }*/

                vbResourceOrderedList.push_back(ResourcePosition);
                vbResourceOrderedList.push_back(ResourceTexcoord);
                vbResourceOrderedList.push_back(ResourceBlend);

                ReversedObject reversedObject;
                reversedObject.elementList = elementList;
                reversedObject.d3d11GameType = d3d11GameType;
                reversedObject.outputFolderGameTypePath = outputFolderGameTypePath;
                reversedObject.textureOverrideIBList = textureOverrideIBList;
                reversedObject.resourceVBList = vbResourceOrderedList;

                matchReversedObjectList.push_back(reversedObject);
            }
            else if (ResourceVBList.size() == 1) {
                int bufStride = 0;
                std::vector<std::string> elementList;
                LOG.Info(L"Detect only one ResourceVB, maybe this is a object mod.");
                ModResource resourceVBObject = ResourceVBList[0];

                elementList.clear();
                bufStride = 0;
                for (const auto& pair : d3d11GameType.ElementNameD3D11ElementMap) {
                    std::string elementName = pair.first;
                    D3D11Element d3d11Element = pair.second;
                    int byteWidth = d3d11Element.ByteWidth;
                    bufStride += byteWidth;
                    elementList.push_back(elementName);
                    LOG.Info("Current ElementName: " + elementName + " ByteWidth: " + std::to_string(byteWidth));
                }
                if (resourceVBObject.Stride == bufStride) {

                    std::vector<ModResource> vbResourceOrderedList;
                    vbResourceOrderedList.push_back(resourceVBObject);
                    ReversedObject reversedObject;
                    reversedObject.elementList = elementList;
                    reversedObject.d3d11GameType = d3d11GameType;
                    reversedObject.outputFolderGameTypePath = outputFolderGameTypePath;
                    reversedObject.textureOverrideIBList = textureOverrideIBList;
                    reversedObject.resourceVBList = vbResourceOrderedList;
                    matchReversedObjectList.push_back(reversedObject);
                }

                elementList.clear();
                bufStride = 0;
                for (const auto& pair : d3d11GameType.ElementNameD3D11ElementMap) {
                    std::string elementName = pair.first;
                    if (elementName == "TEXCOORD1") {
                        continue;
                    }
                    D3D11Element d3d11Element = pair.second;
                    int byteWidth = d3d11Element.ByteWidth;
                    bufStride += byteWidth;
                    elementList.push_back(elementName);
                    LOG.Info("Current ElementName: " + elementName + " ByteWidth: " + std::to_string(byteWidth));
                }
                if (resourceVBObject.Stride == bufStride) {
                    std::vector<ModResource> vbResourceOrderedList;
                    vbResourceOrderedList.push_back(resourceVBObject);
                    ReversedObject reversedObject;
                    reversedObject.elementList = elementList;
                    reversedObject.d3d11GameType = d3d11GameType;
                    reversedObject.outputFolderGameTypePath = outputFolderGameTypePath;
                    reversedObject.textureOverrideIBList = textureOverrideIBList;
                    reversedObject.resourceVBList = vbResourceOrderedList;
                    matchReversedObjectList.push_back(reversedObject);
                }



            }

            LOG.Info(L"Parse ElementList completed!");

            this->reversedObjectList = matchReversedObjectList;
            reverseAndOutput();
        }


        if (possibleModelNumber == 0) {
            LOG.Error(L"Can't match any D3D11Element type,please check if it's a valid mod,if it's valid then contact NicoMico for help.");
        }

    }



}

