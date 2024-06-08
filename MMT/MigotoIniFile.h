#pragma once

#include "GlobalConfigs.h"


class MigotoIniFile {
public:
    GlobalConfigs wheelConfig;
    ExtractConfig basicConfig;
    D3D11GameType d3d11GameType;
    std::wstring splitOutputFolder;
    std::vector<std::string> CategoryList;
    std::unordered_map<std::string, int> CategoryStrideMap;
    int blendElementByteWidth = 0;
    uint64_t drawNumber = 0;

    //一个partName，UUID的map供后面ib文件名使用
    std::unordered_map<std::string, std::string> partNameUUIDMap;

    //一个Category，UUID的map供后面buf文件名使用
    std::unordered_map<std::string, std::string> categoryUUIDMap;



    //生成永劫Mod
    void generateOriginalIniFileCS();

    //模仿GIMI生成ini格式
    void generateINIFileGIMI();


    MigotoIniFile();
    MigotoIniFile(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType);
};

