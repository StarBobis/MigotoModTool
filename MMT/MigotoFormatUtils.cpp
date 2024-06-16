#include "MigotoFormatUtils.h"
#include "MMTFormatUtils.h"


std::unordered_map<std::wstring, std::vector<std::byte>> RecalculateColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideMap) {

    
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];

    //获取Position分类的内容
    std::vector<std::byte> PositionCategoryValues = finalVBCategoryDataMap[L"Position"];

    //获取POSITION和TANGENT
    std::vector<std::vector<double>> POSITION_VALUES;
    std::vector<std::vector<double>> NORMAL_VALUES;

    int positionStride = 0;
    //因为POSITION不会出现像TEXCOORD那样的动态步长，所以这里直接获取OrderedElementList，统计出Position的长度
    for (std::string elementName : d3d11GameType.OrderedFullElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];
        if (d3d11Element.Category == "Position") {
            int byteWidth = d3d11Element.ByteWidth;
            positionStride += byteWidth;

        }
    }
    LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
    {
        std::vector<std::byte> POSITION_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i, i + 4);
        //LOG.LogOutput(L"POSITION_X: " + std::to_wstring(bytesToFloat(POSITION_X)));
        std::vector<std::byte> POSITION_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 4, i + 8);
        std::vector<std::byte> POSITION_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 8, i + 12);
        std::vector<double> POSITION_DOUBLE = { MMTFormat_ByteVectorToFloat(POSITION_X), MMTFormat_ByteVectorToFloat(POSITION_Y) ,MMTFormat_ByteVectorToFloat(POSITION_Z) };
        POSITION_VALUES.push_back(POSITION_DOUBLE);

        std::vector<std::byte> NORMAL_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 12, i + 16);
        std::vector<std::byte> NORMAL_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 16, i + 20);
        std::vector<std::byte> NORMAL_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 20, i + 24);
        std::vector<double> NORMAL_DOUBLE = { MMTFormat_ByteVectorToFloat(NORMAL_X), MMTFormat_ByteVectorToFloat(NORMAL_Y) ,MMTFormat_ByteVectorToFloat(NORMAL_Z) };
        NORMAL_VALUES.push_back(NORMAL_DOUBLE);
    }
    LOG.Info(L"Start to CalculateAverageNormalsAndStoreTangent");
    //把顶点相同的法线 3维向量 相加然后归一化，作为所有值为这个顶点值的相同向量
    //然后对每个对应的TANGENT，都用这个值作为前3位，0作为最后一位

    //计算出平局法线的TANGENT
    std::vector<std::vector<double>> TANGENT_VALUES = MMTFormat_NormalNormalizeStoreTangent(POSITION_VALUES, NORMAL_VALUES);
    LOG.Info(L"TANGENT_VALUES size: " + std::to_wstring(TANGENT_VALUES.size()));
    LOG.Info(L"CalculateAverageNormalsAndStoreTangent success");


    //准备一个新的用于返回
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;

    //首先要确定Color在哪个槽位里
    D3D11Element colorElement = d3d11GameType.ElementNameD3D11ElementMap["COLOR"];
    std::wstring colorCategory = MMTString_ToWideString(colorElement.Category);
    std::vector<std::byte> colorCategoryValues = finalVBCategoryDataMap[colorCategory];

    //确定Color所在槽位的步长
    int colorCategoryStride = categoryStrideMap[MMTString_ToByteString(colorCategory)];

    //确定Color所在的槽位偏移
    int colorOffset = 0;

    //TODO 确定Color的ByteWidth,这里我们固定为4，因为只有这一种情况，其实也用不到，不过留作后续兼容使用
    //int colorByteWidth = 4;

    for (std::string elementName : basicConfig.TmpElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

        //首先必须属于color所在的槽位
        if (d3d11Element.Category == MMTString_ToByteString(colorCategory)) {
            if (elementName != "COLOR") {
                colorOffset += d3d11Element.ByteWidth;
            }
            else {
                break;
            }
        }
    }

    //开始更改
    std::vector<std::byte> newTexcoordCategoryValues;
    int count = 0;
    for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
    {
        //先获取COLOR前的所有值
        std::vector<std::byte> beforeValues = MMTFormat_GetRange_Byte(colorCategoryValues, i, i + colorOffset);
        std::vector<std::byte> ColorValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
        std::vector<std::byte> afterValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorCategoryStride);
        //新准备的COLOR值
        std::vector<std::byte> newColorValues;
        std::vector<double> TANGENT_DOUBLES = TANGENT_VALUES[count];

        //R,G,B分别存储归一化之后Normal的分量
        newColorValues.push_back(MMTFormat_PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[0] * 255))));
        newColorValues.push_back(MMTFormat_PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[1] * 255))));
        newColorValues.push_back(MMTFormat_PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[2] * 255))));

        //Alpha通道固定为128
        newColorValues.push_back(ColorValues[3]);

        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());

        count++;
    }

    newVBCategoryDataMap = finalVBCategoryDataMap;
    newVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
    return newVBCategoryDataMap;

}





//兼容COLOR在Position分类中的情况，一种算法同时兼容Position和Texcoord
std::unordered_map<std::wstring, std::vector<std::byte>> ResetColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideMap) {
    //读取各个RGB分量的值
    std::wstring rgb_r = MMTString_ToWideString(basicConfig.ColorMap["rgb_r"]);
    std::wstring rgb_g = MMTString_ToWideString(basicConfig.ColorMap["rgb_g"]);
    std::wstring rgb_b = MMTString_ToWideString(basicConfig.ColorMap["rgb_b"]);
    std::wstring rgb_a = MMTString_ToWideString(basicConfig.ColorMap["rgb_a"]);

    //准备好数字用于替换
    int rgb_r_num = -1;
    int rgb_g_num = -1;
    int rgb_b_num = -1;
    int rgb_a_num = -1;

    //尝试进行格式转换
    if (L"default" != rgb_r) {
        rgb_r_num = std::stoi(rgb_r);
    }
    if (L"default" != rgb_g) {
        rgb_g_num = std::stoi(rgb_g);
    }
    if (L"default" != rgb_b) {
        rgb_b_num = std::stoi(rgb_b);
    }
    if (L"default" != rgb_a) {
        rgb_a_num = std::stoi(rgb_a);
    }

    //如果都是default那干脆啥也别干直接返回
    if (rgb_a_num == -1 && rgb_g_num == -1 && rgb_b_num == -1 && rgb_a_num == -1) {
        return finalVBCategoryDataMap;
    }

    //准备一个新的用于返回
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;

    //首先要确定Color在哪个槽位里
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
    D3D11Element colorElement = d3d11GameType.ElementNameD3D11ElementMap["COLOR"];
    std::wstring colorCategory = MMTString_ToWideString(colorElement.Category);
    std::vector<std::byte> colorCategoryValues = finalVBCategoryDataMap[colorCategory];

    //准备好数字用于写入
    std::byte patchRgb_R = MMTFormat_PackNumberToByte(rgb_r_num);
    std::byte patchRgb_G = MMTFormat_PackNumberToByte(rgb_g_num);
    std::byte patchRgb_B = MMTFormat_PackNumberToByte(rgb_b_num);
    std::byte patchRgb_A = MMTFormat_PackNumberToByte(rgb_a_num);

    //确定Color所在槽位的步长
    int colorCategoryStride = categoryStrideMap[MMTString_ToByteString(colorCategory)];

    //确定Color所在的槽位偏移
    int colorOffset = 0;

    //TODO 确定Color的ByteWidth,这里我们固定为4，因为只有这一种情况，其实也用不到，不过留作后续兼容使用
    //int colorByteWidth = 4;

    for (std::string elementName : basicConfig.TmpElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

        //首先必须属于color所在的槽位
        if (d3d11Element.Category == MMTString_ToByteString(colorCategory)) {
            if (elementName != "COLOR") {
                colorOffset += d3d11Element.ByteWidth;
            }
            else {
                break;
            }
        }
    }

    //开始更改
    std::vector<std::byte> newTexcoordCategoryValues;
    for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
    {
        //先获取COLOR前的所有值
        std::vector<std::byte> beforeValues = MMTFormat_GetRange_Byte(colorCategoryValues, i, i + colorOffset);
        std::vector<std::byte> ColorValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
        std::vector<std::byte> afterValues = MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorCategoryStride);
        //新准备的COLOR值
        std::vector<std::byte> newColorValues;

        if (rgb_r_num != -1) {
            newColorValues.push_back(patchRgb_R);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 1)[0]);
        }

        if (rgb_g_num != -1) {
            newColorValues.push_back(patchRgb_G);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 1, i + colorOffset + 2)[0]);
        }

        if (rgb_b_num != -1) {
            newColorValues.push_back(patchRgb_B);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 2, i + colorOffset + 3)[0]);
        }

        if (rgb_a_num != -1) {
            newColorValues.push_back(patchRgb_A);
        }
        else {
            newColorValues.push_back(MMTFormat_GetRange_Byte(colorCategoryValues, i + colorOffset + 3, i + colorOffset + 4)[0]);
        }

        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
        newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());
    }

    newVBCategoryDataMap = finalVBCategoryDataMap;
    newVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
    return newVBCategoryDataMap;
};




//TODO 这里的TANGENT计算步长不能写死，必须灵活变化
std::unordered_map<std::wstring, std::vector<std::byte>> TANGENT_averageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> inputVBCategoryDataMap, D3D11GameType d3d11GameType) {
    LOG.Info(L"Start to recalculate tangent use averageNormal algorithm:");
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
    //获取Position分类的内容
    std::vector<std::byte> PositionCategoryValues = inputVBCategoryDataMap[L"Position"];

    //获取POSITION和TANGENT
    std::vector<std::vector<double>> POSITION_VALUES;
    std::vector<std::vector<double>> NORMAL_VALUES;

    int positionStride = 0;
    //因为POSITION不会出现像TEXCOORD那样的动态步长，所以这里直接获取OrderedElementList，统计出Position的长度
    for (std::string elementName : d3d11GameType.OrderedFullElementList) {
        D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];
        if (d3d11Element.Category == "Position") {
            int byteWidth = d3d11Element.ByteWidth;
            positionStride += byteWidth;

        }
    }
    LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
    {
        std::vector<std::byte> POSITION_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i, i + 4);
        //LOG.LogOutput(L"POSITION_X: " + std::to_wstring(bytesToFloat(POSITION_X)));
        std::vector<std::byte> POSITION_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 4, i + 8);
        std::vector<std::byte> POSITION_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 8, i + 12);
        std::vector<double> POSITION_DOUBLE = { MMTFormat_ByteVectorToFloat(POSITION_X), MMTFormat_ByteVectorToFloat(POSITION_Y) ,MMTFormat_ByteVectorToFloat(POSITION_Z) };
        POSITION_VALUES.push_back(POSITION_DOUBLE);

        std::vector<std::byte> NORMAL_X = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 12, i + 16);
        std::vector<std::byte> NORMAL_Y = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 16, i + 20);
        std::vector<std::byte> NORMAL_Z = MMTFormat_GetRange_Byte(PositionCategoryValues, i + 20, i + 24);
        std::vector<double> NORMAL_DOUBLE = { MMTFormat_ByteVectorToFloat(NORMAL_X), MMTFormat_ByteVectorToFloat(NORMAL_Y) ,MMTFormat_ByteVectorToFloat(NORMAL_Z) };
        NORMAL_VALUES.push_back(NORMAL_DOUBLE);
    }
    LOG.Info(L"Start to CalculateAverageNormalsAndStoreTangent");
    //把顶点相同的法线 3维向量 相加然后归一化，作为所有值为这个顶点值的相同向量
    //然后对每个对应的TANGENT，都用这个值作为前3位，0作为最后一位

    //计算出平局法线的TANGENT
    std::vector<std::vector<double>> TANGENT_VALUES = MMTFormat_CalculateAverageNormalsAndStoreTangent(POSITION_VALUES, NORMAL_VALUES);
    LOG.Info(L"TANGENT_VALUES size: " + std::to_wstring(TANGENT_VALUES.size()));
    LOG.Info(L"CalculateAverageNormalsAndStoreTangent success");

    //创建一个新的vector用来装修改后的Position分类的内容
    std::vector<std::byte> newPositionCategoryValues;
    //添加TANGENT到新的newPositionCategoryValues
    int count = 0;
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
    {
        //TODO 这里的positionStride - 16得到的就是除了TANGENT之外的其他的element的总长度
        //因为在HI3Body2中，Position是由POSITION，NORMAL，COLOR，TANGENT组成的，其它的类型则是POSITION，NORMAL，TANGENT
        //所以虽然这样暂时够用了，但是兼容性并不好，以后再来修改吧。
        std::vector<std::byte> POSITION_NORMAL_VALUES = MMTFormat_GetRange_Byte(PositionCategoryValues, i, i + (positionStride - 16));

        std::vector<double> TANGENT_DOUBLES = TANGENT_VALUES[count];

        std::vector<std::byte> TANGENT_X;
        std::vector<std::byte> TANGENT_Y;
        std::vector<std::byte> TANGENT_Z;
        std::vector<std::byte> TANGENT_W;

        double tangent_double_x = TANGENT_DOUBLES[0];

        TANGENT_X = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[0]);
        TANGENT_Y = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[1]);
        TANGENT_Z = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[2]);
        //TANGENT的W分量要么是-1要么是1，这是由当前顶点的手性决定的，这里我们使用Blender导出时自带的分量试一下
        //TANGENT_W = GetRange(PositionCategoryValues, i + 36, i + 40);
        TANGENT_W = MMTFormat_PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[3]);

        //LOG.LogOutput(L"Tangent: " + formatedFloat(TANGENT_DOUBLES[0],10) + L"," + formatedFloat(TANGENT_DOUBLES[1],10) + L"," + formatedFloat(TANGENT_DOUBLES[2], 10) + L"," + formatedFloat(TANGENT_DOUBLES[3], 10));

        newPositionCategoryValues.insert(newPositionCategoryValues.end(), POSITION_NORMAL_VALUES.begin(), POSITION_NORMAL_VALUES.end());

        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_X.begin(), TANGENT_X.end());
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_Y.begin(), TANGENT_Y.end());
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_Z.begin(), TANGENT_Z.end());
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_W.begin(), TANGENT_W.end());



        count = count + 1;
        //LOG.LogOutput(L"Count: " + std::to_wstring(count));
    }

    LOG.Info(L"Recalculate Tangent success! ");
    newVBCategoryDataMap = inputVBCategoryDataMap;
    newVBCategoryDataMap[L"Position"] = newPositionCategoryValues;
    return newVBCategoryDataMap;
};



//通过读取的参数，翻转Normal的X、Y、Z分量，翻转Tangent的X、Y、Z、W分量
std::unordered_map<std::wstring, std::vector<std::byte>> ReverseNormalTangentValues(
    std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig) {
    LOG.Info(L"Start to flip NORMAL and TANGENT values");
    std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
    std::vector<std::byte> PositionCategoryValues = finalVBCategoryDataMap[L"Position"];

    //这里要拿到TmpElementList，然后统计出属于Position槽位的元素有哪些
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
    std::vector<std::string> positionElementList;
    int positionStride = 0;
    for (std::string elementName : basicConfig.TmpElementList) {
        D3D11Element elementObject = d3d11GameType.ElementNameD3D11ElementMap[elementName];
        std::wstring category = MMTString_ToWideString(elementObject.Category);
        if (category == L"Position") {
            LOG.Info(L"Detect Position category element: " + MMTString_ToWideString(elementName) + L"  stride: " + std::to_wstring(elementObject.ByteWidth));
            positionElementList.push_back(elementName);
            positionStride += elementObject.ByteWidth;

        }
    }
    LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));

    // 然后动态的读取值，然后只修改NORMAL和TANGENT值
    std::vector<std::byte> newPositionCategoryValues;
    for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride) {

        std::vector<std::byte> newPOSITIONValues;
        //遍历positionElementList，但是只处理NORMAL和TANGENT，其他原封不动放到新列表
        int offset = 0;
        for (std::string elementName : positionElementList) {
            //LOG.LogOutput(L"Tmp  offset: " + std::to_wstring(offset));
            D3D11Element elementObject = d3d11GameType.ElementNameD3D11ElementMap[elementName];
            std::wstring format = MMTString_ToWideString(elementObject.Format);
            int elementStrie = elementObject.ByteWidth;

            //LOG.LogOutput(L"Tmp  format: " + format);
            //LOG.LogOutput(L"Tmp  elementStrie: " + std::to_wstring(elementStrie));


            std::vector<std::byte> TmpValues = MMTFormat_GetRange_Byte(PositionCategoryValues, i + offset, i + offset + elementStrie);
            if (elementName == "NORMAL") {
                // TODO 拆出分量，根据条件反转对应分量，目前我们NORMAL就两种类型，一种R32G32B32_FLOAT 一种R16G16B16A16_FLOAT而且A16全是0
                if (format == L"R32G32B32_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 4);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 4, 8);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 8, 12);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.NormalReverseX ? MMTFormat_Reverse32BitFloatValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.NormalReverseY ? MMTFormat_Reverse32BitFloatValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.NormalReverseZ ? MMTFormat_Reverse32BitFloatValue(ValueB) : ValueB;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else if (format == L"R16G16B16A16_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 2);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 2, 4);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 4, 6);
                    std::vector<std::byte> ValueA = MMTFormat_GetRange_Byte(TmpValues, 6, 8);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.NormalReverseX ? MMTFormat_Reverse16BitShortValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.NormalReverseY ? MMTFormat_Reverse16BitShortValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.NormalReverseZ ? MMTFormat_Reverse16BitShortValue(ValueB) : ValueB;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                    //NORMAL的A分量永远是0，不需要反转
                    FlipedValues.insert(FlipedValues.end(), ValueA.begin(), ValueA.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else {
                    newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());
                    //LOG.Warning(L"Can't find any format for NORMAL vlaues,use original NORMAL values");
                }

            }
            else if (elementName == "TANGENT") {

                if (format == L"R32G32B32A32_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 4);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 4, 8);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 8, 12);
                    std::vector<std::byte> ValueA = MMTFormat_GetRange_Byte(TmpValues, 12, 16);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.TangentReverseX ? MMTFormat_Reverse32BitFloatValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.TangentReverseY ? MMTFormat_Reverse32BitFloatValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.TangentReverseZ ? MMTFormat_Reverse32BitFloatValue(ValueB) : ValueB;
                    std::vector<std::byte> ValueA_Reversed = basicConfig.TangentReverseW ? MMTFormat_Reverse32BitFloatValue(ValueA) : ValueA;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueA_Reversed.begin(), ValueA_Reversed.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else if (format == L"R16G16B16A16_FLOAT") {
                    std::vector<std::byte> FlipedValues;

                    std::vector<std::byte> ValueR = MMTFormat_GetRange_Byte(TmpValues, 0, 2);
                    std::vector<std::byte> ValueG = MMTFormat_GetRange_Byte(TmpValues, 2, 4);
                    std::vector<std::byte> ValueB = MMTFormat_GetRange_Byte(TmpValues, 4, 6);
                    std::vector<std::byte> ValueA = MMTFormat_GetRange_Byte(TmpValues, 6, 8);

                    std::vector<std::byte> ValueR_Reversed = basicConfig.TangentReverseX ? MMTFormat_Reverse16BitShortValue(ValueR) : ValueR;
                    std::vector<std::byte> ValueG_Reversed = basicConfig.TangentReverseY ? MMTFormat_Reverse16BitShortValue(ValueG) : ValueG;
                    std::vector<std::byte> ValueB_Reversed = basicConfig.TangentReverseZ ? MMTFormat_Reverse16BitShortValue(ValueB) : ValueB;
                    std::vector<std::byte> ValueA_Reversed = basicConfig.TangentReverseW ? MMTFormat_Reverse16BitShortValue(ValueA) : ValueA;

                    FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                    FlipedValues.insert(FlipedValues.end(), ValueA_Reversed.begin(), ValueA_Reversed.end());

                    newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                }
                else {
                    newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());
                    //LOG.Warning(L"Can't find any format for TANGENT vlaues,use default TANGENT values.");
                }

            }
            else {
                //不反转的话直接添加进去就行了
                newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());

            }
            offset += elementStrie;

        }

        //处理完的POSITION values添加到总的Position槽位里
        newPositionCategoryValues.insert(newPositionCategoryValues.end(), newPOSITIONValues.begin(), newPOSITIONValues.end());
    }

    LOG.Info(L"Size: newPositionCategoryValues : " + std::to_wstring(newPositionCategoryValues.size()));
    LOG.Info(L"Size: PositionCategoryValues : " + std::to_wstring(PositionCategoryValues.size()));

    newVBCategoryDataMap = finalVBCategoryDataMap;
    newVBCategoryDataMap[L"Position"] = newPositionCategoryValues;
    return newVBCategoryDataMap;
};
