#include "VertexBufferTxtFile.h"
#include "GlobalConfigs.h"
#include <sstream>


VertexDataLine::VertexDataLine() {

}


VertexDataLine::VertexDataLine(const std::wstring inputString) {
    // vb0[0]+000 POSITION: 5.04093075, 4.65782213, 1.05010247
    // vb0[115]+000 POSITION: -0.297964305, 0.0108079528, -0.00018915176
    // vb0[115] + 012 NORMAL: 0.51988852, 0.853630543, -0.0321041532
    // vb0[115] + 024 TANGENT : 0, 0.999893308, 0.0146092195, 1
    size_t slotEndPos = inputString.find(L"[");
    Slot = inputString.substr(0, slotEndPos);

    size_t indexStartPos = inputString.find(L"[") + 1;
    size_t indexEndPos = inputString.find(L"]");
    Index = inputString.substr(indexStartPos, indexEndPos - indexStartPos);

    size_t offsetStartPos = inputString.find(L"+");
    Offset = inputString.substr(offsetStartPos, 4);

    size_t elementNameStartPos = offsetStartPos + 5;
    size_t elementNameEndPos = inputString.find(L":");
    ElementName = inputString.substr(elementNameStartPos, elementNameEndPos - elementNameStartPos);

    size_t elementValueStartPos = inputString.find(L":") + 2;
    ElementValue = inputString.substr(elementValueStartPos);

    OriginalStrLine = inputString;
}


//定义将此对象写出到
void VertexBufferTxtFile::outputVB0File(std::wstring outputFileName) {

    //首先，每个VertexDataLine都要根据outputElementList，分别各自重新计算它们的值，才能写出
    //或者直接计算完就写出？
    std::ofstream file(outputFileName);

    if (file.is_open()) {
        file << "stride: " << MMTString_ToByteString(Stride) << std::endl;
        file << "first vertex: 0" << std::endl;
        file << "vertex count: " << MMTString_ToByteString(VertexCount) << std::endl;
        file << "topology: " << MMTString_ToByteString(Topology) << std::endl;
        //接下来开始输出element列表
        int elementListIndex = 0;
        int AlignedByteOffset = 0;
        for (std::string elementName : realElementList) {
            file << "element[" << elementListIndex << "]:" << std::endl;
            D3D11Element elementObject = this->d3d11GameType.ElementNameD3D11ElementMap[elementName];

            file << "  SemanticName: " << elementObject.SemanticName << std::endl;
            file << "  SemanticIndex: " << elementObject.SemanticIndex << std::endl;
            file << "  Format: " + elementObject.Format << std::endl;
            file << "  InputSlot: " << elementObject.InputSlot << std::endl;
            file << "  AlignedByteOffset: " << AlignedByteOffset << std::endl;
            file << "  InputSlotClass: " << elementObject.InputSlotClass << std::endl;
            file << "  InstanceDataStepRate: " << elementObject.InstanceDataStepRate << std::endl;

            //赋值完之后，值要加上此Element的ByteWidth
            AlignedByteOffset = AlignedByteOffset + elementObject.ByteWidth;
            //索引偏移要+1
            elementListIndex = elementListIndex + 1;
        }

        file << std::endl;
        file << "vertex-data:" << std::endl;
        file << std::endl;

        //开始输出VertexData部分

        
        for (const auto& pair : IndexVertexDataLineListMap) {
            int indexNumber = pair.first;

            //这里是为了兼容Force PointlistIndex技术，确保顶点数以Trianglelist为准
            if (indexNumber >= std::stoi(this->VertexCount)) {
                LOG.Info(L"IndexNumber is greater than vertex count ,force exit!");
                break;
            }
            const std::vector<VertexDataLine>& vertexDataLineList = pair.second;

            // 遍历vertexDataLineList,先根据outputElementList中的元素顺序进行排序
            // 先把每个VertexDataLine拿出来放到map里
            std::map<std::wstring, VertexDataLine> elementNameVertexDataLineMap;
            for (const VertexDataLine& vertexDataLine : vertexDataLineList) {
                std::wstring elementName;
                if (boost::algorithm::starts_with(vertexDataLine.ElementName,"ATTRIBUTE")) {
                    //TODO
                    //elementName = d3d11ElementAttribute.ExtractSemanticNameD3D11ElementMap[vertexDataLine.ElementName].SemanticName;
                }
                else {
                    elementName = vertexDataLine.ElementName;
                }
                
                elementNameVertexDataLineMap[elementName] = vertexDataLine;
            }

            //遍历outputElementList
            int AlignedByteOffset = 0;
            for (std::string elementName : realElementList) {
                D3D11Element elementObject = this->d3d11GameType.ElementNameD3D11ElementMap[elementName];
                //拿到对应的VertexDataLine，直接输出
                VertexDataLine vertexDataLine = elementNameVertexDataLineMap[MMTString_ToWideString(elementName)];
                file << "vb0[" << MMTString_ToByteString(vertexDataLine.Index) << "]";
                //alignedOffset设置为三位数
                std::wstringstream offsetStrStream{};
                offsetStrStream << std::setfill(L'0') << std::setw(3) << AlignedByteOffset;
                file << "+" << MMTString_ToByteString(offsetStrStream.str()) << " " << elementName << ": " << MMTString_ToByteString(vertexDataLine.ElementValue) << std::endl;
                AlignedByteOffset = AlignedByteOffset + elementObject.ByteWidth;
            }
            file << std::endl;

        }

        // 文件流对象超出作用域时，文件会自动关闭
    }
    else {
        std::cout << "Unable to open the file." << std::endl;
    }

}


VertexBufferTxtFile::VertexBufferTxtFile() {};


VertexBufferTxtFile::VertexBufferTxtFile(const std::wstring& filename,D3D11GameType d3d11GameType,bool parseElement,bool parseData) {
    std::filesystem::path filePath(filename);
    this->FileName = filePath.filename().wstring();
    this->Index = this->FileName.substr(0, 6);
    this->Hash = this->FileName.substr(11,8);
    this->d3d11GameType = d3d11GameType;
    
    this->realElementList = parseRealElementList(filename,parseElement);
    
    if (parseData) {
        parseAllLineFromFile(filename);
    }
    LOG.Info(L"VertexBufferFileData parse over.");
}


//这个是用来读取基础属性用的，且不使用d3d11ElementAttribute来解析数据，甚至不解析任何数据
VertexBufferTxtFile::VertexBufferTxtFile(const std::wstring& filename) {
    std::filesystem::path filePath(filename);
    this->FileName = filePath.filename().wstring();
    this->Index = this->FileName.substr(0, 6);
    this->Hash = this->FileName.substr(11, 8);
    this->realElementList = parseRealElementList(filename,false);
}


std::vector<D3D11Element> VertexBufferTxtFile::parseShowElementList(const std::wstring& filename) {

    //根据传入的VBFilePath，获取当前是vb几
    std::wstring vbSlotNumber = MMTString_GetFileNameFromFilePath(filename).substr(9, 1);

    //解析每个Trianglelist的VB文件
    std::wifstream file(filename);
    if (!file.is_open()) {
        LOG.Error(L"Can't open: " + filename);
    }
    else {
        //LOG.Info(L"Parsing Show ElementList For: " + filename);
    }

    //解析用到的变量
    std::wstring line;
    std::wstring lineBefore = L"";

    int fileShowStride = 0;

    bool inElementLine = false;
    bool inVertexData = false;

    std::vector<D3D11Element> d3d11ElementList;
    D3D11Element d3d11Element;
    int elementNumber = 0;

    std::vector<VertexDataLine> firstVertexDataList;

    while (std::getline(file, line)) {
        boost::algorithm::to_lower(line);
        boost::algorithm::trim(line);

        if (boost::algorithm::starts_with(line, "element")) {
            inElementLine = true;
            d3d11Element.ElementNumber = elementNumber;
            elementNumber++;
            continue;
        }

        // 2.然后解析其中的D3d11Element的列表
        if (inElementLine && !inVertexData) {
            std::size_t colonPos = line.find(':');
            if (colonPos != std::wstring::npos) {
                std::wstring key = line.substr(0, colonPos);
                boost::algorithm::trim(key);

                std::wstring value = line.substr(colonPos + 1);
                boost::algorithm::trim(value);
                //LOG.LogOutput(L"Current key: " + key + L" value: " + value);
                if (boost::algorithm::ends_with(key, "semanticname")) {
                    d3d11Element.SemanticName = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "semanticindex")) {
                    d3d11Element.SemanticIndex = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "format")) {
                    d3d11Element.Format = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "inputslot")) {
                    d3d11Element.InputSlot = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "alignedbyteoffset")) {
                    d3d11Element.AlignedByteOffset = std::stoi(value);
                }
                else if (boost::algorithm::ends_with(key, "inputslotclass")) {
                    d3d11Element.InputSlotClass = MMTString_ToByteString(value);
                }
                else if (boost::algorithm::ends_with(key, "instancedatasteprate")) {
                    d3d11Element.InstanceDataStepRate = MMTString_ToByteString(value);
                    d3d11Element.ExtractSlot = "vb" + MMTString_ToByteString(vbSlotNumber);
                    //理论上这个应该是最后一个
                    //LOG.LogOutput(L"Read vbSlotNumber: " + d3d11Element.InputSlot);
                    if (d3d11Element.InputSlot == MMTString_ToByteString(vbSlotNumber)) {
                        //LOG.LogOutput(L"Add into d3d11ElementList: " + d3d11Element.SemanticName);
                        //这里还要设置ExtractVB和ExtractTechnique
                        d3d11ElementList.push_back(d3d11Element);
                    }
                }
            }
        }

        // 3.解析VertexDataLine部分
        //遇到vertex-data要退出的啊，不然卡死了
        //至少也要解析一个VertexData,不然的话第一个如果不在vertexdata里出现，那就错误识别了。
        if (boost::algorithm::starts_with(line, "vertex-data:")) {
            break;
        }
        lineBefore = line;
    }
    file.close();
    //LOG.NewLine();
    return d3d11ElementList;
}


std::vector<std::string> VertexBufferTxtFile::parseRealElementList(const std::wstring& filename,bool parseElement) {

    std::vector<std::string> tmpElementList;

    std::wstring line;
    std::wstring lineBefore = L"";
    bool startVertexData = false;
    std::vector<VertexDataLine> vertexDataLineList;

    std::wifstream file(filename);

    while (std::getline(file, line)) {
        if (!startVertexData) {
            std::wstring strideStr = L"stride: ";
            std::wstring vertexCountStr = L"vertex count: ";
            std::wstring topologyStr = L"topology: ";
            std::wstring vertexDataFlag = L"vertex-data:";

            if (boost::algorithm::starts_with(line, strideStr)) {
                std::wstring valueStr = line.substr(strideStr.length());
                boost::algorithm::trim(valueStr);
                this->Stride = valueStr;
            }
            else if (boost::algorithm::starts_with(line, vertexCountStr)) {
                std::wstring valueStr = line.substr(vertexCountStr.length());
                boost::algorithm::trim(valueStr);
                this->VertexCount = valueStr;
            }
            else if (boost::algorithm::starts_with(line, topologyStr)) {
                std::wstring valueStr = line.substr(topologyStr.length());
                boost::algorithm::trim(valueStr);
                this->Topology = valueStr;
            }
            else if (boost::algorithm::starts_with(line, vertexDataFlag)) {
                startVertexData = true;
                if (!parseElement) {
                    break;
                }
            }

        }
        else {
            if (line.empty() && !lineBefore.empty()) {

                if (!vertexDataLineList.empty()) {

                    std::unordered_map <std::wstring, int> elementValueNumberMap;
                    for (size_t i = 0; i < vertexDataLineList.size(); i++)
                    {
                        VertexDataLine vertexDataLine = vertexDataLineList[i];
                        int number = elementValueNumberMap[vertexDataLine.ElementValue];
                        if (elementValueNumberMap.count(vertexDataLine.ElementValue) == 0) {
                            elementValueNumberMap[vertexDataLine.ElementValue] = 1;
                        }
                        else {
                            elementValueNumberMap[vertexDataLine.ElementValue] = number + 1;
                        }
                    }

                    std::wstring texcoordOffset = L"";
                    std::wstring texcoord1Offset = L"";
                    for (size_t i = 0; i < vertexDataLineList.size(); i++)
                    {
                        VertexDataLine vertexDataLine = vertexDataLineList[i];
                        int number = elementValueNumberMap[vertexDataLine.ElementValue];
                        if (number == 1) {
                            tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            //LOG.LogOutput("添加了" + vertexDataLine.ElementName);
                        }
                        else if (number > 1 && vertexDataLine.Offset == L"+000" ) {
                            //这里是对永劫一种特殊情况的处理：
                            //vb2[0]+000 BLENDWEIGHTS: 1, 0, 0, 0
                            //vb2[0] + 016 BLENDINDICES: 1, 0, 0, 0
                            //有时候BLENDWEIGHTS和BLENDINDICES相同，会导致无法读取到BLENDWEIGHTS
                            if ( vertexDataLine.ElementName == L"BLENDWEIGHTS") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));

                            }

                            if (vertexDataLine.ElementName == L"BLENDWEIGHT") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));

                            }
                            // 这个是崩坏三2.0中出现的特殊情况处理，只有在Trianglelist中时才能通过：
                            // 这里TEXCOORD1是无效的，TEXCOORD需要添加。
                            //vb1[0]+000 TEXCOORD: -0.487339795, 0.0454249829
                            //vb1[0]+000 TEXCOORD1: -0.487339795, 0.0454249829
                            if (vertexDataLine.ElementName == L"TEXCOORD" && Topology == L"trianglelist") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }


                            //只处理UE情况
                          /*  if (boost::algorithm::starts_with(vertexDataLine.ElementName, L"ATTRIBUTE")) {
                                std::wstring checkElementNameTexcoord = d3d11ElementAttribute.ElementNameD3D11ElementMap[L"TEXCOORD"].ExtractSemanticName;
                                std::wstring checkElementNameTangent = d3d11ElementAttribute.ElementNameD3D11ElementMap[L"TANGENT"].ExtractSemanticName;
                                if (vertexDataLine.ElementName == checkElementNameTexcoord) {
                                    tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                                }
                                else if (vertexDataLine.ElementName == checkElementNameTangent) {
                                    tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                                }
                            }*/
                            /*
                                这里是对UE4的特殊支持，因为UE4有如下特殊类型
                                vb2[0]+000 ATTRIBUTE6: 0.851074219, 0.0283813477
                                vb2[0]+000 ATTRIBUTE5: 0.851074219, 0.0283813477
                                vb2[0]+000 ATTRIBUTE7: 0.851074219, 0.0283813477
                                vb2[0]+000 ATTRIBUTE8: 0.851074219, 0.0283813477

                                以及TANGENT的值可能和BITANGENT值相同
                                vb5[0]+000 ATTRIBUTE9: 0, 0, 0
                                vb5[0]+012 ATTRIBUTE10: 0, 0, 0
                            */


                        }
                        else if (number > 1 && vertexDataLine.Offset != L"+000") {
                           

                            if (vertexDataLine.ElementName == L"TEXCOORD") {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                                texcoordOffset = vertexDataLine.Offset;
                            }
                            else if (vertexDataLine.ElementName == L"TEXCOORD1" && vertexDataLine.Offset != texcoordOffset ) {
                                texcoord1Offset = vertexDataLine.Offset;
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                            //添加绝区零专属的TEXCOORD2和TEXCOORD3支持
                            else if (vertexDataLine.ElementName == L"TEXCOORD2" && vertexDataLine.Offset != texcoordOffset && vertexDataLine.Offset != texcoord1Offset) {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                            else if (vertexDataLine.ElementName == L"TEXCOORD3" && vertexDataLine.Offset != texcoordOffset && vertexDataLine.Offset != texcoord1Offset) {
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                            else if (!boost::algorithm::starts_with(vertexDataLine.ElementName, L"TEXCOORD")) {
                                //LOG.LogOutput("添加了" + vertexDataLine.ElementName);
                                tmpElementList.push_back(MMTString_ToByteString(vertexDataLine.ElementName));
                            }
                        }


                    }

                    break;
                }
                vertexDataLineList.clear();
            }
            else {
                std::string vb0Str = "vb";
                if (boost::algorithm::starts_with(line, vb0Str)) {
                    VertexDataLine dataLine(line);
                    vertexDataLineList.push_back(dataLine);
                }
            }
            lineBefore = line;
        }
        
    }
    file.close();

    // 原神4.4更新将Trianglelist中的VertexCount去掉了
    // 所以读取完之后如果没有的话，我们需要遍历并读取每一行的Index例如vb0[123]里的123来设置VertexCount
    // 因为是遍历所有行数，所以总是能设置正确的VertexCount
    if (this->VertexCount == L"") {
        LOG.Info(L"Can't find vertex count attribute in VB file, count the whole line manually.");
        std::wifstream anotherFile(filename);
        //跳到文件末尾
        anotherFile.seekg(0, std::ios::end);
        //获取文件大小
        std::streampos fileSize = anotherFile.tellg();
        //跳转到结尾-1000的位置
        anotherFile.seekg(-1000, std::ios::end);

        std::wstring line2;
        while (std::getline(anotherFile, line2)) {
            if (boost::algorithm::starts_with(line2, "vb")) {
                VertexDataLine dataLine(line2);
                this->VertexCount = dataLine.Index;
            }
        }
        anotherFile.close();

        //Index是从0开始的，所以我们需要 + 1
        this->VertexCount = std::to_wstring(std::stoi(this->VertexCount) + 1);
        LOG.Info(L"VertexCount: " + this->VertexCount);

    }
    
    return tmpElementList;
}


void VertexBufferTxtFile::parseAllLineFromFile(const std::wstring& filename) {
    this->FileName = filename;
    //首先打开文件，并且将文件读取到一个Vector里备用
    std::wifstream file(filename);
    std::vector<std::wstring> lines;
    std::wstring tmpline;
    while (std::getline(file, tmpline)) {
        lines.push_back(tmpline);
    }
    file.close();
    // add a black line so easier to process
    lines.push_back(L"");
    LOG.Info(L"Read file name:" + filename);
    LOG.Info(L"Read all line number: " + std::to_wstring(lines.size()));

    bool startVertexData = false;
    std::wstring lineBefore = L"";
    std::vector<VertexDataLine> vertexDataLineList;

    // process line by line
    for (std::wstring line : lines) {
        if (!startVertexData) {
            if (boost::algorithm::starts_with(line, L"vertex-data:")) {
                startVertexData = true;
            }
        }
        else {
            //这里判断如果上一行有东西，且这一行读取到了空行，就会添加
            //但是如果文件结尾没有空行，就会漏掉一行
            if (line.empty() && !lineBefore.empty()) {
                //如果列表不为空，就加入本类的列表，加入后清空
                if (!vertexDataLineList.empty()) {
                    int vertexIndex = std::stoi(vertexDataLineList[0].Index);
                    /*if (vertexIndex > 114) {
                        LOG.LogOutput(vertexDataLineList[0].ElementName);
                    }*/
                    IndexVertexDataLineListMap[vertexIndex] = vertexDataLineList;

                }
                vertexDataLineList.clear();
            }
            else {
                std::wstring vb0Str = L"vb";
                if (boost::algorithm::starts_with(line, vb0Str)) {
                    VertexDataLine dataLine(line);
                    if (boost::algorithm::any_of_equal(this->realElementList,MMTString_ToByteString(dataLine.ElementName))) {

                        vertexDataLineList.push_back(dataLine);
                    }
                }
            }
            lineBefore = line;
        }
    }

}

