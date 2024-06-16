#pragma once
#include <string>
#include <vector>
#include <unordered_map>


class D3D11Element {
public:
    std::string SemanticName = "";
    std::string SemanticIndex = "";
    std::string Format = "";
    std::string InputSlot = "0";
    std::string InputSlotClass = "per-vertex";
    std::string InstanceDataStepRate = "0";
    int ByteWidth = 0;
    std::string ExtractSlot = "";
    std::string ExtractTechnique = "";
    std::string Category = "";

    //需要动态计算
    int ElementNumber = 0;
    int AlignedByteOffset = 0;
};


class D3D11GameType {
public:
    std::string GameType;

    //是否使用GPU计算骨骼姿态变换
    bool GPUPreSkinning = false;

    //是否需要补充blendweights，默认不需要
    bool PatchBLENDWEIGHTS = false;

    //UE4补充0
    bool UE4PatchNullInBlend = false;

    //原本的类别，在ini文件中替换时写到的draw的位置类别，每一个配置里都需要手动指定
    std::unordered_map <std::string, std::string> CategoryDrawCategoryMap;
    std::vector<std::string> OrderedFullElementList;

    //需要后续计算得出的属性
    std::unordered_map<std::string, D3D11Element> ElementNameD3D11ElementMap;
    std::unordered_map <std::string, std::string> CategorySlotMap;
    std::unordered_map <std::string, std::string> CategoryTopologyMap;

    D3D11GameType();

    //根据提供的ElementList获取总的Stride
    int getElementListStride(std::vector<std::string>);

    //获取Category Stride Map
    std::unordered_map<std::string, int>  getCategoryStrideMap(std::vector<std::string> inputElementList);

    //获取Category List
    std::vector<std::string>  getCategoryList(std::vector<std::string> inputElementList);

    //获取Category的ElementList
    std::vector<std::string> getCategoryElementList(std::vector<std::string> inputElementList,std::string category);

    //根据OrderedElementList重新排序传入的ElementList
    std::vector<std::string> getReorderedElementList(std::vector<std::string> elementList);
};