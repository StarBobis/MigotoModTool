#pragma once
#include "MigotoFormat.h"
#include <unordered_map>


std::vector<std::unordered_map<std::wstring, std::wstring>> MigotoParseUtil_Get_M_Key_Combination(std::vector<M_Key> cycleKeyList);


std::vector<M_DrawIndexed> MigotoParseUtil_GetActiveDrawIndexedListByKeyCombination(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap, std::vector<M_DrawIndexed> DrawIndexedList);


void MigotoParseUtil_OutputIBFileByDrawIndexedList(
    std::wstring OriginalIBPath, std::wstring TargetIBPath,
    std::wstring IBReadFormat, std::vector<M_DrawIndexed> ActiveDrawIndexedList,
    int MinNumber);



std::wstring MigotoParseUtil_Get_M_Key_Combination_String(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap);

std::vector<std::wstring> MigotoParseUtil_GetRecursiveActivedIniFilePathList(std::wstring IncludePath);

std::vector<M_SectionLine> MigotoParseUtil_ParseMigotoSectionLineList(std::wstring iniFilePath);