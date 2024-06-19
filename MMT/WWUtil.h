#pragma once
#include <iostream>
#include <unordered_map>
class WuwaCSInfo {
public:
    //之前已计算顶点的总和
    int Offset;
    //当前CS计算次数，也就是当前的部件的顶点数
    int CalculateTime;
    //使用哪个Shader来进行Pre-Skinning
    std::string ComputeShaderHash;
};


class WuwaCSInfoJsonObject {
public:

    //这里负责WuwaCSInfoJsonObject的写出和读取
    std::unordered_map<std::string, WuwaCSInfo> PartNameWuwaCSInfoMap;

    void saveToJsonFile(std::wstring outputFolder);

    WuwaCSInfoJsonObject();

    WuwaCSInfoJsonObject(std::wstring readFolderPath);
};