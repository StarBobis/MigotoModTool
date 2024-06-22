#include "ModFormatExtra.h"
#include "MMTFileUtils.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include "IndexBufferBufFile.h"
#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MMTLogUtils.h"
#include "MMTFormatUtils.h"
#include "MMTFileUtils.h"
#include <filesystem>


void ModFormat_INI::Parse_Self_VertexNumberMResourceVBMap() {
    //1.获取所有的ResourceVB
    //根据Resource的顶点数来区分不同的Resource合集，相同顶点数的Resource在相同的Mod里
    std::unordered_map<uint32_t, std::vector<M_Resource>> vertexNumberMResourceVBMap;
    for (const auto& pair : this->Global_ResourceName_Resource_Map) {
        M_Resource m_resource = pair.second;
        //因为VB类型约定俗成需要有Stride和Type，所以这里只过滤出VB类型
        if (m_resource.Stride == L"") {
            continue;
        }
        if (m_resource.Type == L"") {
            continue;
        }

        std::wstring FilePath = m_resource.NameSpace + L"\\" + m_resource.FileName;
        m_resource.FilePath = FilePath;
        uint32_t fileSize = MMTFile_GetFileSize(FilePath);
        uint32_t stride = std::stoi(m_resource.Stride);
        uint32_t vertexNumber = fileSize / stride;

        if (vertexNumber > 0) {
            std::vector<M_Resource> tmpList;
            if (vertexNumberMResourceVBMap.contains(vertexNumber)) {
                tmpList = vertexNumberMResourceVBMap[vertexNumber];
            }
            tmpList.push_back(m_resource);
            vertexNumberMResourceVBMap[vertexNumber] = tmpList;
        }
        LOG.Info(FilePath);
        LOG.Info("fileSize: " + std::to_string(fileSize));
        LOG.Info("stride: " + std::to_string(stride));
        LOG.Info("vertexNumber: " + std::to_string(vertexNumber));
    }
    this->VertexNumberMResourceVBMap = vertexNumberMResourceVBMap;
    LOG.NewLine();
}


void ModFormat_INI::Parse_Self_Hash_TextureOverrideIBList_Map() {
    //根据每个Hash分开TextureOverrideIB列表
    std::unordered_map<std::wstring, std::vector<M_TextureOverride>> hash_TextureOverrideIBList_Map;
    for (M_TextureOverride m_texture_override : this->Global_M_TextureOverrideList) {
        std::wstring IBHashValue = m_texture_override.IndexBufferHash;

        //查找这个TextureOverride的所有ResourceReplace，看看有没有ib在其中
        bool findIBResourceReplace = false;
        for (M_ResourceReplace resource_replace : m_texture_override.ResourceReplaceList) {
            if (resource_replace.ReplaceTarget == L"ib") {
                if (resource_replace.ReplaceTarget != L"null") {
                    findIBResourceReplace = true;
                    break;
                }
            }
        }
        //LOG.Info(L"IBHashValue: " + IBHashValue);
        if (findIBResourceReplace) {
            std::vector<M_TextureOverride> TextureOverrideIBList = hash_TextureOverrideIBList_Map[IBHashValue];
            TextureOverrideIBList.push_back(m_texture_override);
            hash_TextureOverrideIBList_Map[IBHashValue] = TextureOverrideIBList;
            LOG.Info(L"Detect [TextureOverrideIB] : " + IBHashValue + L" [match_first_index]: " + m_texture_override.MatchFirstIndex);
        }
        else {
            //如果不是IB类型的话，不为0是正常的，没必要输出出来
            //LOG.Info(L"m_texture_override.DrawIndexedList.size() is 0!");
        }
    }
    this->Hash_TextureOverrideIBList_Map = hash_TextureOverrideIBList_Map;
}


void ModFormat_INI::Parse_Self_Hash_SingleModDetect_Map() {
    //这俩前置步骤必须得有，后面强依赖
    Parse_Self_VertexNumberMResourceVBMap();
    Parse_Self_Hash_TextureOverrideIBList_Map();

    //这里已经根据Hash分开了每个Hash对应的TextureOverrideIB，那么这时应该根据顶点数去Resource中匹配对应的Resource，组合成一个单独的Mod
    //这样每个Hash都是一个单独的Mod，到了外面只需要根据Hash进行分类输出即可。
    for (const auto& pair : this->Hash_TextureOverrideIBList_Map) {
        std::wstring Hash = pair.first;
        std::vector<M_TextureOverride> TextureOverrideIBList = pair.second;
        //遍历TextureOverrideIBList并查找对应的ResourceReplace的IBResource来计算顶点数
        std::vector<M_TextureOverride> matchedTextureOverrideIBList;
        int IndexBufferVertexCountTotal = 0;
        for (M_TextureOverride textureOverride : TextureOverrideIBList) {
            for (M_ResourceReplace resourceReplace : textureOverride.ResourceReplaceList) {
                if (resourceReplace.ReplaceTarget == L"ib") {
                    //在Resource列表里查找对应的Resource
                    for (M_Resource m_resource : this->Global_M_ResourceList) {
                        if (m_resource.ResourceName == resourceReplace.ReplaceResource) {
                            textureOverride.IBResourceName = m_resource.ResourceName;
                            textureOverride.IBFileName = m_resource.FileName;
                            textureOverride.IBFormat = m_resource.Format;
                            textureOverride.IBFilePath = textureOverride.NameSpace + L"\\" + textureOverride.IBFileName;

                            IndexBufferBufFile ibBufFile(textureOverride.IBFilePath, textureOverride.IBFormat);
                            IndexBufferVertexCountTotal = IndexBufferVertexCountTotal + ibBufFile.UniqueVertexCount;
                            textureOverride.IBBufFile = ibBufFile;

                            LOG.Info(m_resource.ResourceName + L"  " + resourceReplace.ReplaceResource);
                            break;
                        }
                    }
                    matchedTextureOverrideIBList.push_back(textureOverride);
                    break;
                }
            }
        }

        SingleModDetect singleModDetect;
        singleModDetect.HashValue = Hash;
        singleModDetect.TextureOverrideIBList = matchedTextureOverrideIBList;
        singleModDetect.ResourceVBList = VertexNumberMResourceVBMap[IndexBufferVertexCountTotal];
        this->Hash_SingleModDetect_Map[Hash] = singleModDetect;

        LOG.Info("IndexBufferVertexCountTotal: " + std::to_string(IndexBufferVertexCountTotal));
        LOG.Info("Matched ResourceVBList Size: " + std::to_string(singleModDetect.ResourceVBList.size()));
        LOG.NewLine();
    }

}
