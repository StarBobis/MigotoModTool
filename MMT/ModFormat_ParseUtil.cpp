#include "ModFormatExtra.h"
#include "MMTFileUtils.h"
#include <filesystem>

//古董老代码，GPT写的
void cartesianProductHelper2(const std::vector<std::vector<std::wstring>>& data, std::vector<std::wstring>& current, std::vector<std::vector<std::wstring>>& result, size_t index) {
    if (index >= data.size()) {
        result.push_back(current);
        return;
    }

    for (const auto& str : data[index]) {
        current.push_back(str);
        cartesianProductHelper2(data, current, result, index + 1);
        current.pop_back();
    }
}


//古董老代码，GPT写的
std::vector<std::vector<std::wstring>> cartesianProduct2(const std::vector<std::vector<std::wstring>>& data) {
    std::vector<std::vector<std::wstring>> result;
    std::vector<std::wstring> current;
    cartesianProductHelper2(data, current, result, 0);
    return result;
}


std::vector<std::unordered_map<std::wstring, std::wstring>> ModFormat_INI::Parse_Util_Get_M_Key_Combination(std::vector<M_Key> cycleKeyList) {
    LOG.Info(L"Start to calculateKeyCombination");
    std::vector<std::unordered_map<std::wstring, std::wstring>> keyCombinationDictList;

    std::vector<std::vector<std::wstring>> varValuesList;
    std::vector<std::wstring> varNameList;

    for (M_Key cycleKey : cycleKeyList) {

        if (cycleKey.CycleVariableName_PossibleValueList_Map.size() != 0) {

            for (const auto& pair : cycleKey.CycleVariableName_PossibleValueList_Map) {
                LOG.Info(L"Key: " + pair.first);

                std::vector<std::wstring> trueVarValueList;
                for (std::wstring varValue : pair.second) {
                    LOG.Info(L"Value: " + varValue);
                    trueVarValueList.push_back(varValue);
                }
                varValuesList.push_back(trueVarValueList);
                varNameList.push_back(pair.first);
            }
        }
    }

    std::vector<std::vector<std::wstring>> cartesianProductList = cartesianProduct2(varValuesList);
    for (std::vector<std::wstring> keyCombinationValueList : cartesianProductList) {
        std::unordered_map<std::wstring, std::wstring> tmpDict;
        for (int i = 0; i < keyCombinationValueList.size(); i++) {
            std::wstring cycleVarValue = keyCombinationValueList[i];
            std::wstring cycleVarName = varNameList[i];
            //LOG.LogOutput(cycleVarName + L" " + cycleVarValue);
            tmpDict[cycleVarName] = cycleVarValue;
        }
        keyCombinationDictList.push_back(tmpDict);
        //LOG.LogOutputSplitStr();
    }
    //LOG.LogOutputSplitStr();
    return keyCombinationDictList;
}


std::wstring ModFormat_INI::Parse_Util_Get_M_Key_Combination_String(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap) {
    std::wstring combinationStr;
    int count = 1;
    for (const auto& pair : KeyCombinationMap) {
        if (!MMTFile_IsValidFilename(MMTString_ToByteString(pair.first))) {
            //有些人会用文件名中不能出现的字符比如\ /来作为按键的值来对抗自动逆向
            //如果用文件名无法使用的字符来对抗逆向，那这里就使用数值代替
            //TODO 未来再补充
            combinationStr = combinationStr + L"$key" + std::to_wstring(count) + L"_";
        }
        else {
            combinationStr = combinationStr + L"$" + pair.first + L"_";
        }
        combinationStr = combinationStr + pair.second + L"_";
        count++;
    }



    return combinationStr;

}


std::vector<std::wstring> ModFormat_INI::Parse_Util_GetRecursiveActivedIniFilePathList(std::wstring IncludePath) {
    std::vector<std::wstring> includeFilePathList = MMTFile_GetFilePathListRecursive(IncludePath);
    std::vector<std::wstring> parseIniFilePathList;
    for (const auto& filePath : includeFilePathList)
    {
        std::filesystem::path filePathObject(filePath);
        std::wstring fileName = filePathObject.filename().wstring();

        std::wstring lowerFileName = MMTString_ToLowerCase(fileName);
        if (lowerFileName.starts_with(L"disabled")) {
            continue;
        }
        if (!lowerFileName.ends_with(L".ini")) {
            continue;
        }
        parseIniFilePathList.push_back(filePath);
        //LOG.Info(filePath);
    }
    //LOG.NewLine();
    return parseIniFilePathList;
}


std::vector<M_SectionLine> ModFormat_INI::Parse_Util_ParseMigotoSectionLineList(std::wstring iniFilePath) {
    std::vector<M_SectionLine> migotoSectionLineList;

    //首先把这个ini文件的每一行读取到列表里
    std::vector<std::wstring> readLineList = MMTFile_ReadIniFileLineList(iniFilePath);
    //初始化默认NameSpace为当前文件所在目录
    std::wstring defaultNameSpace = MMTString_GetFolderPathFromFilePath(iniFilePath);
    //如果读取到了强行设置的NameSpace，就放到这里供后续使用
    std::wstring specifiedNameSpace = L"";

    //之前的设计是每调用一个方法，解析一种数据类型，现在我们直接按Section来，
    //读取每一行，判断是否为[开头，是就进入Section读取区域，否则就读取是否为namespace
    //然后读取到Section之后呢，先不处理，先把此section所有的行放到一个section对象里，然后所有section对象放到列表，等待后续处理。
    std::vector<std::wstring> tmpSectionLineList;
    M_SectionLine lastMigotoSectionLine;
    bool inSection = false;
    for (std::wstring readLine : readLineList) {
        //LOG.LogOutput(L"Parsing: " + readLine);
        std::wstring lowerReadLine = boost::algorithm::to_lower_copy(readLine);
        boost::algorithm::trim(lowerReadLine);
        //跳过注释
        if (lowerReadLine.starts_with(L";")) {
            continue;
        }

        //在读取解析ini文件时，在所有的Section读取之前如果读取到了namespace，则该ini文件所有的namespace都设置为指定的namespace
        if (!inSection && lowerReadLine.starts_with(L"namespace")) {
            std::vector<std::wstring> readLineSplitList = MMTString_SplitString(readLine, L"=");
            if (readLineSplitList.size() < 2) {
                LOG.Error(L"Invalid namespace assign.");
            }
            //namespace = xxx，我们只考虑出现一个等号的情况，所以这里索引固定为1
            std::wstring RightStr = readLineSplitList[1];
            //去除两边空格
            boost::algorithm::trim(RightStr);
            specifiedNameSpace = RightStr;
        }
        else if (lowerReadLine.starts_with(L"[")) {
            //每遇到一个[都说明遇到了新的section
            inSection = true;

            //遇到新的Section要把旧的Section加到总的Section列表里
            if (tmpSectionLineList.size() != 0) {
                lastMigotoSectionLine.SectionLineList = tmpSectionLineList;
                //LOG.LogOutput(L"Add Size: " + std::to_wstring(lastMigotoSectionLine.SectionLineList.size()));
                migotoSectionLineList.push_back(lastMigotoSectionLine);
                //然后清空当前的列表准备读取新的。
                tmpSectionLineList.clear();
            }

            //然后添加新的
            lastMigotoSectionLine = M_SectionLine();
            lastMigotoSectionLine.NameSpace = defaultNameSpace;
            if (specifiedNameSpace != L"") {
                lastMigotoSectionLine.NameSpace = specifiedNameSpace;
            }
            std::wstring sectionName = lowerReadLine.substr(1, lowerReadLine.length() - 2);
            LOG.Info(L"SectionName: " + sectionName + L" NameSpace: " + lastMigotoSectionLine.NameSpace);
            lastMigotoSectionLine.SectionName = sectionName;
            //别忘了把当前的SectionName的行也加入进去
            //行添加之前必须判断不为空
            if (lowerReadLine != L"") {
                tmpSectionLineList.push_back(readLine);
            }
        }
        else if (inSection) {
            if (lowerReadLine != L"") {
                tmpSectionLineList.push_back(readLine);
            }
        }

    }

    // 最后结尾时把最后一个Section的line也添加进去
    //遇到新的Section要把旧的Section加到总的Section列表里
    if (lastMigotoSectionLine.SectionLineList.size() != 0) {
        lastMigotoSectionLine.SectionLineList = tmpSectionLineList;
        //LOG.LogOutput(L"Add Size: " + std::to_wstring(lastMigotoSectionLine.SectionLineList.size()));
        migotoSectionLineList.push_back(lastMigotoSectionLine);
        //然后清空当前的列表准备读取新的。
        tmpSectionLineList.clear();
    }

    LOG.Info(L"MigotoSectionLineList Size: " + std::to_wstring(migotoSectionLineList.size()));
    LOG.NewLine();
    return migotoSectionLineList;
}

//--

