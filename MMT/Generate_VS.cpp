#include "GlobalConfigs.h"
#include "D3d11GameType.h"
#include "MigotoIniFile.h"
#include "GenerateUtil.h"



void UnityGenerate() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring drawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        extractConfig.GameType = MMTString_ToWideString(extractConfig.WorkGameType);
        D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[extractConfig.WorkGameType];
        LOG.Info("Initialize d3d11GameType over: " + d3d11GameType.GameType);
        //获取当前日期 YYYY_MM_DD
        std::wstring timeStr = MMTString_GetFormattedDateTimeForFilename().substr(0, 10);
        // 指定输出目录
        std::wstring splitReadFolder = G.OutputFolder + drawIB + L"/";
        std::wstring splitOutputFolder = G.OutputFolder + timeStr + L"/" + drawIB + L"/";

        //先判断是否存在1.ib什么的
        bool findValidFile = false;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
            if (std::filesystem::exists(splitReadFolder + VBFileName)) {
                findValidFile = true;
                break;
            }
        }
        if (!findValidFile) {
            LOG.Info(L"Detect didn't export vb file for DrawIB: " + drawIB + L" , so skip this drawIB split.");
            continue;
        }
        //能分割再创建分割文件夹
        std::filesystem::create_directories(splitOutputFolder);

        SplitUtil splitUtil(G,extractConfig,d3d11GameType);
        splitUtil.splitReadFolder = splitReadFolder;
        splitUtil.splitOutputFolder = splitOutputFolder;

        //（1）读取并重算Color和Tangent，然后分割
        splitUtil.readSplitRecalculate();

        //（2）输出分割好的文件
        splitUtil.outputModFiles();

        //（3）调用生成Wheel格式的ini文件
        MigotoIniFile wheelIniFile(G, extractConfig, d3d11GameType);

        wheelIniFile.splitOutputFolder = splitOutputFolder;
        wheelIniFile.CategoryList = splitUtil.CategoryList;
        wheelIniFile.CategoryStrideMap = splitUtil.CategoryStrideMap;
        wheelIniFile.blendElementByteWidth = splitUtil.blendElementByteWidth;
        wheelIniFile.drawNumber = splitUtil.drawNumber;

        wheelIniFile.categoryUUIDMap = splitUtil.categoryUUIDMap;
        wheelIniFile.partNameUUIDMap = splitUtil.partNameUUIDMap;

        //wheelIniFile.generateIniFile();
        wheelIniFile.generateINIFileGIMI();

        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

    }

}

