#pragma once
#include <vector>
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"
#include <boost/algorithm/string.hpp>
#include "IndexBufferBufFile.h"

//-----------------------------------------------------------------------------------------------------------------------------------
// 这里存放基础数据类型，指的是从ini里进行分析而提取的第一层抽象信息，方便后续所有的分析过程
// 用于解析3Dmigoto的基本数据类型
//-----------------------------------------------------------------------------------------------------------------------------------


class IniLineObject {
public:
	std::wstring LeftStr;
	std::wstring LeftStrTrim;
	std::wstring RightStr;
	std::wstring RightStrTrim;
	bool valid = false;

	IniLineObject();

	IniLineObject(std::wstring readLine);
	//下面这个留着指定分隔符，上面那个用来判断变量相关的时候
	IniLineObject(std::wstring readLine, std::wstring delimiter);
};


class M_SectionLine {
public:
	std::wstring NameSpace;
	std::wstring SectionName;
	std::vector<std::wstring> SectionLineList;

	M_SectionLine();
};


//同时用于constants和present
class M_Variable {
public:
	std::wstring NameSpace;
	std::wstring VariableName;
	std::wstring InitializeValue = L"";
	std::wstring NamespacedVarName;
	std::wstring Type; //global local normal

	M_Variable();
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType);
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType);
};


//Condition代表一个条件，一个Condition中可以有多个对比表达式
//比如 if $var == 1 && $var2 == 2 此时$var,1就是一个对比表达式，代表此Condition生效的条件之一
//Condition_VarName_VarValue_Map即存储了此Condition生效的所有条件
//一个ResourceReplace可以有多个M_Condition，这是正常的，这是因为if会有多个嵌套
//我们每个Condition都代表一个if或else if后的对比表达式的组合，而不考虑嵌套问题，嵌套应该在TextureOverride解析时进行考虑。
class M_Condition {
public:
	std::wstring NameSpace;
	std::unordered_map<std::wstring, std::wstring> Condition_VarName_VarValue_Map;

	void show();

	M_Condition();

	M_Condition(std::wstring ConditionStr);
};


class M_Key {
public:
	std::wstring NameSpace;
	M_Condition Condition;
	std::wstring KeyName;
	std::wstring BackName;
	std::wstring Type;

	//例如 $variable5 = 0,1 代表按下这个键之后会让$variable5的值在0,1之间循环切换到下一个，所以是循环变量
	std::unordered_map<std::wstring, std::vector<std::wstring>> CycleVariableName_PossibleValueList_Map;

	//例如$creditinfo = 0 代表按下这个键之后会立刻激活并将$creditinfo设为0，所以是激活变量
	std::unordered_map<std::wstring, std::wstring> ActiveVariableName_ActiveValue_Map;
};


class M_DrawIndexed {
public:
	//例如DrawIndexed = 6,12,0 表示DrawStartIndex为0，DrawOffsetIndex为12，DrawNumber为6
	//即从索引0开始绘制，从Offset开始读取，读取DrawNumber个数量来绘制
	bool AutoDraw = false;
	std::wstring DrawNumber;
	std::wstring DrawOffsetIndex;
	std::wstring DrawStartIndex;

	std::vector<M_Condition> ActiveConditionList;

	M_DrawIndexed();
};


class M_ResourceReplace {
public:
	std::wstring ReplaceTarget;
	std::wstring ReplaceResource;
	std::vector<M_Condition> ActiveConditionList;

};


class M_TextureOverride {
public:
	std::wstring NameSpace;
	std::wstring IndexBufferHash;
	std::vector<M_ResourceReplace> ResourceReplaceList;
	std::wstring MatchPriority;
	std::wstring Handling; //skip
	std::wstring MatchFirstIndex;
	std::vector<M_DrawIndexed> DrawIndexedList;

	//用于处理TextureOverrideIB相关逻辑
	std::wstring IBResourceName;
	std::wstring IBFileName;
	std::wstring IBFilePath;
	std::wstring IBFormat;
	IndexBufferBufFile IBBufFile;

};


class M_Resource {
public:
	std::wstring NameSpace = L"";
	std::wstring ResourceName = L"";
	std::wstring Type = L"";
	std::wstring Stride = L"";
	std::wstring Format = L"";
	std::wstring FileName = L"";

	//后面拼接出来的属性，根据NameSpace和FileName
	std::wstring FilePath = L"";
};


//TODO 要完全解析还是得参考原本的3Dmigoto解析流程，有空再研究吧。
class M_CommandList {
public:
	std::wstring NameSpace = L"";


};