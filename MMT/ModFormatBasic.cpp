#include "ModFormatBasic.h"
#include <iostream>


//--------------------------------------------------------------------------------------------------------------------
M_Condition::M_Condition() {

}


//传入if或者else if后面的条件判断部分字符串，随后解析出其中涉及的变量和逻辑
M_Condition::M_Condition(std::wstring ConditionStr) {
	LOG.NewLine();
	LOG.Info(L"Start to parse condition expression for: " + ConditionStr);
	//首先查找是否含有&& 或者 || 这种类型符号，如果找到了那就按多个进行处理，否则按单个进行处理
	bool singleExpression = true;
	if (ConditionStr.find(L"&&") != std::wstring::npos) {
		singleExpression = false;
	}
	else if (ConditionStr.find(L"||") != std::wstring::npos) {
		singleExpression = false;
	}

	if (singleExpression) {
		LOG.Info("Can't find && or || in ConditionStr, take it as a single expression condition.");
		//首先偷懒，使用= 或 ==分割，如果分割出来的ConditionVarValue里没有再出现== !=或者变量$这类的东西，说明正常运作
		IniLineObject conditionLine(ConditionStr);
		this->Condition_VarName_VarValue_Map[conditionLine.LeftStrTrim] = conditionLine.RightStrTrim;
	}
	else {
		LOG.Info("find && or || in ConditionStr, take it as a multiple expression condition.");

	}
    

    //检查ConditionVarValue中是否含有变量名，逻辑符号等，进行更复杂的解析判断。
	LOG.Info(L"Parse condition expression over.");
	LOG.NewLine();
}

void M_Condition::show() {

}


//--------------------------------------------------------------------------------------------------------------------
IniLineObject::IniLineObject() {

}

IniLineObject::IniLineObject(std::wstring readLine) {
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
IniLineObject::IniLineObject(std::wstring readLine, std::wstring delimiter) {
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


//--------------------------------------------------------------------------------------------------------------------
M_SectionLine::M_SectionLine() {

}


//--------------------------------------------------------------------------------------------------------------------
M_Variable::M_Variable() {

}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->Type = InType;
}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->InitializeValue = InInitializeValue;
	this->Type = InType;
}


//--------------------------------------------------------------------------------------------------------------------
M_DrawIndexed::M_DrawIndexed() {

}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------