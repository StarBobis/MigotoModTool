#pragma once
#include "GlobalConfigs.h"
//这个类里只允许放程序入口调用的方法，不要轻易增删方法，每次修改这里都会影响到所有的cpp文件，修改后的编译时间长达1分钟

//鸣潮
void ExtractFromWW();
void ExtractFromBuffer_CS_WW_Body(std::wstring DrawIB, std::wstring GameType);
void ExtractFromBuffer_VS_WW_Object(std::wstring DrawIB, std::wstring GameType);
void Generate_CS_WW_Body();

//尘白禁区 SnowBreak
void ExtractFromBuffer_VS_UE4(std::wstring DrawIB, std::wstring GameType);
void Extract_VS_UE4();
void Generate_VS_UE4();

//原神,崩坏:星穹铁道,崩坏三,绝区零
void ExtractFromBuffer_VS();
void UnityGenerate();

//原神,崩坏:星穹铁道,崩坏三,绝区零		Dump式逆向提取Mod模型
void ExtractFromBuffer_VS_Reverse();

//原神,崩坏:星穹铁道,崩坏三,绝区零(Unity系列游戏)		脚本式逆向提取Mod模型
void Reverse_Lv4_Merged_OutfitCompiler();
void Reverse_Lv4_Single();

//TODO Deprecated Reverse Method
void ReverseMerged();


