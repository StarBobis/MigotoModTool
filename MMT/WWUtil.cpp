#include "WWUtil.h"
#include "MMTJsonUtils.h"
#include "MMTLogUtils.h"
#include "GlobalConfigs.h"

void WuwaCSInfoJsonObject::saveToJsonFile(std::wstring outputFolder) {
    std::wstring jsonFilePath = outputFolder + L"wwinfo.json";
    //写出原始的顶点数对应索引和CS信息
    json wuwaCsInfoJson;
    for (const auto& pair : this->PartNameWuwaCSInfoMap) {
        WuwaCSInfo csInfo = pair.second;
        LOG.Info("VertexCount: " + std::to_string(csInfo.CalculateTime) + "  Offset:" + std::to_string(csInfo.Offset) + "  ComputeShaderHash:" + csInfo.ComputeShaderHash);
        json csInfoJson;
        csInfoJson["CalculateTime"] = std::to_string(csInfo.CalculateTime);
        csInfoJson["Offset"] = std::to_string(csInfo.Offset);
        csInfoJson["ComputeShaderHash"] = csInfo.ComputeShaderHash;

        //这里key是partName
        wuwaCsInfoJson[pair.first] = csInfoJson;
    }
    //写出到文件
    if (std::filesystem::exists(jsonFilePath)) {
        std::filesystem::remove(jsonFilePath);
    }
    std::ofstream file(jsonFilePath);
    std::string content = wuwaCsInfoJson.dump(4); // 使用缩进格式保存到文件，缩进为4个空格
    file << content << std::endl;
    file.close();
}



WuwaCSInfoJsonObject::WuwaCSInfoJsonObject() {

}

WuwaCSInfoJsonObject::WuwaCSInfoJsonObject(std::wstring readFolderPath) {
    std::wstring readJsonPath = readFolderPath + L"wwinfo.json";
    if (std::filesystem::exists(readJsonPath)) {
        json wwcsinfoJson = MMTJson_ReadJsonFromFile(readJsonPath);
        WuwaCSInfo wwcsInfo;
        for (const auto& item : wwcsinfoJson.items()) {
            std::string CalculateTime = item.value()["CalculateTime"];
            std::string Offset = item.value()["Offset"];
            std::string ComputeShaderHash = item.value()["ComputeShaderHash"];
            LOG.Info("CalculateTime: " + CalculateTime);
            LOG.Info("Offset: " + Offset);
            LOG.Info("ComputeShaderHash: " + ComputeShaderHash);
            wwcsInfo.CalculateTime = std::stoi(CalculateTime);
            wwcsInfo.Offset = std::stoi(Offset);
            wwcsInfo.ComputeShaderHash = ComputeShaderHash;
            this->PartNameWuwaCSInfoMap[item.key()] = wwcsInfo;
        }
    }
}
