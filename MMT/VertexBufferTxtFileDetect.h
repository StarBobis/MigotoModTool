#pragma once

#include "D3d11GameType.h"
#include <iostream>
#include <vector>
#include <unordered_map>


class VertexBufferDetect {
public:
    std::wstring Index;
    std::wstring Topology;
    //正常识别出来的ElementList的map
    std::unordered_map<std::string, D3D11Element> elementNameD3D11ElementMap;
    //在此文件中真正存在数据的ElementList的名称列表
    std::vector<std::string> realElementNameList;
    std::vector<std::string> showElementNameList;
    std::vector<D3D11Element> realElementList;
    std::vector<D3D11Element> showElementList;
    int fileShowStride = 0;
    int fileElementListShowStride = 0;
    int fileRealStride = 0;

    int vbSlotNumber = 0;

    //.txt文件里展示的顶点数量
    int fileShowVertexCount = 0; 
    //.txt文件里真实的数据行数
    int fileRealVertexCount = 0;
    //对应.buf文件中的 文件大小/展示步长 得到的顶点数量
    int fileBufferVertexCount = 0;
    //对应.buf文件中的去掉结尾0的真实文件大小/展示步长 得到真实顶点数量
    int fileBufferRealVertexCount = 0;

    VertexBufferDetect();
    VertexBufferDetect(std::wstring);
};