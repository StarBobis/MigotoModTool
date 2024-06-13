#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtData.h"
#include "GlobalFunctions.h"
#include "ConstantBufferBufFile.h"
#include <set>
#include "FrameAnalysisData.h"
#include "ExtractUtil.h"


void ExtractFromBuffer_VS_WW_Object(std::wstring DrawIB, std::wstring GameType) {
    ExtractConfig extractConfig = G.DrawIB_ExtractConfig_Map[DrawIB];
    D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[MMTString_ToByteString(GameType)];
    LOG.Info("GameType: " + d3d11GameType.GameType);
    std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(OutputDrawIBFolder);
    FrameAnalysisData FAData(G.WorkFolder);
    std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
    
    // 对于object类型应该从vb0中读取顶点数量
    // 限制顶点数量匹配到我们的DrawNumber来设置对应的索引
    std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
    std::wstring VSExtractIndex;
    std::wstring PositionExtractFileName = L"";
    int MatchNumber = 0;
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
        //注意:鸣潮中出现了部分物体类型多次Draw时，有些Draw不使用贴图槽位且顶点数量也无法对上，Hash值也不同的情况
        //所以我们提取物体Mod类型时，必须要确保ps-t0槽位的贴图确实存在
        //如果不存在则说明不是真正渲染贴图的那个槽位。
        std::vector<std::wstring> Pst0_TextureDDSFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t0=", L".dds");
        std::vector<std::wstring> Pst0_TextureJPGFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t0=", L".jpg");
        if (Pst0_TextureDDSFileList.size() == 0 && Pst0_TextureJPGFileList.size() == 0) {
            LOG.Info(L"Can't find ps-t0 or jpg texture for index:" + ibFileData.Index + L" it will not be a valid object type, so skip this.");
            continue;
        }

        VSExtractIndex = ibFileData.Index;
        PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vb0=", L".buf")[0];
        int PositionFileSize = MMTFile_GetFileSize(G.WorkFolder + PositionExtractFileName);
        MatchNumber = PositionFileSize / PositionStride;
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


    
    std::wstring TexcoordExtractSlot = L"-vs-t0=";
    std::wstring NormalExtractSlot = L"-vs-t1=";
    LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
    LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
    LOG.NewLine();
    //收集各个槽位的内容，并组合成VB0的内容
    std::wstring TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
    std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];

    //这里的变量名放到上面初始化了
    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
    LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
    LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);

    std::unordered_map<int, std::vector<std::byte>> PositionBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + PositionExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> NormalBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + NormalExtractFileName, MatchNumber);
    std::unordered_map<int, std::vector<std::byte>> TexcoordBufMap = MMTFile_ReadBufMapFromFile(G.WorkFolder + TexcoordExtractFileName, MatchNumber);

    std::wstring CategoryHash_Position = PositionExtractFileName.substr(11, 8);
    std::wstring CategoryHash_Normal = NormalExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Texcoord = TexcoordExtractFileName.substr(13, 8);

    extractConfig.CategoryHashMap["Position"] = MMTString_ToByteString(CategoryHash_Position);
    extractConfig.CategoryHashMap["Normal"] = MMTString_ToByteString(CategoryHash_Normal);
    extractConfig.CategoryHashMap["Texcoord"] = MMTString_ToByteString(CategoryHash_Texcoord);


    std::vector<std::byte> finalVB0Buf;
    for (int i = 0; i < MatchNumber; i++) {
        finalVB0Buf.insert(finalVB0Buf.end(), PositionBufMap[i].begin(), PositionBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), NormalBufMap[i].begin(), NormalBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), TexcoordBufMap[i].begin(), TexcoordBufMap[i].end());
    }

    FmtFileData fmtFileData;
    fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
    fmtFileData.d3d11GameType = d3d11GameType;
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
        std::wstring IBReadFilePath = G.WorkFolder + IBFileName;
        IndexBufferTxtFile ibFileData(IBReadFilePath, true);

        MatchFirstIndexList.push_back(MMTString_ToByteString(ibFileData.FirstIndex));
        PartNameList.push_back(std::to_string(outputCount));

        LOG.Info(IBFileName);
        LOG.Info(L"MatcheFirstIndex: " + ibFileData.FirstIndex + L"  PartName:" + std::to_wstring(outputCount));
        LOG.Info("MinNumber: " + std::to_string(ibFileData.MinNumber) + "\t\tMaxNumber:" + std::to_string(ibFileData.MaxNumber));

        std::wstring Format = ibFileData.Format;
        std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;
        std::unordered_map<int, std::vector<std::byte>> IBFileBuf = MMTFile_ReadIBBufFromFile(IBReadBufferFilePath, Format);
        int FirstIndex = std::stoi(ibFileData.FirstIndex);
        int IndexCount = std::stoi(ibFileData.IndexCount);

        int EndIndex = FirstIndex + IndexCount;
        std::vector<std::byte> finalIBBuf;
        std::vector<std::byte> tmpIBBuf;
        for (int i = FirstIndex; i < EndIndex; i++) {
            tmpIBBuf = IBFileBuf[i];
            finalIBBuf.insert(finalIBBuf.end(), tmpIBBuf.begin(), tmpIBBuf.end());
        }
        fmtFileData.Format = Format;

        //分别输出fmt,ib,vb
        std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
        std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
        std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

        //输出FMT文件
        fmtFileData.OutputFmtFile(OutputFmtFilePath);
        //输出IB文件
        std::ofstream outputIBFile(OutputIBBufFilePath, std::ofstream::binary);
        outputIBFile.write(reinterpret_cast<const char*>(finalIBBuf.data()), finalIBBuf.size());
        outputIBFile.close();
        //输出VB文件
        std::ofstream outputVBFile(OutputVBBufFilePath, std::ofstream::binary);
        outputVBFile.write(reinterpret_cast<const char*>(finalVB0Buf.data()), finalVB0Buf.size());
        outputVBFile.close();

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