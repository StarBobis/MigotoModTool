#include <set>

#include "D3d11GameType.h"
#include "IndexBufferTxtFile.h"
#include "VertexBufferBufFile.h"
#include "VertexBufferTxtFile.h"
#include "GlobalConfigs.h"
#include "GlobalFunctions.h"
#include "FrameAnalysisData.h"


void Extract_VS_UE4() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("Extract Buffer from SnowBreak:");
        LOG.NewLine();

        std::wstring MatchedGameType = extractConfig.GameType;
        if (extractConfig.GameType == L"Auto") {
            //进行自动类型识别并重新赋值GameType,这里用排除法通过对比实际stride与预计stride是否相同，来排除每个类型。
            //最终得到所有可能的类型列表，如果类型列表的数量不止一个，则报错并提示所有可能的类型
            //如果只有一个类型，则使用那个类型来进行提取

            std::vector<std::string> MatchedGameTypeList;
            LOG.Info("Start to auto detect game type:");
            for (const auto& pair: G.GameTypeName_D3d11GameType_Map) {
                std::string GameType = pair.first;
                D3D11GameType d3d11GameType = pair.second;
                LOG.Info("Try Match GameType: " + d3d11GameType.GameType);

                FrameAnalysisData FAData(G.WorkFolder);
                std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
                LOG.Info("Read FrameAnalysis file name list success.");

                // 从vb0中读取到顶点数量然后在下面的过程中，UE4的VB0一般固定为POSITION长度固定为2
                // 限制顶点数量匹配到我们的DrawNumber来设置对应的索引
                std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
                std::wstring VSExtractIndex;
                std::wstring PositionExtractFileName = L"";
                int MatchNumber = 0;
                int POSITION_ByteWidth = d3d11GameType.ElementNameD3D11ElementMap["POSITION"].ByteWidth;
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".txt")) {
                        continue;
                    }
                    if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
                        continue;
                    }
                    IndexBufferTxtFile ibFileData(G.WorkFolder + filename, false);
                    VSExtractIndex = ibFileData.Index;
                    PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vb0=", L".buf")[0];
                    int POSITION_FileSize = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName);
                    MatchNumber = POSITION_FileSize / POSITION_ByteWidth;
                    LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

                    LOG.Info(filename);
                    if (ibFileData.FirstIndex == L"") {
                        LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
                        continue;
                    }
                    int matchFirstIndex = std::stoi(ibFileData.FirstIndex);
                    matchFirstIndexIBFileNameMap[matchFirstIndex] = filename;
                }
                for (const auto& pair : matchFirstIndexIBFileNameMap) {
                    LOG.Info("Match First Index: " + std::to_string(pair.first));
                }
                LOG.NewLine();


                std::wstring NormalExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Normal"]) + L"=";
                std::wstring TexcoordExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Texcoord"]) + L"=";
                std::wstring ColorExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Color"]) + L"=";
                std::wstring BlendExtractSlot = L"-" + MMTString_ToWideString(d3d11GameType.CategorySlotMap["Blend"]) + L"=";
                LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
                LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
                LOG.Info(L"Color Extract Slot: " + ColorExtractSlot);
                LOG.Info(L"Blend Extract Slot: " + BlendExtractSlot);
                LOG.NewLine();
                //收集各个槽位的内容，并组合成VB0的内容
                std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];
                std::wstring TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
                std::wstring ColorExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + ColorExtractSlot, L".buf")[0];
                std::wstring BlendExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + BlendExtractSlot, L".buf")[0];

                //这里的变量名放到上面初始化了
                LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);
                LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
                LOG.Info(L"ColorExtractFileName: " + ColorExtractFileName);
                LOG.Info(L"BlendExtractFileName: " + BlendExtractFileName);
                LOG.NewLine();

                int PositionRealStride = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName) / MatchNumber;
                int NormalRealStride = MMTFile_GetFileSize(G.WorkFolder + NormalExtractFileName) / MatchNumber;
                int TexcoordRealStride = MMTFile_GetFileSize(G.WorkFolder + TexcoordExtractFileName) / MatchNumber;
                int ColorRealStride = MMTFile_GetFileSize(G.WorkFolder + ColorExtractFileName) / MatchNumber;
                int BlendRealStride = MMTFile_GetFileSize(G.WorkFolder + BlendExtractFileName) / MatchNumber;

                std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
                if (PositionRealStride != CategoryStrideMap["Position"]) {
                    continue;
                }
                else if (NormalRealStride != CategoryStrideMap["Normal"]) {
                    continue;
                }
                else if (TexcoordRealStride != CategoryStrideMap["Texcoord"]) {
                    continue;
                }
                else if (ColorRealStride != CategoryStrideMap["Color"]) {
                    continue;
                }
                else if (BlendRealStride != CategoryStrideMap["Blend"]) {
                    continue;
                }
                //如果能执行到这里说明可以添加到匹配到的类型列表中
                MatchedGameTypeList.push_back(GameType);
            }

            if (MatchedGameTypeList.size() == 0) {
                LOG.Error("Can't find any GameType using auto game type detect, try to manually set GameType if you think this exists in our GameType,or contact NicoMico to add this new GameType.");
            }
            else if (MatchedGameTypeList.size() == 1) {
                MatchedGameType = MMTString_ToWideString(MatchedGameTypeList[0]);
                LOG.Info(L"Auto GameType detect success! Current GameType: " + MatchedGameType);
                LOG.NewLine();
            }
            else {
                std::string GameTypeListString = "";
                for (std::string matchedGameType : MatchedGameTypeList) {
                    GameTypeListString = GameTypeListString + matchedGameType + " ";
                }
                LOG.Error("More than one GameType detected: " + GameTypeListString + " Please manually set GameType!");
            }
        
        }

        LOG.Info(L"Start to extract using game type: " + MatchedGameType);
        //检测GameType并调用对应的
        if (MatchedGameType == L"SnB_Body") {
            ExtractFromBuffer_VS_UE4(DrawIB,MatchedGameType);
        }
        else if (MatchedGameType == L"SnB_BodyType2") {
            ExtractFromBuffer_VS_UE4(DrawIB, MatchedGameType);
        }
        else if (MatchedGameType == L"SnB_BodyType3") {
            ExtractFromBuffer_VS_UE4(DrawIB, MatchedGameType);
        }
        else {
            LOG.Error(L"Unknown GameType:" + MatchedGameType);
        }

    }

};