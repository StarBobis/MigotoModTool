#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MigotoFormat.h"
#include <boost/algorithm/string.hpp>


std::vector<M_Variable> parseConstantsSection(M_SectionLine m_sectionLine) {
	std::vector<M_Variable> parsedVariableList;
	//LOG.LogOutput(L"Size: " + std::to_wstring(m_sectionLine.SectionLineList.size()));
	for (std::wstring readLine: m_sectionLine.SectionLineList) {
		LOG.Info(L"Parsing: " + readLine );
		//转换成小写方便对比（标准流程）
		std::wstring lowerReadLine = boost::algorithm::to_lower_copy(readLine);
		//去除前后空格方便对比（标准流程）
		boost::algorithm::trim(lowerReadLine);
		//SectionName之前已经解析出来了，顺便跳过（标准流程）
		if (lowerReadLine.starts_with(L"[")) {
			continue;
		}

		int variableIndex = (int)lowerReadLine.find(L"$");
		//检测$变量
		if (variableIndex != std::wstring::npos) {
			//找到变量就解析变量名称
			std::wstring variableAssignStr = lowerReadLine.substr(variableIndex);
			LOG.Info(L"[variableAssignStr]: " + variableAssignStr);
			//分两种情况，有初始值和没有初始值的
			int findEqualIndex = (int)variableAssignStr.find(L"=");
			if (findEqualIndex != std::wstring::npos) {
				//找到=则分割并设置名称和初始值
				std::vector<std::wstring> variableAssignSplitList = MMTString_SplitString(variableAssignStr,L"=");

				std::wstring variableName = variableAssignSplitList[0].substr(1);
				boost::algorithm::trim(variableName);

				std::wstring variableInitializeValue = variableAssignSplitList[1];
				boost::algorithm::trim(variableInitializeValue);
				if (lowerReadLine.starts_with(L"global")) {
					M_Variable m_variable(m_sectionLine.NameSpace, variableName, variableInitializeValue,L"global");
					parsedVariableList.push_back(m_variable);
				}
				else if(lowerReadLine.starts_with(L"local")){
					M_Variable m_variable(m_sectionLine.NameSpace, variableName, variableInitializeValue, L"local");
					parsedVariableList.push_back(m_variable);
				}
				else {
					M_Variable m_variable(m_sectionLine.NameSpace, variableName, variableInitializeValue, L"normal");
					parsedVariableList.push_back(m_variable);
				}
				
			}
			else {
				std::wstring variableName = boost::algorithm::trim_copy(variableAssignStr).substr(1);
				if (lowerReadLine.starts_with(L"global")) {
					M_Variable m_variable(m_sectionLine.NameSpace, variableName, L"global");
					parsedVariableList.push_back(m_variable);
				}
				else if (lowerReadLine.starts_with(L"local")) {
					M_Variable m_variable(m_sectionLine.NameSpace, variableName, L"local");
					parsedVariableList.push_back(m_variable);
				}
				else {
					M_Variable m_variable(m_sectionLine.NameSpace, variableName, L"normal");
					parsedVariableList.push_back(m_variable);
				}
				
			}
		}
		else {
			LOG.Info(L"[Skip] Can't find $ , Skip this line.");
		}
	}
	return parsedVariableList;
}


M_Key parseKeySection(M_SectionLine m_sectionLine) {
	M_Key m_key;

	for (std::wstring readLine : m_sectionLine.SectionLineList) {
		LOG.Info(L"Parsing: " + readLine);
		//转换成小写方便对比（标准流程）
		std::wstring lowerReadLine = boost::algorithm::to_lower_copy(readLine);
		//去除前后空格方便对比（标准流程）
		boost::algorithm::trim(lowerReadLine);
		//SectionName之前已经解析出来了，顺便跳过（标准流程）
		if (lowerReadLine.starts_with(L"[")) {
			continue;
		}
		
		if (lowerReadLine.starts_with(L"condition")) {
			IniLineObject conditionLine(lowerReadLine,L"=");
			if (conditionLine.valid) {
				IniLineObject conditionWorkLine(conditionLine.RightStrTrim,L"==");
				if (conditionWorkLine.valid) {
					M_Condition m_condition(m_sectionLine.NameSpace, 
						m_sectionLine.NameSpace + L"\\" + conditionWorkLine.LeftStrTrim.substr(1),
						conditionWorkLine.RightStrTrim
					);
					m_key.Condition = m_condition;
				}
				else if (conditionLine.RightStrTrim.starts_with(L"$")) {
					M_Condition m_condition(m_sectionLine.NameSpace,
						m_sectionLine.NameSpace + L"\\" + conditionLine.RightStrTrim.substr(1),
						L"1"
					);

					m_key.Condition = m_condition;
				}
				else {
					LOG.Error(L"Not a valid condition str : " + conditionLine.RightStrTrim);
				}

			}
		}
		else if (lowerReadLine.starts_with(L"key")) {
			IniLineObject keyLine(lowerReadLine, L"=");
			if (keyLine.valid) {
				m_key.KeyName = keyLine.RightStrTrim;
			}
		}
		else if (lowerReadLine.starts_with(L"back")) {
			IniLineObject backLine(lowerReadLine, L"=");
			if (backLine.valid) {
				m_key.BackName = backLine.RightStrTrim;
			}
		}
		else if (lowerReadLine.starts_with(L"type")) {
			IniLineObject typeLine(lowerReadLine, L"=");
			if (typeLine.valid) {
				m_key.Type = typeLine.RightStrTrim;
			}
		}
		else if (lowerReadLine.starts_with(L"$")) {
			IniLineObject varLine(lowerReadLine, L"=");
			if (varLine.valid) {
				std::wstring varName = m_sectionLine.NameSpace + L"\\" + varLine.LeftStrTrim.substr(1);
				std::vector<std::wstring> varValueList = MMTString_SplitString(varLine.RightStrTrim,L",");
				if (varValueList.size() == 1) {
					m_key.ActiveVariableName_ActiveValue_Map[varName] = varLine.RightStrTrim;
					LOG.Info(L"Parsed ActiveVariable, varName: " + varName + L"  varValue: " + varLine.RightStrTrim);
				}
				else {
					m_key.CycleVariableName_PossibleValueList_Map[varName] = varValueList;
					LOG.Info(L"Parsed CycleVariable, varName: " + varName + L"  cycleVarValue: " + varLine.RightStrTrim);

				}
			}
		}

	}

	return m_key;
}


M_TextureOverride parseTextureOverrideSection(M_SectionLine m_sectionLine) {
	M_TextureOverride m_textureoverride;
	m_textureoverride.NameSpace = m_sectionLine.NameSpace;
	bool isUnderLogic = false;

	//用于计算if和endif来判断是否退出了if
	int ifLevel = 0;
	std::vector<M_Condition> tmpActiveConditionList;
	std::wstring lastLogic = L"";


	std::vector<M_DrawIndexed> tmpDrawIndexedList;

	for (std::wstring readLine : m_sectionLine.SectionLineList) {
		LOG.Info(L"Parsing: " + readLine);
		//转换成小写方便对比（标准流程）
		std::wstring lowerReadLine = boost::algorithm::to_lower_copy(readLine);
		//去除前后空格方便对比（标准流程）
		boost::algorithm::trim(lowerReadLine);
		//SectionName之前已经解析出来了，顺便跳过（标准流程）
		if (lowerReadLine.starts_with(L"[")) {
			continue;
		}

		//TODO
		//所有的ini解析都要分为两种情况，一种是触发了自定义逻辑比如if endif的
		//另一种是不包含在if endif区域内的。
		//所以原版的3Dmigoto解析还是有点东西的，尝试一下能不能搬过来。
		

		if (lowerReadLine.starts_with(L"hash")) {
			IniLineObject hashLine(lowerReadLine, L"=");
			if (hashLine.valid) {
				m_textureoverride.IndexBufferHash = hashLine.RightStrTrim;
			}
		}
		else if (lowerReadLine.starts_with(L"if")) {
			//如果遇到if，又分为两种情况，一种是已经在if里，一种是没在if里
			//首先不论何时遇到if，肯定会if等级+1
			ifLevel++;
			LOG.Info(L"Meet if, IfLevel: " + std::to_wstring(ifLevel));

			//TODO 这里的解析是有问题的，没法处理使用&& 或者||的情况，需要抽象出单独的逻辑和变量解析。
			std::wstring conditionStr = lowerReadLine.substr(2);
			LOG.Info(L"Meet Condition: " + lowerReadLine);
			IniLineObject conditionLine(conditionStr);
			M_Condition m_condition;
			m_condition.NameSpace = m_sectionLine.NameSpace;
			m_condition.ConditionVarName = conditionLine.LeftStrTrim;
			m_condition.ConditionVarValue = conditionLine.RightStrTrim;
			tmpActiveConditionList.push_back(m_condition);
			lastLogic = L"if";
		}
		else if (lowerReadLine.starts_with(L"else if")) {
			//然后是遇到else if
			//如果上一个是endif的话，则正常处理但if的值得加回来，因为之前被剪掉了
			if (lastLogic == L"endif") {
				ifLevel++;
			}
			//如果上一个不是endif，那么不变
			else {
				
			}

			LOG.Info(L"Meet else if, IfLevel: " + std::to_wstring(ifLevel));

			std::wstring conditionStr = lowerReadLine.substr(2);
			LOG.Info(L"Meet Condition: " + lowerReadLine);
			IniLineObject conditionLine(conditionStr);
			
			M_Condition m_condition;
			m_condition.NameSpace = m_sectionLine.NameSpace;
			m_condition.ConditionVarName = conditionLine.LeftStrTrim;
			m_condition.ConditionVarValue = conditionLine.RightStrTrim;
			tmpActiveConditionList.push_back(m_condition);
			lastLogic = L"else if";
		}
		else if (lowerReadLine.starts_with(L"endif")) {
			ifLevel--;
			LOG.Info(L"Meet endif, IfLevel: " + std::to_wstring(ifLevel));
			// 如果if等级为0，说明结束了if，说明之前识别到的时候已经把条件列表添加进去了，
			// 那就没事可以清除了
			if (ifLevel == 0) {
				tmpActiveConditionList.clear();
			}
			lastLogic = L"endif";
		}
		else if (lowerReadLine.starts_with(L"match_first_index")) {
			IniLineObject match_first_index_line_object(lowerReadLine, L"=");
			if (match_first_index_line_object.valid) {
				m_textureoverride.MatchFirstIndex = match_first_index_line_object.RightStrTrim;
			}
		}
		else if (lowerReadLine.starts_with(L"ib")) {
			IniLineObject ib_line_object(lowerReadLine, L"=");
			if (ib_line_object.valid) {
				M_ResourceReplace m_resource_replace;
				m_resource_replace.ReplaceTarget = ib_line_object.LeftStrTrim;
				m_resource_replace.ReplaceResource = ib_line_object.RightStrTrim;
				if (ifLevel != 0) {
					m_resource_replace.ActiveConditionList = tmpActiveConditionList;
				}
				m_textureoverride.ResourceReplaceList.push_back(m_resource_replace);
			}
		}
		else if (lowerReadLine.starts_with(L"drawindexed")) {
			IniLineObject drawIndexedLine(lowerReadLine,L"=");
			if (drawIndexedLine.valid) {
				std::wstring drawParamStr = drawIndexedLine.RightStrTrim;
				std::vector <std::wstring> drawParams = MMTString_SplitString(drawParamStr,L",");

				//for (std::wstring xxx:drawParams) {
				//	LOG.Info(xxx);
				//}
				//LOG.NewLine();

				M_DrawIndexed m_draw_indexed;

				if (drawParams.size() >= 3) {
					m_draw_indexed.AutoDraw = false;
					m_draw_indexed.DrawNumber = drawParams[0];
					boost::algorithm::trim(m_draw_indexed.DrawNumber);
					//LOG.Info(m_draw_indexed.DrawNumber);

					m_draw_indexed.DrawOffsetIndex = drawParams[1];
					//boost::algorithm::trim(m_draw_indexed.DrawOffsetIndex);
					//LOG.Info(m_draw_indexed.DrawOffsetIndex);

					m_draw_indexed.DrawStartIndex = drawParams[2];
					//boost::algorithm::trim(m_draw_indexed.DrawStartIndex);
					//LOG.Info(m_draw_indexed.DrawStartIndex);

					//LOG.Error(L"Stop");

				}
				else {
					LOG.Info(L"Commandlist drawindexed parameter is not enough to 3.");
					if (drawParamStr == L"auto") {
						m_draw_indexed.AutoDraw = true;
					}
				}
				if (ifLevel != 0) {
					m_draw_indexed.ActiveConditionList = tmpActiveConditionList;
				}
				else {
					LOG.Info(L"IfLevel: " + std::to_wstring(ifLevel));
				}
				tmpDrawIndexedList.push_back(m_draw_indexed);
			}
			else {
				LOG.Warning(L"Not a valid [drawindexed]");
			}
		}else if (lowerReadLine.starts_with(L"ps-")) {
			IniLineObject pixelSlotReplaceLine(lowerReadLine, L"=");
			if (pixelSlotReplaceLine.valid) {
				M_ResourceReplace m_resource_replace;
				m_resource_replace.ReplaceTarget = pixelSlotReplaceLine.LeftStrTrim;
				m_resource_replace.ReplaceResource = pixelSlotReplaceLine.RightStrTrim;
				if (ifLevel != 0) {
					m_resource_replace.ActiveConditionList = tmpActiveConditionList;
				}
				m_textureoverride.ResourceReplaceList.push_back(m_resource_replace);
			}
		}


	}
	m_textureoverride.DrawIndexedList = tmpDrawIndexedList;
	LOG.Info(L"DrawIndexedList Size : " + std::to_wstring(m_textureoverride.DrawIndexedList.size()));
	return m_textureoverride;
}


M_Resource parseResourceSection(M_SectionLine m_sectionLine) {
	M_Resource m_resource;
	m_resource.ResourceName = m_sectionLine.SectionName;
	m_resource.NameSpace = m_sectionLine.NameSpace;

	for (std::wstring readLine : m_sectionLine.SectionLineList) {
		LOG.Info(L"Parsing: " + readLine);
		//转换成小写方便对比（标准流程）
		std::wstring lowerReadLine = boost::algorithm::to_lower_copy(readLine);
		//去除前后空格方便对比（标准流程）
		boost::algorithm::trim(lowerReadLine);
		//SectionName之前已经解析出来了，顺便跳过（标准流程）
		if (lowerReadLine.starts_with(L"[")) {
			
			continue;
		}

		if (lowerReadLine.starts_with(L"type")) {
			IniLineObject typeLine(lowerReadLine,L"=");
			if (typeLine.valid) {
				m_resource.Type = typeLine.RightStrTrim;
			}
		}else if (lowerReadLine.starts_with(L"stride")) {
			IniLineObject typeLine(lowerReadLine, L"=");
			if (typeLine.valid) {
				m_resource.Stride = typeLine.RightStrTrim;
			}
		}else if (lowerReadLine.starts_with(L"filename")) {
			//这里注意用的是原本的行，因为文件名是区分大小写的。
			IniLineObject typeLine(readLine, L"=");
			if (typeLine.valid) {
				m_resource.FileName = typeLine.RightStrTrim;
				LOG.Info(L"Detect [filename]: " + m_resource.FileName);
			}else{
				LOG.Error(L"Can't parse [filename] ");

			}
		}
		else if (lowerReadLine.starts_with(L"format")) {
			IniLineObject formatLine(lowerReadLine, L"=");
			if (formatLine.valid) {
				m_resource.Format = formatLine.RightStrTrim;
			}
		}
		//TODO

	}

	return m_resource;
}


