#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MigotoParseUtil.h"
#include "ModFormatUnity.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"


void Reverse_Lv4_Merged_OutfitCompiler() {
    //初始化
    LOG.Info("Start to reverse outfit_compiler.py format merged mod.");
    json reverseJsonObject = MMTJson_ReadJsonFromFile(G.Path_RunInputJson);
    std::wstring reverseFilePath = MMTString_ToWideString(reverseJsonObject["ReverseFilePath"]);
    std::wstring reverseFolderPath = MMTString_GetFolderPathFromFilePath(reverseFilePath) + L"-Reverse\\";
    std::filesystem::create_directories(reverseFolderPath);
    LOG.Info(L"ReverseFilePath: " + reverseFilePath);
    LOG.NewLine();

    //初始化通用ini解析对象
    ModFormat_Unity_INI modFormatUnity(reverseFilePath);
    modFormatUnity.Parse_Hash_SingleModDetect_Map();
    LOG.NewLine();

    for (const auto& singleModPair: modFormatUnity.Hash_SingleModDetect_Map) {
        std::wstring DrawIBHashValue = singleModPair.first;
        SingleModDetect singleModDetect = singleModPair.second;

        std::vector<M_Resource> ResourceVBList = singleModDetect.ResourceVBList;
        std::vector<M_TextureOverride> TextureOverrideIBList = singleModDetect.TextureOverrideIBList;
        LOG.Info(L"Mod Hash:" + DrawIBHashValue);
        LOG.Info("ResourceVBList Size: " + std::to_string(ResourceVBList.size()));
        LOG.Info("TextureOverrideIBList Size: " + std::to_string(TextureOverrideIBList.size()));
        LOG.NewLine();

        //直接开始解析ElementList,遍历所有的D3D11数据类型来解析出可能满足的
        int possibleModelNumber = 0;
        for (D3D11GameType d3d11GameType : G.CurrentD3d11GameTypeList) {
            //这里的SingleMod主要作用是试图识别当前类型的ElementList来拼接出一个合理的Mod，拼接不出来的话ValidMod值就会是false
            ValidModDetect singleMod(d3d11GameType, ResourceVBList, TextureOverrideIBList);

            //如果不是合法的Mod直接跳过，减少磁盘占用，反正生成出来也没啥用，数据类型肯定都是错的
            if (!singleMod.ValidMod) {
                LOG.Info("Not a valid mod for this gametype: " + d3d11GameType.GameType + " so skip this.");
                LOG.NewLine();
                continue;
            }
            else {
                possibleModelNumber++;
            }

            //拼接并创建输出用的对应GameType目录
            std::wstring OutputGameTypeFolderPath = reverseFolderPath + DrawIBHashValue+ L"_"+ MMTString_ToWideString(d3d11GameType.GameType) + L"\\";
            std::filesystem::create_directories(OutputGameTypeFolderPath);

            //然后直接输出，每个IB都要根据DrawIBList进行分割VB文件并输出
            for (M_TextureOverride textureOverrideIB : TextureOverrideIBList) {
                for (M_DrawIndexed drawIndexed : textureOverrideIB.DrawIndexedList) {

                    //拼接输出的文件路径
                    std::wstring outputFileNamePrefix = drawIndexed.DrawNumber + L"_" + drawIndexed.DrawOffsetIndex;
                    std::wstring outputIBFilePath = OutputGameTypeFolderPath + outputFileNamePrefix + L".ib";
                    std::wstring outputFmtFilePath = OutputGameTypeFolderPath + outputFileNamePrefix + L".fmt";
                    std::wstring outputVBFilePath = OutputGameTypeFolderPath + outputFileNamePrefix + L".vb";
                    LOG.Info(L"Output IBFileName: " + outputIBFilePath);
                    LOG.Info(L"Output VBFileName: " + outputVBFilePath);
                    LOG.Info(L"Output FMTFileName: " + outputFmtFilePath);
                    LOG.NewLine();

                    //获取当前DrawIndexed的绘制顶点数和绘制偏移
                    int drawNumber = std::stoi(drawIndexed.DrawNumber);
                    int drawOffset = std::stoi(drawIndexed.DrawOffsetIndex);

                    //根据当前DrawIndexed的绘制顶点数和绘制偏移，从IB文件中截取出DrawIndexed绘制的那些IB数据，并设为当前ibBufFile的IB数据，随后进行输出
                    IndexBufferBufFile ibBufFile = textureOverrideIB.IBBufFile;
                    ibBufFile.SelfDivide(drawOffset, drawOffset + drawNumber);
                    ibBufFile.SaveToFile_UINT32(outputIBFilePath, ibBufFile.MinNumber * -1);


                    //设置并输出FMT文件
                    FmtFileData fmtFile;
                    fmtFile.ElementNameList = singleMod.ElementList;
                    fmtFile.d3d11GameType = d3d11GameType;
                    fmtFile.Format = L"DXGI_FORMAT_R32_UINT";
                    fmtFile.OutputFmtFile(outputFmtFilePath);

                    //如果Size为0则不进行输出，并直接弹出错误人工处理，正常情况这里是不可能触发为0的
                    if (singleMod.finalVB0Bytes.size() == 0) {
                        LOG.Error(L"Can't output because finalVB0Bytes size is 0,pleae check your mod ini.");
                    }

                    //这里要注意，MinNumber和MaxNumber都+1的原因是VB0IndexNumberBytes的索引在读取数据时是从1开始的
                    //所以当MinNumber最小值为0时，对应第一个数据，对应在VB0IndexNumberBytes里的索引就是1，所以这里要+1
                    uint32_t readNumberOffset = ibBufFile.MinNumber + 1;
                    //这里每个VB文件还是得根据当前IB的Buffer的最大最小值来取一部分，以确保顶点数和IB文件中顶点数量相同
                    std::vector<byte> outputBytes;
                    for (readNumberOffset; readNumberOffset <= ibBufFile.MaxNumber + 1; readNumberOffset++) {
                        std::vector<byte> indexBytes = singleMod.VB0IndexNumberBytes[readNumberOffset];
                        outputBytes.insert(outputBytes.end(), indexBytes.begin(), indexBytes.end());
                    }

                    //输出最终截取后的vb0文件
                    std::ofstream outputVBFile(outputVBFilePath, std::ios::binary);
                    outputVBFile.write(reinterpret_cast<const char*>(outputBytes.data()), outputBytes.size());
                    outputVBFile.close();
                }
            }
        }

        //所有类型都匹配不到的话就弹出一个提示
        if (possibleModelNumber == 0) {
            LOG.Error(L"Can't match any D3D11Element type,please check if it's a valid mod,if it's valid then contact NicoMico for help or open a issue on github with your mod file in attachment.");
        }
    }
}