#pragma once
#include "GlobalConfigs.h"

//COLOR重计算，使用基于算数平均的AverageNormal重计算方法
std::unordered_map<std::wstring, std::vector<std::byte>> RecalculateColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideConfig);

//COLOR值直接赋予
std::unordered_map<std::wstring, std::vector<std::byte>> ResetColor(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig, std::unordered_map<std::string, int> categoryStrideConfig);

//TANGENT使用基于Vector的AverageNormal归一化重计算方法
std::unordered_map<std::wstring, std::vector<std::byte>> TANGENT_averageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> inputVBCategoryDataMap, D3D11GameType d3d11GameType);

//翻转NORMAL和TANGENT的值
std::unordered_map<std::wstring, std::vector<std::byte>> ReverseNormalTangentValues(std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap, ExtractConfig basicConfig);
