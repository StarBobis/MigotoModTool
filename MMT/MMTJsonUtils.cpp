#include "MMTJsonUtils.h"
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"

//用于读取Json文件到对象
nlohmann::json MMTJson_ReadJsonFromFile(const std::wstring& filenamew) {
    std::string filename = MMTString_ToByteString(filenamew);

    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG.Info("Could not open file for reading: " + filename);
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    std::string content = ss.str();
    ss.clear();

    nlohmann::json jsonData = nlohmann::json::parse(content);
    return jsonData;
}


void MMTJson_SaveToJsonFile(std::wstring jsonOutputPath, nlohmann::json jsonObject) {
    std::ofstream outputJsonFile(jsonOutputPath);
    outputJsonFile << jsonObject.dump(4);
    outputJsonFile.close();
}


