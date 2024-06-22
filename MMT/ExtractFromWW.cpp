#include "GlobalConfigs.h"
#include "GlobalFunctions.h"
#include "IndexBufferTxtFile.h"
#include "ConstantBufferBufFile.h"
#include "FrameAnalysisData.h"

void ExtractFromWW() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("I suggest this test Setting in your d3dx.ini for dump WW:\nanalyse_options = dump_rt dump_tex dump_cb dump_vb dump_ib buf txt");
        LOG.NewLine();
        LOG.Info("Extract Buffer from Weathering Wave:");

        std::wstring MatchedGameType = extractConfig.GameType;
        if (extractConfig.GameType == L"Auto") {
            LOG.Info(L"Try auto detect game type:" );
            std::vector<std::wstring> MatchGameTypeList;
            for (const auto& pair: G.GameTypeName_D3d11GameType_Map) {
                D3D11GameType d3d11GameType = pair.second;
                FrameAnalysisData FAData(G.WorkFolder);
                std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
                std::wstring VSExtractIndex;
                // 从vs-t0中读取到顶点数量
                // 然后在下面的过程中限制顶点数量匹配到我们的DrawNumber来设置对应的索引才对，而不是最大的索引。
                std::wstring TexcoordExtractFileName = L"";
                int MatchNumber = 0;
                LOG.Info(L"Start to check for every file related with your DrawIB: " + DrawIB);
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".txt")) {
                        continue;
                    }
                    if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
                        continue;
                    }
                    IndexBufferTxtFile ibFileData(G.WorkFolder + filename, false);

                    VSExtractIndex = ibFileData.Index;

                    TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vs-t0=", L".buf")[0];
                    int TexcoordFileSize = MMTFile_GetFileSize(G.WorkFolder + TexcoordExtractFileName);
                    MatchNumber = TexcoordFileSize / 16;
                    LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

                    LOG.Info(filename);
                    if (ibFileData.FirstIndex == L"") {
                        LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
                        continue;
                    }
                    int matchFirstIndex = std::stoi(ibFileData.FirstIndex);

                }
                if (MatchNumber == 0) {
                    LOG.Error(L"Can't find any related ib file with your DrawIB : " + DrawIB + L" Please check:\n1.Did you dump a FrameAnalysis folder for extract this IB hash.\n2.Did you choose the correct IB value,there may be multiple IB control render,only one of them is real.");
                }
                LOG.NewLine();


                LOG.NewLine();
                int MatchedDrawNumber = 0;
                std::wstring MatchedDrawNumberCSIndex = L"";
                std::string DrawComputeShader = "";
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".buf")) {
                        continue;
                    }
                    if (filename.find(L"-cs=") == std::wstring::npos) {
                        continue;
                    }
                    //暂时只需要cs-cb0就够了
                    if (filename.find(L"-cs-cb0=") == std::wstring::npos) {
                        continue;
                    }
                    //需要过滤掉不是我们ComputeShader的文件名
                    if (filename.find(L"1ff924db9d4048d1") == std::wstring::npos && filename.find(L"4d0760c2c7406824") == std::wstring::npos) {
                        continue;
                    }

                    LOG.Info(filename);
                    std::wstring filepath = G.WorkFolder + filename;
                    //LOG.Info(filepath);

                    ConstantBufferBufFile cbFileData(filepath);
                    LOG.Info("CB[0].W CS Calculate Time: " + std::to_string(cbFileData.lineCBValueMap[0].W));
                    LOG.Info("CB[0].Y Draw Number1: " + std::to_string(cbFileData.lineCBValueMap[0].Y));
                    LOG.Info("CB[0].Z Draw Number2: " + std::to_string(cbFileData.lineCBValueMap[0].Z));
                    LOG.Info("CB[1].X Draw Number3: " + std::to_string(cbFileData.lineCBValueMap[1].X));

                    //这里如果cb0.w和 cb0.y相同，则为cb0.w加上cb1.x
                    //如果不同则为cb0.w + cb0.x
                    int drawNumber = 0;
                    if (cbFileData.lineCBValueMap[0].W == cbFileData.lineCBValueMap[0].Y) {
                        drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[1].X;
                        LOG.Info("CB[0].W == CB[0].Y   DrawNumber = CB[0].W + CB[1].X = " + std::to_string(drawNumber));
                    }
                    else {
                        drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[0].Y;
                        LOG.Info("CB[0].W != CB[0].Y   DrawNumber = CB[0].W + CB[0].Y = " + std::to_string(drawNumber));
                    }
                    LOG.NewLine();

                    //LOG.Info("Draw Number: " + std::to_string(drawNumber));

                    //这里只有drawNumber等于我们从vs-t0中找到的DrawNumber时，才进行替换
                    
                        if (MatchNumber == drawNumber) {
                            //检测并设置DrawComputeShader，用于判断具体要从哪个槽位提取
                            if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
                                DrawComputeShader = "1ff924db9d4048d1";

                            }
                            else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
                                DrawComputeShader = "4d0760c2c7406824";
                            }
                            LOG.Info("Find Match Number File! Set draw compute shader to : " + DrawComputeShader);
                            MatchedDrawNumberCSIndex = filename.substr(0, 6);
                        }

                }
                LOG.Info(L"MatchedDrawNumber: " + std::to_wstring(MatchedDrawNumber));
                LOG.Info(L"MatchedDrawNumberCSIndex: " + MatchedDrawNumberCSIndex);

                //如果能找到MaxDrawNumberIndex，说明使用CS进行计算，说明当前类型肯定是WW_Body
                if (MatchedDrawNumberCSIndex != L"") {
                    MatchedGameType = MMTString_ToWideString("WW_Body");
                    MatchGameTypeList.push_back(MMTString_ToWideString("WW_Body"));
                    break;
                }

                //如果找不到对应的数值，说明不使用CS进行计算，说明为Object类型，接下来进行object类型匹配
                //对每个槽位的宽度进行检查，不通过则continue，全部通过则放入列表
                
                LOG.Info("Start to match object types:");
                // 对于object类型应该从vb0中读取顶点数量
                // 限制顶点数量匹配到我们的DrawNumber来设置对应的索引
                std::wstring PositionExtractFileName = L"";
                MatchNumber = 0;
                std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
                int PositionStride = CategoryStrideMap["Position"];
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
                    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                    int PositionFileSize = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName);
                    MatchNumber = PositionFileSize / PositionStride;
                    LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

                    LOG.Info(filename);
                    if (ibFileData.FirstIndex == L"") {
                        LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
                        continue;
                    }


                    //注意:鸣潮中出现了部分物体类型多次Draw时，有些Draw不使用贴图槽位且顶点数量也无法对上，Hash值也不同的情况
                    //所以我们提取物体Mod类型时，必须要确保ps-t0槽位的贴图确实存在
                    //如果不存在则说明不是真正渲染贴图的那个槽位。
                    std::vector<std::wstring> Pst0_TextureDDSFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t1=", L".dds");
                    std::vector<std::wstring> Pst0_TextureJPGFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t1=", L".jpg");
                    if (Pst0_TextureDDSFileList.size() == 0 && Pst0_TextureJPGFileList.size() == 0) {
                        LOG.Warning(L"Can't find ps-t1 or jpg texture for index:" + ibFileData.Index + L" it will not be a valid object type, so skip this.");
                        continue;
                    }

                    //TODO 这里不管是用ps-t0还是ps-t1都无法做到通用兼容，并不是最优解，后面有空可以优化一下
                }
               
                LOG.NewLine();



                std::wstring TexcoordExtractSlot = L"-vs-t0=";
                std::wstring NormalExtractSlot = L"-vs-t1=";
                LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
                LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
                LOG.NewLine();
                //收集各个槽位的内容，并组合成VB0的内容
                TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
                std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];

                //这里的变量名放到上面初始化了
                LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
                LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);

                int NormalStride = MMTFile_GetFileSize(G.WorkFolder + NormalExtractFileName) / MatchNumber;
                int TexcoordStride = MMTFile_GetFileSize(G.WorkFolder + TexcoordExtractFileName) / MatchNumber;

                if (NormalStride != CategoryStrideMap["Normal"]) {
                    continue;
                }
                if (TexcoordStride != CategoryStrideMap["Texcoord"]) {
                    continue;
                }

                //如果上面这个步长都能对上，则这里直接加入即可
                MatchedGameType = MMTString_ToWideString(pair.first);
                MatchGameTypeList.push_back(MMTString_ToWideString(pair.first));

            }

            //然后根据大小做出对应选择
            if (MatchGameTypeList.size() == 1) {
                LOG.NewLine();
                LOG.Info(L"GameType Matched: " + MatchedGameType);
                LOG.NewLine();

                if (MatchedGameType == L"WW_Body") {
                    ExtractFromBuffer_CS_WW_Body(DrawIB, L"WW_Body");
                }
                else {
                    ExtractFromBuffer_VS_WW_Object(DrawIB, MatchedGameType);

                }
            }
            else if (MatchGameTypeList.size() > 1) {
                std::wstring GameTypeListStr = L"";
                for (std::wstring GameType: MatchGameTypeList) {
                    GameTypeListStr = GameTypeListStr + GameType + L" ";
                }
                //列出所有可能的类型
                LOG.Error(L"More than one GameType Matched: " + GameTypeListStr);
            }
            else {
                LOG.Error("Unknown GameType! Please contanct NicoMico to add support for this type!");
            }

        }
        else {
            if (MatchedGameType == L"WW_Body") {
                ExtractFromBuffer_CS_WW_Body(DrawIB, L"WW_Body");
            }
            else {
                ExtractFromBuffer_VS_WW_Object(DrawIB, MatchedGameType);

            }
        }



    }
}