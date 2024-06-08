#include <set>

#include "D3d11GameType.h"
#include "IndexBufferTxtFile.h"
#include "VertexBufferBufFile.h"
#include "VertexBufferTxtFile.h"
#include "GlobalConfigs.h"
#include "GlobalFunctions.h"
#include "FrameAnalysisData.h"

void ExtractFromBuffer_SnB() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("Extract Buffer from SnowBreak:");
        LOG.NewLine();

        if (extractConfig.GameType == L"Auto") {
            LOG.Error(L"Sorry, This game doesn't support Auto type detect yet");
        }
        else if (extractConfig.GameType == L"SnB_Body") {
            ExtractFromBuffer_VS_SnB_SnB_Body(DrawIB);
        }
        
        else {
            LOG.Error(L"Unknown GameType!");
        }

    }

};