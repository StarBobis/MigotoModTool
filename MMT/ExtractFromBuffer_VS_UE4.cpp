#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtData.h"
#include "GlobalFunctions.h"
#include "ConstantBufferBufFile.h"
#include <set>
#include "FrameAnalysisData.h"
#include "ExtractUtil.h"
#include "VertexBufferBufFile.h"
#include "IndexBufferBufFile.h"


void ExtractFromBuffer_VS_UE4(std::wstring DrawIB,std::wstring GameType) {
    ExtractConfig extractConfig = G.DrawIB_ExtractConfig_Map[DrawIB];

    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[MMTString_ToByteString(GameType)];
    LOG.Info("GameType: " + d3d11GameType.GameType);

    std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(OutputDrawIBFolder);
    LOG.Info(L"Create output folder for DrawIB: " + DrawIB + L"  Path:" + OutputDrawIBFolder);

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
    std::wstring TexcoordExtractSlot = L"-"+ MMTString_ToWideString(d3d11GameType.CategorySlotMap["Texcoord"]) + L"=";
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

    std::unordered_map<int, std::vector<std::byte>> PositionBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + PositionExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> NormalBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + NormalExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> TexcoordBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + TexcoordExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> ColorBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + ColorExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> BlendBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + BlendExtractFileName, MatchNumber );
    
    std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
    //判断Texcoord槽位长度是否正确
    int RealTexcoordStride = TexcoordBufMap[0].size();
    int TexcoordPresetStride = CategoryStrideMap["Texcoord"];
    LOG.Info("RealTexcoordStride:" + std::to_string(RealTexcoordStride));
    LOG.Info("TexcoordPresetStride:" + std::to_string(TexcoordPresetStride));
    if (RealTexcoordStride != TexcoordPresetStride) {
        LOG.Error("Current Processing Texcoord stride: " + std::to_string(RealTexcoordStride) + " can't match with GameType's Texcoord stride: " + std::to_string(TexcoordPresetStride) + " \nPlease try use other GameType to extract.");
    }

    LOG.NewLine();
    //判断Blend槽位长度是否正确
    int RealBlendStride = BlendBufMap[0].size();
    int BlendPresetStride = CategoryStrideMap["Blend"];
    LOG.Info("RealBlendStride:" + std::to_string(RealBlendStride));
    LOG.Info("BlendPresetStride:" + std::to_string(BlendPresetStride));
    if (RealBlendStride != BlendPresetStride && !d3d11GameType.UE4PatchNullInBlend) {
        LOG.Error("Current Processing Blend stride: " + std::to_string(RealBlendStride) + " can't match with GameType's Blend stride: " + std::to_string(BlendPresetStride) + " \nPlease try use other GameType to extract.");
    }
    LOG.NewLine();
    //移除Blend中的位于BLENDINDICES和BLENDWEIGHTS之后的00空白
    std::unordered_map<int, std::vector<std::byte>> BlendBufMap_Clean;
    for (const auto& pair: BlendBufMap) {
        std::vector<std::byte> BlendTmpBuf = pair.second;
        std::vector<std::byte> BlendTmpBufNew;

        for (int i = 0; i < BlendTmpBuf.size(); i++) {
            if (i < 4) {
                BlendTmpBufNew.push_back(BlendTmpBuf[i]);
            }
            else if(i > 7 && i < 12){
                BlendTmpBufNew.push_back(BlendTmpBuf[i]);
            }
        }
        BlendBufMap_Clean[pair.first] = BlendTmpBufNew;
    }


    std::wstring CategoryHash_Position = PositionExtractFileName.substr(11, 8);
    std::wstring CategoryHash_Normal = NormalExtractFileName.substr(11, 8);
    std::wstring CategoryHash_Texcoord = TexcoordExtractFileName.substr(11, 8);
    std::wstring CategoryHash_Color = ColorExtractFileName.substr(11, 8);
    std::wstring CategoryHash_Blend = BlendExtractFileName.substr(11, 8);

    extractConfig.CategoryHashMap["Position"] = MMTString_ToByteString(CategoryHash_Position);
    extractConfig.CategoryHashMap["Normal"] = MMTString_ToByteString(CategoryHash_Normal);
    extractConfig.CategoryHashMap["Texcoord"] = MMTString_ToByteString(CategoryHash_Texcoord);
    extractConfig.CategoryHashMap["Color"] = MMTString_ToByteString(CategoryHash_Color);
    extractConfig.CategoryHashMap["Blend"] = MMTString_ToByteString(CategoryHash_Blend);

    LOG.Info(L"Position Category Hash: " + CategoryHash_Position);
    LOG.Info(L"Normal Category Hash: " + CategoryHash_Normal);
    LOG.Info(L"Texcoord Category Hash: " + CategoryHash_Texcoord);
    LOG.Info(L"Color Category Hash: " + CategoryHash_Color);
    LOG.Info(L"Blend Category Hash: " + CategoryHash_Blend);
    LOG.NewLine();

    std::vector<std::byte> finalVB0Buf;
    for (int i = 0; i < MatchNumber; i++) {
        finalVB0Buf.insert(finalVB0Buf.end(), PositionBufMap[i].begin(), PositionBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), NormalBufMap[i].begin(), NormalBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), TexcoordBufMap[i].begin(), TexcoordBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), ColorBufMap[i].begin(), ColorBufMap[i].end());

        //如果Blend槽位提取的长度为16，则这里要用清洗过之后的Blend值
        if (RealBlendStride == 16 && d3d11GameType.UE4PatchNullInBlend) {
            finalVB0Buf.insert(finalVB0Buf.end(), BlendBufMap_Clean[i].begin(), BlendBufMap_Clean[i].end());
        }
        else {
            finalVB0Buf.insert(finalVB0Buf.end(), BlendBufMap[i].begin(), BlendBufMap[i].end());
        }
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

    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        std::wstring IBFileName = pair.second;
        std::wstring Index = IBFileName.substr(0, 6);

        std::wstring IBReadBufferFileName = IBFileName.substr(0, IBFileName.length() - 4) + L".buf";
        std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;

        std::wstring IBReadFilePath = G.WorkFolder + IBFileName;
        IndexBufferTxtFile ibFileData(IBReadFilePath, true);

        MatchFirstIndexList.push_back(MMTString_ToByteString(ibFileData.FirstIndex));
        PartNameList.push_back(std::to_string(outputCount));

        LOG.Info(IBFileName);
        LOG.Info(L"MatcheFirstIndex: " + ibFileData.FirstIndex + L"  PartName:" + std::to_wstring(outputCount));
        LOG.Info("MinNumber: " + std::to_string(ibFileData.MinNumber) + "\t\tMaxNumber:" + std::to_string(ibFileData.MaxNumber));


        //分别输出fmt,ib,vb
        std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
        std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
        std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

        //输出FMT文件
        fmtFileData.OutputFmtFile(OutputFmtFilePath);
        //输出IB文件
        IndexBufferBufFile ibBufFile(IBReadBufferFilePath, ibFileData.Format);
        ibBufFile.SelfDivide(std::stoi(ibFileData.FirstIndex), std::stoi(ibFileData.IndexCount));
        ibBufFile.SaveToFile_UINT32(OutputIBBufFilePath, -1 * ibBufFile.MinNumber);

        //输出VB文件
        VertexBufferBufFile vbBufFile;
        vbBufFile.FinalVB0Buf = finalVB0Buf;
        vbBufFile.SelfDivide(ibBufFile.MinNumber, ibBufFile.MaxNumber, fmtFileData.Stride);
        vbBufFile.SaveToFile(OutputVBBufFilePath);

        outputCount++;
    }


    //输出Tmp.json
    extractConfig.MatchFirstIndexList = MatchFirstIndexList;
    extractConfig.PartNameList = PartNameList;
    extractConfig.TmpElementList = d3d11GameType.OrderedFullElementList;
    extractConfig.WorkGameType = MMTString_ToByteString(GameType);

    extractConfig.saveTmpConfigs(OutputDrawIBFolder);

    ExtractUtil_MoveAllTextures(G.WorkFolder, DrawIB, OutputDrawIBFolder);
}