#include <Windows.h>
#include <filesystem>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "GlobalConfigs.h"


std::vector<std::wstring> Global_FrameAnalyseFileNameList;

bool MMTFile_MoveFileToRecycleBin_Deprecated(const std::wstring& filePath)
{
    if (!std::filesystem::exists(filePath))
    {
        // 文件不存在
        return false;
    }

    SHFILEOPSTRUCT fileOp = { 0 };
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = filePath.c_str();
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION;

    int result = SHFileOperation(&fileOp);
    if (result != 0)
    {
        // 发生错误
        return false;
    }

    // 文件已发送到回收站
    return true;
}

//查找FrameAnalyse文件夹下满足条件的文件名列表 searchStr, endStr
std::vector<std::wstring> MMTFile_FindFileNameListWithCondition(std::wstring SearchFolderPath, std::wstring searchStr, std::wstring endStr) {
    std::vector<std::wstring> findFileNameList;
    std::vector<std::wstring> FilteredFileNameList;
    //然后直接初始化FrameAnalyse文件列表
    LOG.Info(L"Initialize FrameAnalyse folder file list.");
    for (const auto& entry : std::filesystem::directory_iterator(SearchFolderPath)) {
        if (!(entry.is_regular_file())) {
            continue;
        }
        FilteredFileNameList.push_back(entry.path().filename().wstring());
    }

    for (std::wstring fileName : FilteredFileNameList) {

        if (fileName.find(searchStr) != std::string::npos && fileName.substr(fileName.length() - endStr.length()) == endStr) {
            findFileNameList.push_back(fileName);
        }
    }
    return findFileNameList;
}







std::vector<std::wstring> MMTFile_ReadIniFileLineList(std::wstring filePath)
{
    std::vector<std::wstring> lines;
    std::wifstream inFile(filePath);

    if (inFile.is_open())
    {
        std::wstring line;
        while (std::getline(inFile, line))
        {
            lines.push_back(line);
        }

        inFile.close();
    }

    return lines;
}



std::vector<std::wstring> MMTFile_GetFilePathListRecursive(std::wstring directory)
{
    std::vector<std::wstring> filePathList;

    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        const auto& path = entry.path();
        if (std::filesystem::is_directory(path))
        {
            // 递归调用以获取子文件夹中的文件
            std::vector<std::wstring> subFileNames = MMTFile_GetFilePathListRecursive(path);
            filePathList.insert(filePathList.end(), subFileNames.begin(), subFileNames.end());
        }
        else if (std::filesystem::is_regular_file(path))
        {
            filePathList.push_back(path.wstring());
        }
    }

    return filePathList;
}



std::vector<std::wstring> MMTFile_ReadAllLinesW(std::wstring filePath) {
    std::wifstream file(filePath); // 创建ifstream对象并打开文件
    std::vector<std::wstring> lines; // 创建一个string类型的vector容器来存储文件中的每一行
    //LOG.LogOutput(L"Start to read all lines.");
    if (file.is_open()) { // 检查文件是否成功打开
        std::wstring line;
        while (std::getline(file, line)) { // 读取每一行，直到文件结尾
            lines.push_back(line); // 将读取到的行添加到vector中
        }
        file.close(); // 关闭文件
    }
    else {
        std::cout << "Can't open this file!" << std::endl;
    }
    return lines;
}


//目录，后缀名，删除指定目录下指定后缀名的所有文件
void MMTFile_DeleteFilesWithSuffix(std::wstring directory, std::wstring suffix)
{
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == suffix)
        {
            std::filesystem::remove(entry.path());
            //MoveFileToRecycleBin(entry.path().wstring());
            LOG.Info(L"Delete file:" + entry.path().wstring());
        }
    }
}



std::unordered_map<int, std::vector<std::byte>> MMTFile_ReadBufMapFromFile(std::wstring readPathW, int vertexNumber) {
    //LOG.Info(L"Read from " + readPathW);
    std::unordered_map<int, std::vector<std::byte>> BufMap;

    int stride = 0;
    std::ifstream BufFile(readPathW, std::ifstream::binary);
    BufFile.seekg(0, std::ios::end);
    //LOG.Info(L"Read Size: " + std::to_wstring(BufFile.tellg()));
    stride = int(BufFile.tellg() / vertexNumber);
    BufFile.seekg(0, std::ios::beg);
    //LOG.Info(L"Stride: " + std::to_wstring(stride));

    std::vector<std::byte> Buffer(vertexNumber * stride);
    BufFile.read(reinterpret_cast<char*>(Buffer.data()), vertexNumber * stride);
    BufFile.close();
    for (int i = 0; i < vertexNumber; i++) {
        BufMap[i] = MMTFormat_GetRange_Byte(Buffer, i * stride, i * stride + stride);
    }

    return BufMap;
}

int MMTFile_GetFileSize(std::wstring FileName) {
    std::ifstream BufFile(FileName, std::ifstream::binary);
    BufFile.seekg(0, std::ios::end);
    int fileSize = int(BufFile.tellg());
    BufFile.close();
    return fileSize;
}

int MMTFile_GetRealFileSize_NullTerminated(std::wstring FileName) {
    std::ifstream file(FileName, std::ios::binary);
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    int end = data.size() - 1;
    while (end >= 0 && data[end] == 0) {
        end--;
    }
    int length = end + 1;
    file.close();
    return length;
}


//查找指定directoryPath目录下以prefix开头的目录中最大的一个，用于自动确定FrameAnalyseFolder.
std::wstring MMTFile_FindMaxPrefixedDirectory(const std::wstring& directoryPath, const std::wstring& prefix) {
    std::vector<std::wstring> matchingDirectories;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_directory()) {
            std::wstring directoryName = entry.path().filename().wstring();
            if (directoryName.substr(0, prefix.size()) == prefix) {
                matchingDirectories.push_back(directoryName);
            }
        }
    }

    if (matchingDirectories.empty()) {
        return L"";
    }

    // 按字典序排序目录名称
    std::sort(matchingDirectories.begin(), matchingDirectories.end());

    // 返回最大的目录名称
    return matchingDirectories.back();
}


//在文件中查找指定属性，比如查找topology,需提供绝对路径 路径名，属性名
std::wstring MMTFile_FindMigotoIniAttributeInFile(const std::wstring& filePath, const std::wstring& attributeName) {
    std::wifstream file(filePath);
    std::wstring attributeValue;

    if (file.is_open()) {
        std::wstring line;
        while (std::getline(file, line)) {
            boost::algorithm::trim(line);
            if (line.find(attributeName + L":") != std::string::npos) {
                size_t pos = line.find(L":");
                std::wstring var = line.substr(pos + 1);
                boost::algorithm::trim(var);
                attributeValue = var;
                break;
            }
        }
        file.close();
    }

    return attributeValue;
}


bool MMTFile_IsValidFilename(std::string filename) {
    try {
        // 使用 std::filesystem::path 类来检查文件名是否有效
        std::filesystem::path p(filename);
        return p.filename() == filename;
    }
    catch (const std::exception&) {
        // 如果出现任何异常,说明文件名无效
        return false;
    }
}





