#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>


//移动文件到回收站而不是直接删除，防止数据丢失
//实际测试发现有BUG，有的时候无法移动文件到回收站，所以不推荐使用
bool MMTFile_MoveFileToRecycleBin_Deprecated(const std::wstring& filePath);

std::vector<std::wstring> MMTFile_FindFileNameListWithCondition(std::wstring SearchFolderPath, std::wstring searchStr, std::wstring endStr);


std::vector<std::wstring> MMTFile_ReadIniFileLineList(std::wstring filePath);

std::vector<std::wstring> MMTFile_GetFilePathListRecursive(std::wstring directory);

//读取文件的所有行，一般用于ini读取或分析
std::vector<std::wstring> MMTFile_ReadAllLinesW(std::wstring filePath);

//删除指定目录下指定后缀名文件，用于清理上一次生成的Mod文件。
void MMTFile_DeleteFilesWithSuffix(std::wstring directory, std::wstring suffix);

std::unordered_map<int, std::vector<std::byte>> MMTFile_ReadBufMapFromFile(std::wstring readPathW, int vertexNumber);

//读取文件大小
int MMTFile_GetFileSize(std::wstring FileName);

//读取特殊的结尾被填充特别多0的文件的真实的不为0部分的文件大小
uint64_t MMTFile_GetRealFileSize_NullTerminated(std::wstring FileName);

std::wstring MMTFile_FindMaxPrefixedDirectory(const std::wstring& directoryPath, const std::wstring& prefix);

//在文件中查找指定3Dmigoto的ini类型属性，比如查找format,需提供绝对路径，比直接分析整个文件的其它方法更快
std::wstring MMTFile_FindMigotoIniAttributeInFile(const std::wstring& filePath, const std::wstring& attributeName);

bool MMTFile_IsValidFilename(std::string filename);

