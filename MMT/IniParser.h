#pragma once
#include "GlobalConfigs.h"
#include <vector>


class ModResource {
public:
    std::wstring SectionName;
    std::wstring Format;
    int Stride;
    std::wstring FileName;
    std::wstring Type;

    ModResource() {
        this->SectionName = L"";
        this->Format = L"";
        this->Stride = 0;
        this->FileName = L"";
        this->Type = L"";
    }

    void show() {
        LOG.Info(L"SectionName: " + this->SectionName);
        LOG.Info(L"Format: " + this->Format);
        LOG.Info(L"Stride: " + std::to_wstring(this->Stride));
        LOG.Info(L"FileName: " + this->FileName);
        LOG.Info(L"Type: " + this->Type);
        LOG.NewLine();
    }
};


class KeyValuePair {
public:
    std::wstring key;
    std::wstring value;

    KeyValuePair(std::wstring inputLine) {
        std::vector<std::wstring> splitResultList;
        boost::split(splitResultList, inputLine, boost::is_any_of(L"="));
        if (splitResultList.size() < 2) {
            LOG.Error(L"Wrong kvpair format! Please check your input line.");
        }

        key = splitResultList[0];
        value = splitResultList[1];

        boost::algorithm::trim(key);
        boost::algorithm::trim(value);

    }
};


class Condition {
public:
    std::wstring ConditionName;
    int ConditionLevel;
    bool Positive;

    Condition() {
        this->ConditionName = L"";
        this->ConditionLevel = 0;
        this->Positive = true;
    }

    void show() {
        LOG.Info(L"ConditionName: " + this->ConditionName);
        LOG.Info(L"ConditionLevel: " + std::to_wstring(this->ConditionLevel));
        LOG.Info(L"Positive: " + std::to_wstring(this->Positive));
        LOG.NewLine();
    }
};


class ResourceReplace {
public:
    std::wstring OriginalLine;
    std::vector<Condition> ConditionList;

    ResourceReplace() {
        this->OriginalLine = L"";
        //this->ConditionList = std::vector<Condition>();
    }

    void show() {
        LOG.Info(L"OriginalLine: " + this->OriginalLine);
        for (Condition condition : this->ConditionList) {
            condition.show();
        }
        LOG.NewLine();
    }
};


class CycleKey {
public:
    std::wstring Key;
    std::wstring VarName;
    std::vector<std::wstring> VarValueList;

    CycleKey() {
        this->Key = L"";
        this->VarName = L"";
        //this->VarValueList = std::vector<std::wstring>();
    }

    void show() {
        LOG.Info(L"Key: " + this->Key);
        LOG.Info(L"VarName: " + this->VarName);
        LOG.Info(L"VarValueList: ");
        for (std::wstring varValue : this->VarValueList) {
            LOG.Info(L"VarValue: " + varValue);
        }
        LOG.NewLine();
    }
};


class TextureOverrideIB {
public:
    std::wstring SectionName;
    std::wstring HashValue;
    int MatchFirstIndex = 0;
    std::wstring IBResourceName;
    std::wstring IBFileName;
    std::wstring IBFileFormat;

    int minNumber;
    int maxNumber;

    //For fuzzy match
    int indexCount;


    TextureOverrideIB() {
        this->SectionName = L"";
        this->HashValue = L"";
        this->MatchFirstIndex = 0;
        this->IBResourceName = L"";
        this->IBFileName = L"";
        this->IBFileFormat = L"";

        this->minNumber = 0;
        this->maxNumber = 0;
        this->indexCount = 0;
    }

    void show() {
        LOG.Info(L"SectionName: " + SectionName);
        LOG.Info(L"HashValue: " + HashValue);
        LOG.Info(L"MatchFirstIndex: " + std::to_wstring(MatchFirstIndex));
        LOG.Info(L"IBResourceName: " + IBResourceName);
        LOG.Info(L"IBFileName: " + IBFileName);
        LOG.Info(L"IBFileFormat: " + IBFileFormat);
        LOG.Info(L"minNumber: " + std::to_wstring(minNumber));
        LOG.Info(L"maxNumber: " + std::to_wstring(maxNumber));
        LOG.NewLine();
    }
};


class TextureOverride {
public:
    std::wstring SectionName;
    std::wstring HashValue;
    std::wstring ActiveConditionStr;
    std::wstring MatchFirstIndex;

    std::vector<std::wstring> CommandList;
    std::vector<ResourceReplace> ResourceReplaceList;
    std::vector<ResourceReplace> ActiveResourceReplaceList;

    TextureOverride() {
        this->SectionName = L"";
        this->HashValue = L"";
        this->ActiveConditionStr = L"";
        this->MatchFirstIndex = L"";
    }

    void show() {
        LOG.Info(L"SectionName: " + this->SectionName);
        LOG.Info(L"HashValue: " + this->HashValue);
        LOG.Info(L"ActiveConditionStr: " + this->ActiveConditionStr);
    }

};


class DrawIBMod {
public:
    std::wstring hash;
    std::vector<TextureOverrideIB> textureOverrideIBList;
    std::vector<ModResource> resourceVBList;

};


class ReversedObject {
public:
    std::wstring outputFolderGameTypePath;

    std::vector<std::string> elementList;
    D3D11GameType d3d11GameType;

    std::vector<TextureOverrideIB> textureOverrideIBList;
    std::vector<ModResource> resourceVBList;
};



class ReverseModelSingle {
public:
    std::wstring ApplicationRunningLoaction;

    std::wstring iniFilePath;
    std::wstring iniFolderPath;
    std::wstring outputFolderPath;

    std::vector<DrawIBMod> drawIBModList;

    std::vector<ReversedObject> reversedObjectList;


    ReverseModelSingle(){
        this->iniFilePath = L"";
        this->iniFolderPath = L"";
        this->outputFolderPath = L"";
    }

    ReverseModelSingle(std::wstring);

    void parseElementList(std::wstring);

    void reverseAndOutput();

};









std::unordered_map<std::wstring, std::vector<ResourceReplace>> parseCommandResourceReplaceListDict(std::vector<std::wstring> lineList);
std::vector<TextureOverride> parseTextureOverrideList(std::vector<std::wstring> lineList);
std::vector<TextureOverride> matchActiveResourceReplace(
    std::vector<TextureOverride> textureOverrideList,
    std::unordered_map<std::wstring, std::vector<ResourceReplace>> commandResourceReplaceListDict);


std::unordered_map<std::wstring, ModResource> parseResourceDict(std::vector<std::wstring> lineList, std::wstring iniLocationFolder);
std::vector<CycleKey> parseCycleKeyList(std::vector<std::wstring> lineList);
std::vector<std::unordered_map<std::wstring, std::wstring>> calculateKeyCombination(std::vector<CycleKey> cycleKeyList);


std::vector<ModResource> parseResourceBufferList(std::vector<std::wstring> lines, std::wstring reverseIniFolderPath);
std::vector<ModResource> parseResourceIBList(std::vector<std::wstring> lines, std::wstring reverseIniFolderPath);
std::vector<ModResource> parseResourceAllList(std::vector<std::wstring> lines, std::wstring reverseIniFolderPath);


std::vector<DrawIBMod> parseDrawIBModList(
    std::vector<TextureOverrideIB> TextureOverrideIBList,
    std::vector<ModResource> ResourceVBList,
    std::vector<ModResource> ResourceIBList);

void parseMergedInI(std::wstring filePath,std::wstring gameName,std::wstring ApplicationLocation);







