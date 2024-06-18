#pragma once

class WuwaCSInfo {
public:
    //之前已计算顶点的总和
    int Offset;
    //当前CS计算次数，也就是当前的部件的顶点数
    int CalculateTime;
    //使用哪个Shader来进行Pre-Skinning
    std::wstring ComputeShaderHash;
};
