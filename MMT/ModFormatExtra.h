#pragma once
//-----------------------------------------------------------------------------------------------------------------------------------
// 这里存放的是更高级的抽象数据类型，主要用于解析3Dmigoto的Mod格式
//-----------------------------------------------------------------------------------------------------------------------------------
#include "D3d11GameType.h"
#include <iostream>
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"
#include <boost/algorithm/string.hpp>
#include "IndexBufferBufFile.h"
#include "ModFormatBasic.h"


class SingleModDetect {
public:
	std::wstring HashValue;
	std::vector<M_TextureOverride> TextureOverrideIBList;
	std::vector<M_Resource> ResourceVBList;

	SingleModDetect() {}
};


class ModFormat_INI {
public:
	//单纯解析ini所有行并抽象为Section
	std::vector<M_SectionLine> MigotoSectionLineList;

	//初步解析所有的Section转换为可理解的基础类型
	std::vector<M_Variable> Global_M_VariableList;
	std::vector<M_Key> Global_M_KeyList;
	std::vector<M_Resource> Global_M_ResourceList;
	std::vector<M_TextureOverride> Global_M_TextureOverrideList;
	std::unordered_map<std::wstring, M_Resource> Global_ResourceName_Resource_Map;

	//进一步解析,中间解析产物，并不是每个步骤都用得上
	std::unordered_map<uint32_t, std::vector<M_Resource>> VertexNumberMResourceVBMap;
	std::unordered_map<std::wstring, std::vector<M_TextureOverride>> Hash_TextureOverrideIBList_Map;

	//根据顶点数量匹配出一个Mod来，组装成Map最方便易于使用，很常用
	std::unordered_map <std::wstring, SingleModDetect> Hash_SingleModDetect_Map;

	//用于解析基础数据的方法
	std::vector<M_Variable> Parse_Basic_ConstantsSection(M_SectionLine m_sectionLine);
	M_Key Parse_Basic_KeySection(M_SectionLine m_sectionLine);
	M_Resource Parse_Basic_ResourceSection(M_SectionLine m_sectionLine);
	M_TextureOverride Parse_Basic_TextureOverrideSection(M_SectionLine m_sectionLine);

	//功能性解析，不一定要使用
	std::vector<std::unordered_map<std::wstring, std::wstring>> Parse_Util_Get_M_Key_Combination(std::vector<M_Key> cycleKeyList);
	std::vector<M_DrawIndexed> Parse_Util_GetActiveDrawIndexedListByKeyCombination(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap, std::vector<M_DrawIndexed> DrawIndexedList);
	std::wstring Parse_Util_Get_M_Key_Combination_String(std::unordered_map<std::wstring, std::wstring> KeyCombinationMap);
	std::vector<std::wstring> Parse_Util_GetRecursiveActivedIniFilePathList(std::wstring IncludePath);
	std::vector<M_SectionLine> Parse_Util_ParseMigotoSectionLineList(std::wstring iniFilePath);

	//自我赋值解析，经常用到
	void Parse_Self_VertexNumberMResourceVBMap();
	void Parse_Self_Hash_TextureOverrideIBList_Map();
	void Parse_Self_Hash_SingleModDetect_Map();

	//输入ini路径，初步解析出所有的基础类型
	ModFormat_INI(std::wstring IniFilePath);
};




//用于输出Mod
//TODO 
class ValidModDetect {
public:
	std::vector<M_Resource> OrderedResourceVBList;
	std::vector<M_TextureOverride> TextureOverrideIBList;
	D3D11GameType d3d11GameType;

	std::wstring OutputFolderGameTypePath;
	std::vector<std::string> ElementList;

	//这里一定要注意，索引是从1开始的，并不是从0开始的
	std::unordered_map<int, std::vector<byte>> VB0IndexNumberBytes;
	std::vector<byte> finalVB0Bytes;
	bool ValidMod = false;


	//尝试组装一个用于输出的Mod，如果组装成功则ValidMod被设为true
	ValidModDetect(D3D11GameType d3d11GameTypeInput, std::vector<M_Resource> ResourceVBListInput, std::vector<M_TextureOverride> TextureOverrideIBListInput);
};