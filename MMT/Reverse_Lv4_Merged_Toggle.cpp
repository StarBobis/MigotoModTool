#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "ModFormatExtra.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"


//TODO Lv3已经过时了,完成这个之前需要先完善&& 和 || 的解析支持
//TODO 修改贴图生成的位置在.ib和.vb文件旁边。
//TODO 有些贴图是不根据变量变化的，也要一起复制，方便使用。
//TODO 记得添加CommandList的解析，因为有些是使用CommandList里放着要执行的命令。
void Reverse_Lv4_Merged_Toggle() {
    //初始化
    LOG.Info("Start to reverse single mod.");
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);
    std::wstring reverseFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath) + L"-Reverse\\";
    std::filesystem::create_directories(reverseFolderPath);
    LOG.Info(L"ReverseFilePath: " + reverseFilePath);
    LOG.NewLine();

    //初始化通用ini解析对象
    ModFormat_INI modFormatUnity(reverseFilePath);



}