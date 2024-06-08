#include "D3d11GameType.h"
#include "IniParser.h"
#include "GlobalConfigs.h"


void ReverseSingle() {
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring GameName = MMTString_ToWideString(reverseJsonObject["GameName"]);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);
    LOG.Info(L"reverseFilePath: " + reverseFilePath);
    LOG.Info(L"GameName: " + GameName);

    ReverseModelSingle reverseModel(reverseFilePath);
    reverseModel.ApplicationRunningLoaction = G.ApplicationRunningLocation;

    LOG.Info(L"Parse ini info finished.");
    LOG.NewLine();

    reverseModel.parseElementList(GameName);
    reverseModel.reverseAndOutput();
}


void ReverseMerged() {
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring GameName = MMTString_ToWideString(reverseJsonObject["GameName"]);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);
    LOG.Info(L"reverseFilePath: " + reverseFilePath);
    LOG.Info(L"GameName: " + GameName);

    parseMergedInI(reverseFilePath, GameName, G.ApplicationRunningLocation);

    LOG.NewLine();
}