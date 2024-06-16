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


void MigotoIniFile::generateINIFileGIMI() {
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
    std::wstring outputIniFileName = splitOutputFolder + basicConfig.DrawIB + L".ini";
    std::wofstream outputIniFile(outputIniFileName);


    //1.TextureOverride VB部分，只有使用GPU-PreSkinning时是直接替换hash对应槽位
    if (d3d11GameType.GPUPreSkinning) {
        LOG.Info(L"Start to output TextureOverrideVB section");
        outputIniFile << L"; " + basicConfig.DrawIB  + L" -------------------------" << std::endl << std::endl;
        outputIniFile << L"; Constants -------------------------" << std::endl << std::endl;
        outputIniFile << L"; Overrides -------------------------" << std::endl << std::endl;
        for (std::string categoryName : CategoryList) {
            std::string categoryHash = basicConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
            LOG.Info(L"categoryName: " + MMTString_ToWideString(categoryName) + L" categorySlot: " + MMTString_ToWideString(categorySlot));

            outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + MMTString_ToWideString(categoryName) + L"]" << std::endl;
            outputIniFile << L"hash = " + MMTString_ToWideString(categoryHash) << std::endl;

            //遍历获取所有在当前分类hash下进行替换的分类，并添加对应的资源替换
            for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                std::string originalCategoryName = pair.first;
                std::string drawCategoryName = pair.second;
                if (categoryName == drawCategoryName) {
                    std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                    outputIniFile << MMTString_ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + basicConfig.DrawIB + MMTString_ToWideString(originalCategoryName) << std::endl;
                }
            }

            //draw一般都是在Blend槽位上进行的，所以我们这里要判断确定是Blend要替换的hash才能进行draw。
            if (categoryName == d3d11GameType.CategoryDrawCategoryMap["Blend"]) {
                outputIniFile << "handling = skip" << std::endl;
                outputIniFile << "draw = " << std::to_wstring(drawNumber) << ", 0" << std::endl;
            }

            outputIniFile << std::endl;
        }

        //2.VertexLimitRaise部分，这里只有使用到GPU-PreSkinning技术时才需要突破顶点数量限制
        std::string VertexLimitVB = basicConfig.VertexLimitVB;
        //步长，drawNumber，实现动态步长
        outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + L"_" + std::to_wstring(CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawNumber) + L"_VertexLimitRaise]" << std::endl;
        outputIniFile << L"hash = " + MMTString_ToWideString(VertexLimitVB) << std::endl << std::endl;
    }
    
    //1.IB SKIP部分
    outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + L"IB]" << std::endl;
    outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
    outputIniFile << "handling = skip" << std::endl;
    outputIniFile << std::endl;

    //2.IBOverride部分
    for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
        std::string IBFirstIndex = basicConfig.MatchFirstIndexList[i];
        std::string partName = basicConfig.PartNameList[i];
        outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)] + L"]" << std::endl;
        outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
        outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
        outputIniFile << L"ib = Resource" + basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)] << std::endl;

        //如果不使用GPU-Skinning即为Object类型，此时需要在ib下面替换对应槽位
        if (!d3d11GameType.GPUPreSkinning) {
            for (std::string categoryName : CategoryList) {
                std::string categoryHash = basicConfig.CategoryHashMap[categoryName];
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                LOG.Info(L"categoryName: " + MMTString_ToWideString(categoryName) + L" categorySlot: " + MMTString_ToWideString(categorySlot));

                //遍历获取所有在当前分类hash下进行替换的分类，并添加对应的资源替换
                for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                    std::string originalCategoryName = pair.first;
                    std::string drawCategoryName = pair.second;
                    if (categoryName == drawCategoryName) {
                        std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                        outputIniFile << MMTString_ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + basicConfig.DrawIB + MMTString_ToWideString(originalCategoryName) << std::endl;
                    }
                }
            }
        }

        outputIniFile << "drawindexed = auto" << std::endl;
        outputIniFile << std::endl;

    }

    outputIniFile << L"; CommandList -------------------------" << std::endl << std::endl;
    outputIniFile << L"; Resources -------------------------" << std::endl << std::endl;

    //7.写出贴图槽位部分
    for (const auto& pair : basicConfig.TextureMap) {
        std::string textureName = pair.first;
        std::string textureHash = pair.second;
        std::vector<std::wstring> splits;
        boost::split(splits, textureName, boost::is_any_of("."));
        std::wstring textureResourceName = splits[0];
        outputIniFile << L"[Resource" << textureResourceName << L"]" << std::endl;
        outputIniFile << L"filename = " << MMTString_ToWideString(textureName) << std::endl << std::endl;
        outputIniFile << L"[TextureOverride" << textureResourceName << L"]" << std::endl;
        outputIniFile << "hash = " << MMTString_ToWideString(textureHash) << std::endl;

        outputIniFile << L"this = Resource" + textureResourceName << std::endl;

        outputIniFile << std::endl;
    }

    //5.写出VBResource部分
    for (std::string categoryName : CategoryList) {
        outputIniFile << L"[Resource" + basicConfig.DrawIB + MMTString_ToWideString(categoryName) + L"]" << std::endl;
        outputIniFile << L"type = Buffer" << std::endl;

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
        outputIniFile << L"[Resource" + basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)] + L"]" << std::endl;
        outputIniFile << "type = Buffer" << std::endl;
        outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
        //ib文件，文件名换为md5
        outputIniFile << "filename = " << MMTString_ToWideString(this->partNameUUIDMap[partName]) + L".ib"  << std::endl << std::endl;
    }

    outputIniFile << L"; Ini generated by MMT-GIMISimulator(Migoto Mod Tool's GIMI format ini generator)." << std::endl;
    outputIniFile << L"; Mod Generated by MMT-Community." << std::endl;
    outputIniFile << L"; Github: https://github.com/StarBobis/MigotoModTool" << std::endl;
    outputIniFile << L"; Discord: https://discord.gg/Cz577BcRf5" << std::endl << std::endl;

    //写完关闭文件
    outputIniFile.close();
    LOG.NewLine();
}