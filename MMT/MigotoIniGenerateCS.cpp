#include "MigotoIniFile.h"
#include "MMTStringUtils.h"
#include "MMTJsonUtils.h"
#include <cmath>


MigotoIniFile::MigotoIniFile() {

}


MigotoIniFile::MigotoIniFile(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType) {
    this->wheelConfig = wheelConfig;
    this->basicConfig = basicConfig;
    this->d3d11GameType = d3d11GameType;
}


//专门给Naraka准备的
void MigotoIniFile::generateOriginalIniFileCS() {
    //------------------------------------------------------------------------------------------------------------------
    std::wstring outputIniConfigFileName = splitOutputFolder + L"IniConfig.json";
    json iniConfigJson;
    iniConfigJson["DrawIB"] = MMTString_ToByteString(basicConfig.DrawIB);
    iniConfigJson["partNameUUIDMap"] = partNameUUIDMap;
    iniConfigJson["patchBLENDWEIGHTS"] = d3d11GameType.PatchBLENDWEIGHTS;
    iniConfigJson["blendElementByteWidth"] = blendElementByteWidth;
    iniConfigJson["CategoryStrideMap"] = CategoryStrideMap;
    iniConfigJson["categoryUUIDMap"] = categoryUUIDMap;
    iniConfigJson["CategoryList"] = CategoryList;
    iniConfigJson["CategoryHashMap"] = basicConfig.CategoryHashMap;
    iniConfigJson["CategorySlotMap"] = d3d11GameType.CategorySlotMap;
    iniConfigJson["CategoryDrawCategoryMap"] = d3d11GameType.CategoryDrawCategoryMap;
    iniConfigJson["DrawNumber"] = drawNumber;
    iniConfigJson["VertexLimitVB"] = basicConfig.VertexLimitVB;
    iniConfigJson["PartNameList"] = basicConfig.PartNameList;
    iniConfigJson["MatchFirstIndexList"] = basicConfig.MatchFirstIndexList;
    iniConfigJson["TextureMap"] = basicConfig.TextureMap;
    MMTJson_SaveToJsonFile(outputIniConfigFileName, iniConfigJson);


    //------------------------------------------------------------------------------------------------------------------
    LOG.Info(L"Start to output ini file.");

    //生成的文件名使用md5
    std::wstring outputIniFileName = splitOutputFolder + MMTString_GenerateUUIDW() + L".ini";
    std::wofstream outputIniFile(outputIniFileName);
    std::wstring replace_prefix = L"";

    //1.IB SKIP部分
    outputIniFile << L"[TextureOverride_" + basicConfig.DrawIB + L"_IB_SKIP]" << std::endl;
    outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
    outputIniFile << replace_prefix << "handling = skip" << std::endl;
    outputIniFile << std::endl;

    //2.IBOverride部分
    for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
        std::string IBFirstIndex = basicConfig.MatchFirstIndexList[i];
        std::string partName = basicConfig.PartNameList[i];
        outputIniFile << L"[TextureOverride_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
        outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
        outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
        outputIniFile << replace_prefix << L"ib = Resource_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) << std::endl;
        outputIniFile << replace_prefix << "drawindexed = auto" << std::endl;
        outputIniFile << std::endl;

    }


    //3.VertexLimitRaise部分
    std::string VertexLimitVB = basicConfig.VertexLimitVB;
    //步长，drawNumber，实现动态步长
    outputIniFile << L"[TextureOverride_VB_" + basicConfig.DrawIB + L"_" + std::to_wstring(CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawNumber) + L"_VertexLimitRaise]" << std::endl;
    outputIniFile << L"hash = " + MMTString_ToWideString(VertexLimitVB) << std::endl << std::endl;


    //4.TextureOverride VB部分
    LOG.Info(L"Start to output TextureOverrideVB section");
    for (std::string categoryName : CategoryList) {
        std::string categoryHash = basicConfig.CategoryHashMap[categoryName];
        std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
        LOG.Info(L"categoryName: " + MMTString_ToWideString(categoryName) + L" categorySlot: " + MMTString_ToWideString(categorySlot));

        outputIniFile << L"[TextureOverride_VB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(categoryName) + L"]" << std::endl;
        outputIniFile << L"hash = " + MMTString_ToWideString(categoryHash) << std::endl;

        //遍历获取所有在当前分类hash下进行替换的分类，并添加对应的资源替换
        for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
            std::string originalCategoryName = pair.first;
            std::string drawCategoryName = pair.second;
            if (categoryName == drawCategoryName) {
                std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];

                if (originalCategoryName == "Position") {
                    outputIniFile << L"cs-cb0 = Resource_VertexLimit" << std::endl;
                    outputIniFile << L"cs-t0 = Resource_VB_Position" << std::endl;
                    outputIniFile << L"cs-t1 = Resource_VB_Blend"  << std::endl;
                    outputIniFile << L"handling = skip"  << std::endl;

                    int dispatchNumber =(int)(ceil(drawNumber / 64) + 1);
                    outputIniFile << L"dispatch = "<< std::to_wstring(dispatchNumber) << L",1,1" << std::endl;

                }
                else if (originalCategoryName == "Blend") {

                }
                else {
                    outputIniFile << replace_prefix << MMTString_ToWideString(categoryOriginalSlot) + L" = " + L"Resource_VB_" + MMTString_ToWideString(originalCategoryName) << std::endl;
                }


            }
        }


        outputIniFile << std::endl;
    }

    outputIniFile << L"[Resource_VertexLimit]" << std::endl;
    outputIniFile << L"type = buffer" << std::endl;
    outputIniFile << L"format = R32G32B32A32_UINT" << std::endl;
    outputIniFile << L"data = "<< std::to_wstring(drawNumber) << L" 0 0 0" << std::endl << std::endl;


    //5.写出VBResource部分
    for (std::string categoryName : CategoryList) {
        outputIniFile << L"[Resource_VB_" + MMTString_ToWideString(categoryName) + L"]" << std::endl;

        if (categoryName == "Position" || categoryName == "Blend") {
            outputIniFile << L"type = ByteAddressBuffer" << std::endl;
        }
        else {
            outputIniFile << L"type = Buffer" << std::endl;
        }

        if (categoryName == "Blend" && d3d11GameType.PatchBLENDWEIGHTS) {
            int finalBlendStride = CategoryStrideMap[categoryName] - blendElementByteWidth;
            outputIniFile << "stride = " << std::to_wstring(finalBlendStride) << std::endl;

        }
        else {
            outputIniFile << "stride = " << CategoryStrideMap[categoryName] << std::endl;

        }
        //vb文件的文件名
        outputIniFile << "filename = " << MMTString_ToWideString(this->categoryUUIDMap[categoryName]) + L".buf"  << std::endl << std::endl;
    }


    //6.写出IBResource部分
    for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
        std::string partName = basicConfig.PartNameList[i];
        outputIniFile << L"[Resource_IB_" + basicConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
        outputIniFile << "type = Buffer" << std::endl;
        outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
        //ib文件，文件名换为md5
        outputIniFile << "filename = " << MMTString_ToWideString(this->partNameUUIDMap[partName]) + L".ib" << std::endl << std::endl;
    }


    //7.写出贴图槽位部分
    for (const auto& pair : basicConfig.TextureMap) {
        std::string textureName = pair.first;
        std::string textureHash = pair.second;
        std::vector<std::wstring> splits;
        boost::split(splits, textureName, boost::is_any_of("."));
        std::wstring textureResourceName = splits[0];
        outputIniFile << L"[Resource_" << textureResourceName << L"]" << std::endl;
        outputIniFile << L"filename = " << MMTString_ToWideString(textureName) << std::endl << std::endl;
        outputIniFile << L"[TextureOverride_" << textureResourceName << L"]" << std::endl;
        outputIniFile << "hash = " << MMTString_ToWideString(textureHash) << std::endl;

        outputIniFile << replace_prefix << L"this = Resource_" + textureResourceName << std::endl;

        outputIniFile << std::endl;
    }


    //写完关闭文件
    outputIniFile.close();
    LOG.NewLine();
}

