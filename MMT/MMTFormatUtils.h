#pragma once
#include "VertexBufferTxtFile.h"

//读取字节并转换为数字类型。
std::uint32_t MMTFormat_CharArrayToUINT32_T(char* data);
std::uint16_t MMTFormat_CharArrayToUINT16_T(char* data);
std::vector<std::byte> MMTFormat_Reverse16BitShortValue(std::vector<std::byte> shortValue);
std::vector<std::byte> MMTFormat_Reverse32BitFloatValue(std::vector<std::byte> floatValue);
std::map<int, std::vector<VertexDataLine>> MMTFormat_MergeIndexVertexDataLineListMap(std::map<int, std::vector<VertexDataLine>> OriginalMap, std::map<int, std::vector<VertexDataLine>> AdditionalMap);
std::vector<std::byte> MMTFormat_PackNumberOneByte(int packNumber);
std::vector<std::byte> MMTFormat_ReverseSNORMValue(std::vector<std::byte>& snormValue);
std::byte MMTFormat_ReverseSNORMValueSingle(std::byte snormValue);
std::byte MMTFormat_UnormToSNORMValueSingle(std::byte Value);


//D3D11及数据类型相关操作
std::wstring MMTFormat_NearestFloat(std::wstring originalValue);
std::wstring MMTFormat_NearestFormatedFloat(std::wstring originalValue, int reserveCount);
std::wstring MMTFormat_FormatedFloat(double originalValue, int reserveCount);
std::byte MMTFormat_PackNumberToByte(std::uint32_t number);
std::vector<std::byte> MMTFormat_PackNumberR32_FLOAT_littleIndian(float number);
std::vector<std::vector<double>> MMTFormat_CalculateAverageNormalsAndStoreTangent(const std::vector<std::vector<double>> positions, const std::vector<std::vector<double>> normals);
std::vector<std::vector<double>> MMTFormat_NormalNormalizeStoreTangent(const std::vector<std::vector<double>> positions, const std::vector<std::vector<double>> normals);
double MMTFormat_ByteVectorToFloat(const std::vector<std::byte>& bytes);

//方便的就像Python一样根据范围获取
std::vector<std::byte> MMTFormat_GetRange_Byte(const std::vector<std::byte>& vec, std::size_t startIndex, std::size_t endIndex);
std::vector<uint32_t> MMTFormat_GetRange_UINT32T(const std::vector<uint32_t>& vec, std::size_t startIndex, std::size_t endIndex);
std::uint32_t MMTFormat_STDByteVectorToUINT32_T(std::vector<std::byte> STDByteVector);
std::vector<std::byte> MMTFormat_IntToByteVector(int value);

//TODO 搞到一个新的数据类型叫Vector3D类型里
struct Vector3D {
    double x;
    double y;
    double z;
};
double VectorLength(const Vector3D& vector);
Vector3D NormalizeVector(const Vector3D vector);
Vector3D AddVectors(const Vector3D vector1, const Vector3D vector2);