#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtData.h"
#include "GlobalFunctions.h"
#include "ConstantBufferBufFile.h"
#include <set>
#include "FrameAnalysisData.h"
#include "ExtractUtil.h"
#include "WWUtil.h"
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"


void ExtractFromBuffer_CS_WW_Body(std::wstring DrawIB,std::wstring GameType) {
    ExtractConfig extractConfig = G.DrawIB_ExtractConfig_Map[DrawIB];
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[MMTString_ToByteString(GameType)];
    LOG.Info("GameType: " + d3d11GameType.GameType);
    std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(OutputDrawIBFolder);
    FrameAnalysisData FAData(G.WorkFolder);
    std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
    std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
    std::wstring VSExtractIndex;
    // 从vs-t0中读取到顶点数量
    // 然后在下面的过程中限制顶点数量匹配到我们的DrawNumber来设置对应的索引才对，而不是最大的索引。
    std::wstring TexcoordExtractFileName = L"";
    int MatchNumber = 0;
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

        matchFirstIndexIBFileNameMap[matchFirstIndex] = filename;
    }
    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        LOG.Info("Match First Index: " + std::to_string(pair.first));
    }
    LOG.NewLine();


    LOG.NewLine();
    int MatchedDrawNumber = 0;
    std::wstring MatchedDrawNumberCSIndex = L"";
    std::string DrawComputeShader = "";
    //CalculateTime , (Offset,CalculateTime,CS)
    std::unordered_map<int, WuwaCSInfo> vertexCountWuwaCSInfoMap;
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
        LOG.Info("CB[0].Y Offset 1: " + std::to_string(cbFileData.lineCBValueMap[0].Y));
        LOG.Info("CB[0].Z Offset 2: " + std::to_string(cbFileData.lineCBValueMap[0].Z));
        LOG.Info("CB[1].X Calculate Time: " + std::to_string(cbFileData.lineCBValueMap[1].X));

        //这里如果cb0.w和 cb0.y相同，则为cb0.w加上cb1.x
        //如果不同则为cb0.w + cb0.x
        WuwaCSInfo wwcsinfo;

        int drawNumber = 0;
        if (cbFileData.lineCBValueMap[0].W == cbFileData.lineCBValueMap[0].Y) {
            drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[1].X;
            wwcsinfo.CalculateTime = cbFileData.lineCBValueMap[1].X;
            wwcsinfo.Offset = cbFileData.lineCBValueMap[0].W;
            LOG.Info("CB[0].W == CB[0].Y   DrawNumber = CB[0].W + CB[1].X = " + std::to_string(drawNumber));
        }
        else {
            drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[0].Y;
            wwcsinfo.CalculateTime = cbFileData.lineCBValueMap[0].W;
            wwcsinfo.Offset = cbFileData.lineCBValueMap[0].Y;
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


        if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
            wwcsinfo.ComputeShaderHash = "1ff924db9d4048d1";
        }
        else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
            wwcsinfo.ComputeShaderHash = "4d0760c2c7406824";

        }
        vertexCountWuwaCSInfoMap[wwcsinfo.CalculateTime] = wwcsinfo;
    }
    LOG.Info(L"MatchedDrawNumber: " + std::to_wstring(MatchedDrawNumber));
    LOG.Info(L"MatchedDrawNumberCSIndex: " + MatchedDrawNumberCSIndex);
    LOG.NewLine();
    for (const auto& pair: vertexCountWuwaCSInfoMap) {
        WuwaCSInfo csInfo = pair.second;
        LOG.Info("VertexCount: " + std::to_string(csInfo.CalculateTime) + "  Offset:" + std::to_string(csInfo.Offset) + "  ComputeShaderHash:" + csInfo.ComputeShaderHash);
    }
    LOG.NewLine();
    
    /*
        1.当DrawComputeShader为1ff924db9d4048d1时CS各槽位内容如下：
            cs-t3 stride=8    33145   只能是长度各为4的BLENDWEIGHT和BLENDINDICES
            cs-t4 stride=8    33145   NORMAL,COLOR
            cs-t5 stride=12   33145   POSITION
        2.当DrawComputeShader为4d0760c2c7406824时CS各槽位内容如下：
            cs-t4 stride=8    33145   只能是长度各为4的BLENDWEIGHT和BLENDINDICES
            cs-t5 stride=8    33145   NORMAL,COLOR
            cs-t6 stride=12   33145   POSITION
    */
    std::wstring PositionExtractSlot = L"-cs-t5=";
    std::wstring NormalExtractSlot = L"-cs-t4=";
    std::wstring BlendExtractSlot = L"-cs-t3=";
    if (DrawComputeShader == "1ff924db9d4048d1") {
        PositionExtractSlot = L"-cs-t5=";
        NormalExtractSlot = L"-cs-t4=";
        BlendExtractSlot = L"-cs-t3=";
    }
    else if (DrawComputeShader == "4d0760c2c7406824") {
        PositionExtractSlot = L"-cs-t6=";
        NormalExtractSlot = L"-cs-t5=";
        BlendExtractSlot = L"-cs-t4=";
    }
    else {
        LOG.Info("Can't find DrawComputeShader, use default setting.");
    }
    LOG.Info(L"Position Extract Slot: " + PositionExtractSlot);
    LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
    LOG.Info(L"Blend Extract Slot: " + BlendExtractSlot);
    LOG.NewLine();
    //收集各个槽位的内容，并组合成VB0的内容
    std::wstring PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + PositionExtractSlot, L".buf")[0];
    std::wstring NormalColorExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + NormalExtractSlot, L".buf")[0];
    std::wstring BlendExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + BlendExtractSlot, L".buf")[0];
    std::wstring CS_CB0_FileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + L"-cs-cb0=", L".buf")[0];

    //这里的变量名放到上面初始化了
    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
    LOG.Info(L"NormalExtractFileName: " + NormalColorExtractFileName);
    LOG.Info(L"BlendExtractFileName: " + BlendExtractFileName);
    LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
    LOG.Info(L"CS_CB0_FileName: " + CS_CB0_FileName);

    std::unordered_map<int, std::vector<std::byte>> PositionBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + PositionExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> NormalTangentBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + NormalColorExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> BlendBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + BlendExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> TexcoordBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + TexcoordExtractFileName, MatchNumber);

    std::wstring CategoryHash_Position = PositionExtractFileName.substr(13, 8);
    std::wstring CategoryHash_NormalColor = NormalColorExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Blend = BlendExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Texcoord = TexcoordExtractFileName.substr(13, 8);
    std::wstring CS_CB0_Hash = CS_CB0_FileName.substr(14, 8);

    extractConfig.CategoryHashMap["Position"] = MMTString_ToByteString(CategoryHash_Position);
    extractConfig.CategoryHashMap["Normal"] = MMTString_ToByteString(CategoryHash_NormalColor);
    extractConfig.CategoryHashMap["Blend"] = MMTString_ToByteString(CategoryHash_Blend);
    extractConfig.CategoryHashMap["Texcoord"] = MMTString_ToByteString(CategoryHash_Texcoord);
    extractConfig.VertexLimitVB = MMTString_ToByteString(CS_CB0_Hash);

    std::vector<std::byte> finalVB0Buf;
    for (int i = 0; i < MatchNumber; i++) {
        finalVB0Buf.insert(finalVB0Buf.end(), PositionBufMap[i].begin(), PositionBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), NormalTangentBufMap[i].begin(), NormalTangentBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), BlendBufMap[i].begin(), BlendBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), TexcoordBufMap[i].begin(), TexcoordBufMap[i].end());
    }

    FmtFileData fmtFileData;
    fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
    fmtFileData.d3d11GameType = d3d11GameType;
    fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
    fmtFileData.Stride = d3d11GameType.getElementListStride(fmtFileData.ElementNameList);
    LOG.NewLine();

    LOG.Info("Start to go through every IB file:");
    //遍历每一个IB，输出
    int outputCount = 1;
    std::vector<std::string> MatchFirstIndexList;
    std::vector<std::string> PartNameList;

    WuwaCSInfoJsonObject wuwaCSInfoJsonObject;
    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        std::wstring IBFileName = pair.second;
        std::wstring Index = IBFileName.substr(0, 6);

        std::wstring IBReadBufferFileName = IBFileName.substr(0, IBFileName.length() - 4) + L".buf";
        std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;

        std::wstring IBReadTxtFilePath = G.WorkFolder + IBFileName;
        IndexBufferTxtFile ibFileData(IBReadTxtFilePath, true);

        MatchFirstIndexList.push_back(MMTString_ToByteString(ibFileData.FirstIndex));
        PartNameList.push_back(std::to_string(outputCount));

        LOG.Info(IBFileName);
        LOG.Info(L"MatcheFirstIndex: " + ibFileData.FirstIndex + L"  PartName:" + std::to_wstring(outputCount));
        LOG.Info("MinNumber: " + std::to_string(ibFileData.MinNumber) + "\t\tMaxNumber:" + std::to_string(ibFileData.MaxNumber));

        //分别输出fmt,ib,vb
        std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
        std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
        std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

        //输出FMT文件，这里强制设为R32_UINT
        fmtFileData.OutputFmtFile(OutputFmtFilePath);

        //输出IB文件
        IndexBufferBufFile ibBufFile(IBReadBufferFilePath, ibFileData.Format);
        ibBufFile.SelfDivide(std::stoi(ibFileData.FirstIndex), std::stoi(ibFileData.IndexCount));
        //因为我们VB文件截取出来了，所以这里的索引从0开始计算，所以要每个数都减去本身中的最小值
        ibBufFile.SaveToFile_UINT32(OutputIBBufFilePath, -1 * ibBufFile.MinNumber);
        LOG.Info("Subdivided Unique Vertex Count: " + std::to_string(ibBufFile.UniqueVertexCount));
        wuwaCSInfoJsonObject.PartNameWuwaCSInfoMap[std::to_string(outputCount)] = vertexCountWuwaCSInfoMap[ibBufFile.UniqueVertexCount];

        //输出VB文件
        VertexBufferBufFile vbBufFile;
        vbBufFile.FinalVB0Buf = finalVB0Buf;
        LOG.Info("VBFile SelfDivide: minNumber:" + std::to_string(ibBufFile.MinNumber) + "  maxNumber:" + std::to_string(ibBufFile.MaxNumber) + " stride: " + std::to_string(fmtFileData.Stride));
        LOG.Info("Size: " + std::to_string(vbBufFile.FinalVB0Buf.size()));
        vbBufFile.SelfDivide(ibBufFile.MinNumber, ibBufFile.MaxNumber, fmtFileData.Stride);
        vbBufFile.SaveToFile(OutputVBBufFilePath);

        outputCount++;

    }

    //写出鸣潮的顶点数量
    wuwaCSInfoJsonObject.saveToJsonFile(OutputDrawIBFolder);

    //输出Tmp.json
    extractConfig.MatchFirstIndexList = MatchFirstIndexList;
    extractConfig.PartNameList = PartNameList;
    extractConfig.TmpElementList = d3d11GameType.OrderedFullElementList;
    extractConfig.WorkGameType = MMTString_ToByteString(GameType);

    extractConfig.saveTmpConfigs(OutputDrawIBFolder);

    ExtractUtil_MoveAllTextures(G.WorkFolder, DrawIB, OutputDrawIBFolder);
}