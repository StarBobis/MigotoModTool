#pragma once
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

	IniLineObject() {}

	IniLineObject(std::wstring readLine) {
		int firstDoubleEqualIndex = (int)readLine.find(L"==");
		int firstEqualIndex = (int)readLine.find(L"=");
		//LOG.Info(L"firstDoubleEqualIndex: " + std::to_wstring(firstDoubleEqualIndex));
		//LOG.Info(L"firstEqualIndex: " + std::to_wstring(firstEqualIndex));
		//默认使用==，如果==找不到，那就换成=
		std::wstring delimiter = L"==";
		if (firstDoubleEqualIndex == std::wstring::npos) {
			delimiter = L"=";
		}

		//找到了==或者找到了=都可以接受
		if (firstEqualIndex != std::wstring::npos || firstDoubleEqualIndex != std::wstring::npos) {
			std::vector<std::wstring> lowerReadLineSplitList = MMTString_SplitString(readLine, delimiter);
			if (lowerReadLineSplitList.size() < 2) {
				LOG.Error(L"lowerReadLineSplitList size is " + std::to_wstring(lowerReadLineSplitList.size()) + L",please check!");
			}
			std::wstring leftStr = lowerReadLineSplitList[0];
			std::wstring rightStr = lowerReadLineSplitList[1];
			//LOG.Info(L"leftStr:" + leftStr);
			//LOG.Info(L"rightStr:" + rightStr);
			LeftStr = leftStr;
			RightStr = rightStr;
			boost::algorithm::trim(leftStr);
			boost::algorithm::trim(rightStr);
			LeftStrTrim = leftStr;
			RightStrTrim = rightStr;
			valid = true;
		}
		else {
			LeftStr = L"";
			RightStr = L"";
			LeftStrTrim = L"";
			RightStrTrim = L"";
			valid = false;
		}
	}

	//下面这个留着指定分隔符，上面那个用来判断变量相关的时候
	IniLineObject(std::wstring readLine, std::wstring delimiter) {
		int firstEqualIndex = (int)readLine.find_first_of(delimiter);
		if (firstEqualIndex != std::wstring::npos) {
			std::vector<std::wstring> lowerReadLineSplitList = MMTString_SplitString(readLine, delimiter);
			std::wstring leftStr = lowerReadLineSplitList[0];
			std::wstring rightStr = lowerReadLineSplitList[1];
			LeftStr = leftStr;
			RightStr = rightStr;
			boost::algorithm::trim(leftStr);
			boost::algorithm::trim(rightStr);
			LeftStrTrim = leftStr;
			RightStrTrim = rightStr;
			valid = true;
		}
		else {
			LeftStr = L"";
			RightStr = L"";
			LeftStrTrim = L"";
			RightStrTrim = L"";
			valid = false;
		}
	}
};


class M_SectionLine {
public:
	std::wstring NameSpace;
	std::wstring SectionName;
	std::vector<std::wstring> SectionLineList;

	M_SectionLine() {}
};


//同时用于constants和present
class M_Variable {
public:
	std::wstring NameSpace;
	std::wstring VariableName;
	std::wstring InitializeValue = L"";
	std::wstring NamespacedVarName;
	std::wstring Type; //global local normal

	M_Variable() {

	}

	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType) {
		this->NameSpace = InNameSpace;
		this->VariableName = InVariableName;
		this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
		this->Type = InType;
	}

	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType) {
		this->NameSpace = InNameSpace;
		this->VariableName = InVariableName;
		this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
		this->InitializeValue = InInitializeValue;
		this->Type = InType;
	}
};


class M_Condition {
public:
	std::wstring NameSpace;
	std::wstring ConditionVarName;
	std::wstring ConditionVarValue;

	M_Condition() {}

	M_Condition(std::wstring InNameSpace, std::wstring InConditionVarName, std::wstring InConditionVarValue) {
		this->NameSpace = InNameSpace;
		this->ConditionVarName = InConditionVarName;
		this->ConditionVarValue = InConditionVarValue;
	}
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

	M_DrawIndexed() {}
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


class MigotoActiveMod {
public:

	//首先得有资源列表吧，ResourceList来根据资源名称读取对应磁盘上的资源
	std::vector<M_Resource> ResourceList;

	//其次得有Key列表吧
	std::vector<M_Key> KeyList;

};