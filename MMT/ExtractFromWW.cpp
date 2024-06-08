#include "GlobalConfigs.h"
#include "GlobalFunctions.h"

void ExtractFromWW() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("I suggest this test Setting in your d3dx.ini for dump WW:\nanalyse_options = dump_rt dump_tex dump_cb dump_vb dump_ib buf txt");
        LOG.NewLine();
        LOG.Info("Extract Buffer from Weathering Wave:");

        if (extractConfig.GameType == L"Auto") {
            LOG.Error(L"Sorry, This game doesn't support Auto type detect yet" );
        }
        else if (extractConfig.GameType == L"WW_Body") {
            ExtractFromBuffer_CS_WW_Body(DrawIB);
        }
        else if (extractConfig.GameType == L"WW_Object") {
            ExtractFromBuffer_VS_WW_Object(DrawIB);
        }
        //TODO 优化集成WW_Object和WW_objectType2到一个方法中，不然冗余太多了
        else if (extractConfig.GameType == L"WW_ObjectType2") {
            ExtractFromBuffer_VS_WW_ObjectType2(DrawIB);
        }
        else {
            LOG.Error(L"Unknown GameType!");
        }

    }
}