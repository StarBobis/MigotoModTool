#include "D3d11GameType.h"
#include "GlobalConfigs.h"

//根据ElementList来计算总stride
int D3D11GameType::getElementListStride(std::vector<std::string> inputElementList) {
	int totalStride = 0;
	for (std::string elementName : inputElementList) {
		std::string elementNameUpper = boost::algorithm::to_upper_copy(elementName);
		D3D11Element elementObject = ElementNameD3D11ElementMap[elementNameUpper];
		totalStride = totalStride + elementObject.ByteWidth;
	}
	return totalStride;
}


D3D11GameType::D3D11GameType() {

}


//根据ElementList来计算总stride
std::unordered_map<std::string, int>  D3D11GameType::getCategoryStrideMap(std::vector<std::string> inputElementList) {
    std::unordered_map<std::string, int> CategoryStrideMap;
    for (std::string elementName : inputElementList) {
        D3D11Element elementObject = this->ElementNameD3D11ElementMap[elementName];
        int byteWidth = elementObject.ByteWidth;
        std::wstring elementCategory = MMTString_ToWideString(elementObject.Category);
        int categoryStride = CategoryStrideMap[MMTString_ToByteString(elementCategory)];
        if (categoryStride == NULL) {
            categoryStride = byteWidth;
        }
        else {
            categoryStride = categoryStride + byteWidth;
        }
        CategoryStrideMap[MMTString_ToByteString(elementCategory)] = categoryStride;
    }
    return CategoryStrideMap;
}


std::vector<std::string>   D3D11GameType::getCategoryList(std::vector<std::string> inputElementList) {
    std::vector<std::string> CategoryList;
    for (std::string elementName : inputElementList) {
        D3D11Element elementObject = this->ElementNameD3D11ElementMap[elementName];
        int byteWidth = elementObject.ByteWidth;
        std::wstring elementCategory = MMTString_ToWideString(elementObject.Category);
        if (!boost::algorithm::any_of_equal(CategoryList, MMTString_ToByteString(elementCategory))) {
            CategoryList.push_back(MMTString_ToByteString(elementCategory));
        }

    }
    return CategoryList;
}


std::vector<std::string> D3D11GameType::getCategoryElementList(std::vector<std::string> inputElementList, std::string category) {
    std::vector <std::string> positionElementListTmp;
    for (std::string elementName : inputElementList) {
        D3D11Element d3d11ElementTmp = this->ElementNameD3D11ElementMap[elementName];
        if (d3d11ElementTmp.Category == category) {
            positionElementListTmp.push_back(elementName);
        }
    }
    return positionElementListTmp;
}

std::vector<std::string> D3D11GameType::getReorderedElementList(std::vector<std::string> elementList) {
    std::vector<std::string> orderedElementList;
    for (std::string elementName : this->OrderedFullElementList) {
        for (std::string element : elementList) {
            if (boost::algorithm::to_lower_copy(element) == boost::algorithm::to_lower_copy(elementName)) {
                orderedElementList.push_back(elementName);
                break;
            }
        }
    }
    return orderedElementList;
}

