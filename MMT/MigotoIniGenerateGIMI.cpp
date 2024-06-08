#include "MigotoIniFile.h"
#include "MMTStringUtils.h"
#include "MMTJsonUtils.h"
#include <cmath>


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

    //���ɵ��ļ���ʹ��md5
    std::wstring outputIniFileName = splitOutputFolder + MMTString_GenerateUUIDW() + L".ini";
  

    std::wofstream outputIniFile(outputIniFileName);


    //1.TextureOverride VB����
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

        //������ȡ�����ڵ�ǰ����hash�½����滻�ķ��࣬�����Ӷ�Ӧ����Դ�滻
        for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
            std::string originalCategoryName = pair.first;
            std::string drawCategoryName = pair.second;
            if (categoryName == drawCategoryName) {
                std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                outputIniFile << MMTString_ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + basicConfig.DrawIB + MMTString_ToWideString(originalCategoryName) << std::endl;
            }
        }


        //drawһ�㶼����Blend��λ�Ͻ��еģ�������������Ҫ�ж�ȷ����BlendҪ�滻��hash���ܽ���draw��
        if (categoryName == d3d11GameType.CategoryDrawCategoryMap["Blend"]) {
            outputIniFile << "handling = skip" << std::endl;
            outputIniFile << "draw = " << std::to_wstring(drawNumber) << ", 0" << std::endl;
        }

        outputIniFile << std::endl;
    }


    //2.VertexLimitRaise����
    std::string VertexLimitVB = basicConfig.VertexLimitVB;
    //������drawNumber��ʵ�ֶ�̬����
    outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + L"_" + std::to_wstring(CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawNumber) + L"_VertexLimitRaise]" << std::endl;
    outputIniFile << L"hash = " + MMTString_ToWideString(VertexLimitVB) << std::endl << std::endl;


    //1.IB SKIP����
    outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + L"IB]" << std::endl;
    outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
    outputIniFile << "handling = skip" << std::endl;
    outputIniFile << std::endl;

    

    //2.IBOverride����
    for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
        std::string IBFirstIndex = basicConfig.MatchFirstIndexList[i];
        std::string partName = basicConfig.PartNameList[i];
        outputIniFile << L"[TextureOverride" + basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)] + L"]" << std::endl;
        outputIniFile << L"hash = " + basicConfig.DrawIB << std::endl;
        outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
        outputIniFile << L"ib = Resource" + basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)] << std::endl;
        outputIniFile << "drawindexed = auto" << std::endl;
        outputIniFile << std::endl;

    }

    outputIniFile << L"; CommandList -------------------------" << std::endl << std::endl;
    outputIniFile << L"; Resources -------------------------" << std::endl << std::endl;

    //7.д����ͼ��λ����
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

    //5.д��VBResource����
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
        //vb�ļ����ļ���
        outputIniFile << "filename = " << MMTString_ToWideString(this->categoryUUIDMap[categoryName]) + L".buf"  << std::endl << std::endl;
    }


    //6.д��IBResource����
    for (int i = 0; i < basicConfig.PartNameList.size(); ++i) {
        std::string partName = basicConfig.PartNameList[i];
        outputIniFile << L"[Resource" + basicConfig.DrawIB + wheelConfig.GIMIPartNameAliasMap[MMTString_ToWideString(partName)] + L"]" << std::endl;
        outputIniFile << "type = Buffer" << std::endl;
        outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
        //ib�ļ����ļ�����Ϊmd5
        outputIniFile << "filename = " << MMTString_ToWideString(this->partNameUUIDMap[partName]) + L".ib"  << std::endl << std::endl;
    }

    outputIniFile << L"; Ini generated by MMT-GIMISimulator(Migoto Mod Tool's GIMI format ini generator)." << std::endl << std::endl;
    outputIniFile << L"; Mod Generated by MMT-Community." << std::endl << std::endl;
    outputIniFile << L"; Github: https://github.com/StarBobis/MigotoModTool" << std::endl << std::endl;
    outputIniFile << L"; Discord: https://discord.gg/Cz577BcRf5" << std::endl << std::endl;

    //д��ر��ļ�
    outputIniFile.close();
    LOG.NewLine();
}