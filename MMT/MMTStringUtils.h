#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

// 根据文件完整路径获取文件所在目录路径,结尾不会自动补充/
std::wstring MMTString_GetFolderPathFromFilePath(std::wstring filePath);

//string转换为wstring
std::wstring MMTString_ToWideString(std::string input);

//wstring转换为string
std::string MMTString_ToByteString(std::wstring input);

//生成UUID
std::wstring MMTString_GenerateUUIDW();

//生成日期字符串
std::wstring MMTString_GetFormattedDateTimeForFilename();

//纯C++实现分割字符串，因为Boost库无法正常以L"=="作为分隔符
//原始字符串，用于分割的字符串
std::vector<std::wstring> MMTString_SplitString(std::wstring originalStr,std::wstring delimiter);

//通过文件路径获取文件名
std::wstring MMTString_GetFileNameFromFilePath(std::wstring filePath);
std::wstring MMTString_ToLowerCase(const std::wstring str);
std::wstring MMTString_GetPSHashFromFileName(const std::wstring& input);
std::wstring MMTString_GetVSHashFromFileName(const std::wstring& input);
std::wstring MMTString_RemoveSquareBrackets(std::wstring inputStr);