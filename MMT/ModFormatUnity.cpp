#include "ModFormatUnity.h"
#include "MMTFileUtils.h"
#include <boost/algorithm/cxx11/any_of.hpp>
#include "IndexBufferBufFile.h"



void ModFormat_Unity_INI::Parse_VertexNumberMResourceVBMap() {
    //1.获取所有的ResourceVB
    //根据Resource的顶点数来区分不同的Resource合集，相同顶点数的Resource在相同的Mod里
    std::unordered_map<uint32_t, std::vector<M_Resource>> vertexNumberMResourceVBMap;
    for (const auto& pair : this->Global_ResourceName_Resource_Map) {
        M_Resource m_resource = pair.second;
        //因为VB类型约定俗成需要有Stride和Type，所以这里只过滤出VB类型
        if (m_resource.Stride == L"") {
            continue;
        }
        if (m_resource.Type == L"") {
            continue;
        }

        std::wstring FilePath = m_resource.NameSpace + L"\\" + m_resource.FileName;
        m_resource.FilePath = FilePath;
        uint32_t fileSize = MMTFile_GetFileSize(FilePath);
        uint32_t stride = std::stoi(m_resource.Stride);
        uint32_t vertexNumber = fileSize / stride;

        if (vertexNumber > 0) {
            std::vector<M_Resource> tmpList;
            if (vertexNumberMResourceVBMap.contains(vertexNumber)) {
                tmpList = vertexNumberMResourceVBMap[vertexNumber];
            }
            tmpList.push_back(m_resource);
            vertexNumberMResourceVBMap[vertexNumber] = tmpList;
        }
        LOG.Info(FilePath);
        LOG.Info("fileSize: " + std::to_string(fileSize));
        LOG.Info("stride: " + std::to_string(stride));
        LOG.Info("vertexNumber: " + std::to_string(vertexNumber));
    }
    this->VertexNumberMResourceVBMap = vertexNumberMResourceVBMap;
    LOG.NewLine();
}


void ModFormat_Unity_INI::Parse_Hash_TextureOverrideIBList_Map() {
    //根据每个Hash分开TextureOverrideIB列表
    std::unordered_map<std::wstring, std::vector<M_TextureOverride>> hash_TextureOverrideIBList_Map;
    for (M_TextureOverride m_texture_override : this->Global_M_TextureOverrideList) {
        std::wstring IBHashValue = m_texture_override.IndexBufferHash;

        //查找这个TextureOverride的所有ResourceReplace，看看有没有ib在其中
        bool findIBResourceReplace = false;
        for (M_ResourceReplace resource_replace : m_texture_override.ResourceReplaceList) {
            if (resource_replace.ReplaceTarget == L"ib") {
                if (resource_replace.ReplaceTarget != L"null") {
                    findIBResourceReplace = true;
                    break;
                }
            }
        }
        //LOG.Info(L"IBHashValue: " + IBHashValue);
        if (findIBResourceReplace) {
            std::vector<M_TextureOverride> TextureOverrideIBList = hash_TextureOverrideIBList_Map[IBHashValue];
            TextureOverrideIBList.push_back(m_texture_override);
            hash_TextureOverrideIBList_Map[IBHashValue] = TextureOverrideIBList;
            LOG.Info(L"Detect [TextureOverrideIB] : " + IBHashValue + L" [match_first_index]: " + m_texture_override.MatchFirstIndex);
        }
        else {
            //如果不是IB类型的话，不为0是正常的，没必要输出出来
            //LOG.Info(L"m_texture_override.DrawIndexedList.size() is 0!");
        }
    }
    this->Hash_TextureOverrideIBList_Map = hash_TextureOverrideIBList_Map;
}


void ModFormat_Unity_INI::Parse_Hash_SingleModDetect_Map() {
    //这俩前置步骤必须得有，后面强依赖
    Parse_VertexNumberMResourceVBMap();
    Parse_Hash_TextureOverrideIBList_Map();

    //这里已经根据Hash分开了每个Hash对应的TextureOverrideIB，那么这时应该根据顶点数去Resource中匹配对应的Resource，组合成一个单独的Mod
    //这样每个Hash都是一个单独的Mod，到了外面只需要根据Hash进行分类输出即可。
    for (const auto& pair : this->Hash_TextureOverrideIBList_Map) {
        std::wstring Hash = pair.first;
        std::vector<M_TextureOverride> TextureOverrideIBList = pair.second;
        //遍历TextureOverrideIBList并查找对应的ResourceReplace的IBResource来计算顶点数
        std::vector<M_TextureOverride> matchedTextureOverrideIBList;
        int IndexBufferVertexCountTotal = 0;
        for (M_TextureOverride textureOverride : TextureOverrideIBList) {
            for (M_ResourceReplace resourceReplace : textureOverride.ResourceReplaceList) {
                if (resourceReplace.ReplaceTarget == L"ib") {
                    //在Resource列表里查找对应的Resource
                    for (M_Resource m_resource : this->Global_M_ResourceList) {
                        if (m_resource.ResourceName == resourceReplace.ReplaceResource) {
                            textureOverride.IBResourceName = m_resource.ResourceName;
                            textureOverride.IBFileName = m_resource.FileName;
                            textureOverride.IBFormat = m_resource.Format;
                            textureOverride.IBFilePath = textureOverride.NameSpace + L"\\" + textureOverride.IBFileName;

                            IndexBufferBufFile ibBufFile(textureOverride.IBFilePath, textureOverride.IBFormat);
                            IndexBufferVertexCountTotal = IndexBufferVertexCountTotal + ibBufFile.UniqueVertexCount;
                            textureOverride.IBBufFile = ibBufFile;

                            LOG.Info(m_resource.ResourceName + L"  " + resourceReplace.ReplaceResource);
                            break;
                        }
                    }
                    matchedTextureOverrideIBList.push_back(textureOverride);
                    break;
                }
            }
        }

        SingleModDetect singleModDetect;
        singleModDetect.HashValue = Hash;
        singleModDetect.TextureOverrideIBList = matchedTextureOverrideIBList;
        singleModDetect.ResourceVBList = VertexNumberMResourceVBMap[IndexBufferVertexCountTotal];
        this->Hash_SingleModDetect_Map[Hash] = singleModDetect;

        LOG.Info("IndexBufferVertexCountTotal: " + std::to_string(IndexBufferVertexCountTotal));
        LOG.Info("Matched ResourceVBList Size: " + std::to_string(singleModDetect.ResourceVBList.size()));
        LOG.NewLine();
    }

}


ModFormat_Unity_INI::ModFormat_Unity_INI(std::wstring IniFilePath) {
    //首先我们把所有的不同的3Dmigoto的ini类型都解析为单独的Section，相比于Lv3的架构更加模块化，方便后续处理
    std::vector<M_SectionLine> migotoSectionLineList = MigotoParseUtil_ParseMigotoSectionLineList(IniFilePath);
    this->MigotoSectionLineList = migotoSectionLineList;

    //列出要解析的内容,然后根据得到的migotoSectionLineList来逐个Section进行解析
    std::vector<M_Variable> global_M_VariableList;
    std::vector<M_Key> global_M_KeyList;
    std::vector<M_Resource> global_M_ResourceList;
    std::vector<M_TextureOverride> global_M_TextureOverrideList;
    for (M_SectionLine migotoSectionLine : migotoSectionLineList) {
        //然后根据名称执行具体的解析方法
        if (migotoSectionLine.SectionName == L"constants") {
            std::vector<M_Variable> m_variable_list = parseConstantsSection(migotoSectionLine);
            global_M_VariableList.insert(global_M_VariableList.end(), m_variable_list.begin(), m_variable_list.end());
        }
        else if (migotoSectionLine.SectionName.starts_with(L"key")) {
            M_Key m_key = parseKeySection(migotoSectionLine);
            global_M_KeyList.push_back(m_key);
        }
        else if (migotoSectionLine.SectionName == L"present") {
            std::vector<M_Variable> m_variable_list = parseConstantsSection(migotoSectionLine);
            global_M_VariableList.insert(global_M_VariableList.end(), m_variable_list.begin(), m_variable_list.end());
        }
        else if (migotoSectionLine.SectionName.starts_with(L"resource")) {
            M_Resource m_resource = parseResourceSection(migotoSectionLine);
            global_M_ResourceList.push_back(m_resource);
        }
        else if (migotoSectionLine.SectionName.starts_with(L"textureoverride")) {
            M_TextureOverride m_textureoverride = parseTextureOverrideSection(migotoSectionLine);
            global_M_TextureOverrideList.push_back(m_textureoverride);
        }
        else {
            LOG.Info(L"Out Size: " + std::to_wstring(migotoSectionLine.SectionLineList.size()));
            LOG.Info(L"Unrecognized Section: " + migotoSectionLine.SectionName);
        }
        LOG.NewLine();
    }
    this->Global_M_KeyList = global_M_KeyList;
    this->Global_M_VariableList = global_M_VariableList;
    this->Global_M_ResourceList = global_M_ResourceList;
    this->Global_M_TextureOverrideList = global_M_TextureOverrideList;
    LOG.Info(L"Parse all section over.");
    LOG.NewLine();
    // 把Resource的列表变成ResourceName:Resource的Map，
    // 方便后面直接通过名字获取Resource，方便很多，甚至还能少写一个方法
    std::unordered_map<std::wstring, M_Resource> global_ResourceName_Resource_Map;
    for (M_Resource resource : global_M_ResourceList) {
        global_ResourceName_Resource_Map[resource.ResourceName] = resource;
    }
    this->Global_ResourceName_Resource_Map = global_ResourceName_Resource_Map;
    LOG.NewLine();
}




ValidModDetect::ValidModDetect(D3D11GameType d3d11GameTypeInput, std::vector<M_Resource> ResourceVBListInput, std::vector<M_TextureOverride> TextureOverrideIBListInput) {
    this->d3d11GameType = d3d11GameTypeInput;
    this->TextureOverrideIBList = TextureOverrideIBListInput;
    
    //首先要确保当前的类别和识别到的Resource数量相同，因为每个类别都是一个Resource
    if (d3d11GameType.CategorySlotMap.size() != ResourceVBListInput.size()) {
        LOG.Info("Try to parse with type:" + d3d11GameType.GameType + " but can't match category size.");
        LOG.Info("d3d11GameType.CategorySlotMap.size(): " + std::to_string(d3d11GameType.CategorySlotMap.size()));
        LOG.Info("ResourceVBList.size(): " + std::to_string(ResourceVBListInput.size()));
        this->ValidMod = false;
        return;
    }

    //根据当前d3d11GameType试图根据Category的Stride拼接出ElementList
    LOG.Info("Current Try Match With Type: " + d3d11GameType.GameType);
    if (ResourceVBListInput.size() == 3) {
        LOG.Info(L"Try Detect classic Position,Texcoord,Blend mod.");
        M_Resource ResourcePosition;
        M_Resource ResourceTexcoord;
        M_Resource ResourceBlend;

        std::vector<std::string> elementList;
        std::vector<M_Resource> vbResourceOrderedList;

        std::unordered_map<std::string, int> categoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
        int positionStrie = categoryStrideMap["Position"];
        std::vector <std::string> positionElementListTmp = d3d11GameType.getCategoryElementList(d3d11GameType.OrderedFullElementList, "Position");
        LOG.NewLine();
        LOG.Info(L"Start to load Position category's element list:");
        LOG.Info(L"Position stride:" + std::to_wstring(positionStrie));

        for (M_Resource resourceVB : ResourceVBListInput) {
            int stride = std::stoi(resourceVB.Stride);
            LOG.Info(L"stride:" + std::to_wstring(stride));
            std::wstring resourceFileName = resourceVB.FileName;
            LOG.Info(L"resourceFileName:" + resourceFileName);

            //POSITION
            if (stride == positionStrie) {
                ResourcePosition = resourceVB;
                elementList.insert(elementList.end(), positionElementListTmp.begin(), positionElementListTmp.end());
                vbResourceOrderedList.push_back(ResourcePosition);
                break;

            }
        }


        int colorByteWidth = d3d11GameType.ElementNameD3D11ElementMap["COLOR"].ByteWidth;
        int texcoordByteWidth = d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD"].ByteWidth;
        int texcoord1ByteWidth = d3d11GameType.ElementNameD3D11ElementMap["TEXCOORD1"].ByteWidth;
        LOG.NewLine();
        LOG.Info(L"Start to load TEXCOORD category's element list:");
        for (M_Resource resourceVB : ResourceVBListInput) {
            int stride = std::stoi(resourceVB.Stride);
            std::wstring resourceFileName = resourceVB.FileName;
            //TEXCOORD
            if (stride == texcoordByteWidth) {
                ResourceTexcoord = resourceVB;
                elementList.push_back("TEXCOORD");
                vbResourceOrderedList.push_back(ResourceTexcoord);

                break;

            }
            if (stride == colorByteWidth + texcoordByteWidth) {
                ResourceTexcoord = resourceVB;
                elementList.push_back("COLOR");
                elementList.push_back("TEXCOORD");
                vbResourceOrderedList.push_back(ResourceTexcoord);

                break;

            }
            if (stride == colorByteWidth + texcoordByteWidth + texcoord1ByteWidth) {
                ResourceTexcoord = resourceVB;
                elementList.push_back("COLOR");
                elementList.push_back("TEXCOORD");
                elementList.push_back("TEXCOORD1");
                vbResourceOrderedList.push_back(ResourceTexcoord);

                break;
            }
        }
        LOG.NewLine();
        LOG.Info(L"Start to load Blend category's element list:");
        for (M_Resource resourceVB : ResourceVBListInput) {
            LOG.Info(L"Current processing ResourceVB: " + resourceVB.FileName);

            int stride = std::stoi(resourceVB.Stride);
            std::wstring resourceFileName = resourceVB.FileName;

            std::vector<std::string> blendElementList = d3d11GameType.getCategoryElementList(d3d11GameType.OrderedFullElementList,"Blend");
            int blendStride = d3d11GameType.getElementListStride(blendElementList);
            LOG.Info(L"Blend stride:" + std::to_wstring(blendStride));
            LOG.Info(L"Current stride:" + std::to_wstring(stride));

            //BLEND
            if (stride == blendStride) {
                ResourceBlend = resourceVB;
                if (boost::algorithm::any_of_equal(blendElementList, "BLENDWEIGHT")) {
                    elementList.push_back("BLENDWEIGHT");
                }
                if (boost::algorithm::any_of_equal(blendElementList, "BLENDWEIGHTS")) {
                    elementList.push_back("BLENDWEIGHTS");
                }
                elementList.push_back("BLENDINDICES");

                vbResourceOrderedList.push_back(ResourceBlend);
                break;
            }
        }
        LOG.NewLine();

        /*LOG.LogOutput(L"current Element list detected");
        for (std::wstring elementName : elementList) {
            LOG.LogOutput(elementName);
        }*/

        this->ElementList = elementList;
        this->OrderedResourceVBList = vbResourceOrderedList;

    }
    else if (ResourceVBListInput.size() == 1) {
        int bufStride = 0;
        std::vector<std::string> elementList;
        LOG.Info(L"Detect only one ResourceVB, maybe this is a object mod.");
        M_Resource resourceVBObject = ResourceVBListInput[0];

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
        if (std::stoi(resourceVBObject.Stride) == bufStride) {
            std::vector<M_Resource> vbResourceOrderedList;
            vbResourceOrderedList.push_back(resourceVBObject);
            this->ElementList = elementList;
            this->OrderedResourceVBList = vbResourceOrderedList;
        }


        //下面是匹配去除了TEXCOORD1的版本
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
        if (std::stoi(resourceVBObject.Stride) == bufStride) {
            std::vector<M_Resource> vbResourceOrderedList;
            vbResourceOrderedList.push_back(resourceVBObject);
            this->ElementList = elementList;
            this->OrderedResourceVBList = vbResourceOrderedList;
        }
    }
    else if (ResourceVBListInput.size() == 4) {
        LOG.Error("Wrong possible ResourceVB size, is your mod contains a extra Remapped Blend part? please remove it and try again.");
    }

    //确保我们识别出来的排序后的Resource列表大小和原本的相等，才能算是识别到了一个有效的Mod
    if (this->OrderedResourceVBList.size() != ResourceVBListInput.size()) {
        this->ValidMod = false;
    }
    else {
        this->ValidMod = true;
    }

    //如果识别的Category列表数量正确，则继续读取VB数据
    if (this->ValidMod) {
        //重新排序ElementList防止顺序不对
        this->ElementList = this->d3d11GameType.getReorderedElementList(this->ElementList);
        //接下来根据ElementList和当前的d3d11GameType就可以去读取并组装FinalVB0数据了。
        LOG.Info(L"read vb file ,combine final file");
        std::unordered_map<int, std::vector<byte>> vb0IndexNumberBytes;
        for (M_Resource resourceVBObject : this->OrderedResourceVBList) {

            LOG.Info(L"current  : " + resourceVBObject.ResourceName);
            LOG.Info(L"current processing vb file: " + resourceVBObject.FileName);
            resourceVBObject.FileName = resourceVBObject.NameSpace + L"\\" + resourceVBObject.FileName;
            LOG.Info(L"Show the resourceVBObject before read and output to vb file.");
            if (resourceVBObject.FileName == L"" || resourceVBObject.FileName.empty()) {
                LOG.Info(L"Can't find any FileNam in this ResourceVB.");
                continue;
            }
            std::wstring resourceVBFileName = resourceVBObject.FileName;
            int resourceVBStride = std::stoi(resourceVBObject.Stride);
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
        this->VB0IndexNumberBytes = vb0IndexNumberBytes;

        //组装为FinalVB0Bytes
        std::vector<byte> outputBytes;
        for (const auto& pair : this->VB0IndexNumberBytes) {
            std::vector<byte> indexBytes = pair.second;
            outputBytes.insert(outputBytes.end(), indexBytes.begin(), indexBytes.end());
        }
        this->finalVB0Bytes = outputBytes;

        //LOG.Info("outputBytes.size():  " + std::to_string(outputBytes.size()));
    }
   

}