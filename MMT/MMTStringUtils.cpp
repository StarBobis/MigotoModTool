#include "MMTStringUtils.h"
#include <filesystem>
// uuid
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
// time
#include <boost/date_time/posix_time/posix_time.hpp>


std::wstring MMTString_GetFolderPathFromFilePath(std::wstring filePath) {
    std::size_t lastSlash = filePath.find_last_of(L"\\/");
    std::wstring path = filePath.substr(0, lastSlash);
    return path;
}

std::wstring MMTString_GetFileNameFromFilePath(std::wstring filePath) {
    std::filesystem::path pathObj(filePath);
    return pathObj.filename();
}


std::wstring MMTString_ToWideString(std::string input) {
    if (input.empty()) return L"";

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, NULL, 0);
    if (size_needed == 0) {
        // Handle error appropriately
        throw std::runtime_error("Failed in MultiByteToWideChar conversion.");
    }

    std::wstring wstrTo(size_needed, L'\0');
    int chars_converted = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, &wstrTo[0], size_needed);
    if (chars_converted == 0) {
        // Handle error appropriately
        throw std::runtime_error("Failed in MultiByteToWideChar conversion.");
    }

    // Remove the null terminator as it is implicitly handled in std::wstring
    wstrTo.pop_back();

    return wstrTo;
}


std::string MMTString_ToByteString(std::wstring input) {
    if (input.empty()) return "";

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
    if (size_needed == 0) {
        // Handle error appropriately
        throw std::runtime_error("Failed in WideCharToMultiByte conversion.");
    }

    std::string strTo(size_needed, '\0');
    int bytes_converted = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, &strTo[0], size_needed, NULL, NULL);
    if (bytes_converted == 0) {
        // Handle error appropriately
        throw std::runtime_error("Failed in WideCharToMultiByte conversion.");
    }

    // Remove the null terminator as it is implicitly handled in std::string
    strTo.pop_back();

    return strTo;
}


std::wstring MMTString_GenerateUUIDW() {
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();
    return MMTString_ToWideString(boost::uuids::to_string(uuid));
}


std::wstring MMTString_GetFormattedDateTimeForFilename()
{
    // 获取本地日期和时间
    boost::posix_time::ptime localTime =
        boost::posix_time::second_clock::local_time();

    // 格式化日期和时间为字符串
    std::wstring dateTimeStr =
        boost::posix_time::to_iso_extended_wstring(localTime);

    // 用下划线替换日期和时间中的非字母数字字符
    boost::replace_all(dateTimeStr, "-", "_");
    boost::replace_all(dateTimeStr, ":", "_");

    return dateTimeStr;
}

//原始字符串，用于分割的字符串
//注意如果找不到分隔符会返回一个长度为1的列表装着原始的字符串，所以判断的时候要用1而不是0
std::vector <std::wstring> MMTString_SplitString(std::wstring originalStr, std::wstring delimiter) {
    std::vector<std::wstring> result;
    std::wstring str = originalStr;
    size_t pos = 0;
    while ((pos = str.find(delimiter)) != std::wstring::npos) {
        std::wstring token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    result.push_back(str);
    return result;
}


std::wstring MMTString_ToLowerCase(const std::wstring str)
{
    std::wstring result;
    std::locale loc;

    for (wchar_t c : str)
    {
        result += std::tolower(c, loc);
    }

    return result;
}


std::wstring MMTString_GetPSHashFromFileName(const std::wstring& input) {
    std::wstring result;
    size_t pos = input.find(L"-ps=");
    if (pos != std::wstring::npos && pos + 4 + 16 <= input.length()) {
        result = input.substr(pos + 4, 16);
    }
    return result;
}


std::wstring MMTString_GetVSHashFromFileName(const std::wstring& input) {
    std::wstring result;
    size_t pos = input.find(L"-vs=");
    if (pos != std::wstring::npos && pos + 4 + 16 <= input.length()) {
        result = input.substr(pos + 4, 16);
    }
    return result;
}

//请确保输入的字符串是[]括起来的
std::wstring MMTString_RemoveSquareBrackets(std::wstring inputStr) {
    if (inputStr.size() >= 2 && inputStr.front() == L'[' && inputStr.back() == L']') {
        inputStr = inputStr.substr(1, inputStr.size() - 2);
    }
    return inputStr;
}
