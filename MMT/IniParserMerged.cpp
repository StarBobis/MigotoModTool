#include "IniParser.h"
#include "MMTStringUtils.h"
#include <boost/algorithm/string.hpp>
#include <set>
#include <unordered_map>
#include "MMTFileUtils.h"

std::vector<std::wstring> cleanAllLineForIniParse(std::vector<std::wstring> lineList) {
	std::vector<std::wstring> newLineList;
	for (std::wstring line : lineList) {
		std::wstring tmpLine = line;
		if (boost::algorithm::starts_with(tmpLine, L";")) {
			continue;
		}
		boost::algorithm::to_lower(tmpLine);
		boost::algorithm::trim(tmpLine);
		boost::algorithm::erase_all(tmpLine, L" ");
		boost::algorithm::erase_all(tmpLine, L"\t");
		boost::algorithm::erase_all(tmpLine, L"\r");
		boost::algorithm::erase_all(tmpLine, L"\n");
		if (tmpLine != L"") {
			newLineList.push_back(tmpLine);
		}
	}
	return newLineList;
}


std::unordered_map<std::wstring, std::vector<ResourceReplace>> parseCommandResourceReplaceListDict(std::vector<std::wstring> lineList) {
	std::unordered_map<std::wstring, std::vector<ResourceReplace>> commandResourceReplaceListDict;

	bool isInCommandlistSection = false;
	bool isInIfSection = false;

	int ifLevelCount = 0;

	std::vector<ResourceReplace> tmpResourceReplaceList;

	std::unordered_map<int, Condition> levelCountConditionDict;

	Condition tmpCondition;
	std::wstring tmpCommandListName = L"";

	for (std::wstring line : lineList) {
		//LOG.LogOutput(line);
		if (boost::algorithm::starts_with(line,L"[") && boost::algorithm::ends_with(line, L"]") and isInCommandlistSection) {
			isInCommandlistSection = false;
			ifLevelCount = 0;
			//LOG.LogOutput(L"[End] CommandList Section");
			commandResourceReplaceListDict[tmpCommandListName] = tmpResourceReplaceList;
			tmpResourceReplaceList.clear();
			//LOG.LogOutputSplitStr();
		}

		if (boost::algorithm::starts_with(line, L"[commandlist") && boost::algorithm::ends_with(line, L"]") and !isInCommandlistSection) {
			isInCommandlistSection = true;
			tmpCommandListName = line.substr(1,(line.length() -1) -1);
			//LOG.LogOutput(L"[Start] CommandList Section");
			//LOG.LogOutput(L"tmpCommandListName: " + tmpCommandListName);

		}

		if (isInCommandlistSection) {

			if (boost::algorithm::starts_with(line, L"endif")) {
				isInIfSection = false;
				//LOG.LogOutput(L"[End] If Section");

				auto it = levelCountConditionDict.find(ifLevelCount);
				if (it != levelCountConditionDict.end()) {
					levelCountConditionDict.erase(it);
				}

				ifLevelCount--;
				//LOG.LogOutput(L"Level Decrease To : " + std::to_wstring(ifLevelCount));

				if (ifLevelCount > 0) {
					tmpCondition = levelCountConditionDict[ifLevelCount];

					//LOG.LogOutput(L"TmpCOnditionName After level decrease: " + tmpCondition.ConditionName);
				}
			}

			if (boost::algorithm::starts_with(line, L"if")) {
				//LOG.LogOutput(L"[Start] If Section");
				//LOG.LogOutput(L"[Generate new Condition]:");
				isInIfSection = true;
				ifLevelCount++;
				tmpCondition = Condition();
				tmpCondition.ConditionName = line.substr(2,line.length()-2);
				tmpCondition.ConditionLevel = ifLevelCount;
				tmpCondition.Positive = true;
				//tmpCondition.show();
				levelCountConditionDict[tmpCondition.ConditionLevel] = tmpCondition;
			}

			if (boost::algorithm::starts_with(line, L"elseif")) {
				//LOG.LogOutput(L"[Detect] else if Section");
				//LOG.LogOutput(L"[Generate new Condition]:");
				tmpCondition = Condition();
				tmpCondition.ConditionName = line.substr(6, line.length() - 6);
				tmpCondition.ConditionLevel = ifLevelCount;
				tmpCondition.Positive = true;
				//tmpCondition.show();
				levelCountConditionDict[tmpCondition.ConditionLevel] = tmpCondition;
			}

			if (boost::algorithm::starts_with(line, L"else") && !boost::algorithm::starts_with(line, L"elseif")) {
				tmpCondition.Positive = false;
				//LOG.LogOutput(L"[Detect] else Section");
				//tmpCondition.show();
				levelCountConditionDict[tmpCondition.ConditionLevel] = tmpCondition;
			}

			if (isInIfSection) {
				if (boost::algorithm::contains(line,"=resource")) {
					//LOG.LogOutput(L"[Detect] ResourceReplace" + line);
					ResourceReplace tmpResourceReplace;
					tmpResourceReplace.OriginalLine = line;

					for (const auto& pair: levelCountConditionDict) {
						int levelCount = pair.first;
						Condition condition = pair.second;
						if (condition.ConditionName == L"") {
							LOG.Error(L"ConditionName is empty when parsing!");
						}
						//condition.show();
						tmpResourceReplace.ConditionList.push_back(condition);
					}

					tmpResourceReplaceList.push_back(tmpResourceReplace);
				}
			}

		}
	}
	
	if (isInCommandlistSection) {
		isInCommandlistSection = false;
		//LOG.LogOutput(L"[End] CommandList Section");
		commandResourceReplaceListDict[tmpCommandListName] = tmpResourceReplaceList;
		//LOG.LogOutputSplitStr();
	}

	for (const auto& pair: commandResourceReplaceListDict) {
		std::wstring commandName = pair.first;
		std::vector<ResourceReplace> resourceReplaceList = pair.second;
		//LOG.LogOutput(L"commandName: " + commandName);
		for (ResourceReplace resourceReplace: resourceReplaceList) {
			//resourceReplace.show();
			for (Condition condition:resourceReplace.ConditionList) {
				if (condition.ConditionName == L"") {
					LOG.Error(L"Empty ConditionName!");
				}
			}
		}
		//LOG.LogOutputSplitStr();
	}

	return commandResourceReplaceListDict;
}


std::vector<TextureOverride> parseTextureOverrideList(std::vector<std::wstring> lineList) {
	std::vector<TextureOverride> textureOverrideList;
	bool isInTextureOverride = false;
	TextureOverride tmpTextureOverride;
	for (std::wstring line : lineList) {
		if (boost::algorithm::starts_with(line, L"[") && boost::algorithm::ends_with(line, L"]") and isInTextureOverride) {
			isInTextureOverride = false;
			//LOG.LogOutput(L"[End] TextureOverride Section");
			textureOverrideList.push_back(tmpTextureOverride);
			//LOG.LogOutputSplitStr();
		}

		if (boost::algorithm::starts_with(line, L"[textureoverride") && boost::algorithm::ends_with(line, L"]") and !isInTextureOverride) {
			isInTextureOverride = true;
			tmpTextureOverride = TextureOverride();
			tmpTextureOverride.SectionName = line.substr(1, (line.length() - 1) - 1);
			//LOG.LogOutput(L"[Start] TextureOverride Section");
			//LOG.LogOutput(L"SectionName: " + tmpTextureOverride.SectionName);

		}

		if (isInTextureOverride) {
			if (boost::algorithm::starts_with(line, L"hash=")) {
				tmpTextureOverride.HashValue = line.substr(5, line.length() - 5);
				//LOG.LogOutput(L"Detect HashValue: " + tmpTextureOverride.HashValue);
			}

			if (boost::algorithm::starts_with(line, L"match_first_index=")) {
				tmpTextureOverride.MatchFirstIndex = line.substr(18, line.length() - 18);
				//LOG.LogOutput(L"Detect matchFirstIndex: " + tmpTextureOverride.MatchFirstIndex);
			}

			if (boost::algorithm::starts_with(line, L"run=commandlist")) {
				std::wstring tmpCommand = line.substr(4, line.length() - 4);
				tmpTextureOverride.CommandList.push_back(tmpCommand);
				//LOG.LogOutput(L"Detect command: " + tmpCommand);
			}

			if (boost::algorithm::starts_with(line, L"$") && boost::algorithm::contains(line, "=")) {
				tmpTextureOverride.ActiveConditionStr = line;
				//LOG.LogOutput(L"Detect active condition: " + tmpTextureOverride.ActiveConditionStr);
			}

		}
	}

	if (isInTextureOverride) {
		//LOG.LogOutput(L"[End] TextureOverride Section");
		textureOverrideList.push_back(tmpTextureOverride);
		//LOG.LogOutputSplitStr();
	}

	return textureOverrideList;
}


std::vector<TextureOverride> matchActiveResourceReplace(
	std::vector<TextureOverride> textureOverrideList, 
	std::unordered_map<std::wstring, std::vector<ResourceReplace>> commandResourceReplaceListDict) {
	LOG.NewLine();
	LOG.Info(L"[Start to match command's ResourceReplace]");
	std::vector<TextureOverride> matchedResourceReplaceTextureOverrideList;
	for (TextureOverride textureOverride : textureOverrideList) {
		//LOG.LogOutput(L"[Match] texture override name: " + textureOverride.SectionName);

		std::vector<ResourceReplace> allResourceReplaceList;
		for (std::wstring command : textureOverride.CommandList) {
			if (!commandResourceReplaceListDict.contains(command)) {
				//LOG.LogOutput(L"Don't have command:"+ command + L" for this textureoverride: " + textureOverride.SectionName);
				continue;
			}
			//LOG.LogOutput(L"Detect command: " + command);
			std::vector<ResourceReplace> resourceReplaceList = commandResourceReplaceListDict[command];
			for (ResourceReplace resourceReplace : resourceReplaceList) {
				allResourceReplaceList.push_back(resourceReplace);
				//resourceReplace.show();
			}
		}
		textureOverride.ResourceReplaceList = allResourceReplaceList;
		matchedResourceReplaceTextureOverrideList.push_back(textureOverride);
	}
	//LOG.LogOutputSplitStr();

	//LOG.LogOutput(L"[Start to match activated ResourceReplace]");
	//LOG.LogOutput(L"Size: " + std::to_wstring(matchedResourceReplaceTextureOverrideList.size()));
	//LOG.LogOutputSplitStr();

	std::vector<TextureOverride> matchedActivatedResourceReplaceTextureOverrideList;

	for (TextureOverride textureOverride : matchedResourceReplaceTextureOverrideList) {
		//LOG.LogOutput(L"TextureOverrideName: " + textureOverride.SectionName);
		//LOG.LogOutput(L"ActivateCondition: " + textureOverride.ActiveConditionStr);

		if (textureOverride.ActiveConditionStr != L"") {
			std::vector<std::wstring> activeConditionSplits;
			boost::split(activeConditionSplits, textureOverride.ActiveConditionStr, boost::is_any_of(L"="));
			std::wstring activeConditionVarName = activeConditionSplits[0];
			std::wstring activeConditionVarValue = activeConditionSplits[1];

			bool findActive = false;
			for (ResourceReplace resourceReplace: textureOverride.ResourceReplaceList) {
				for (Condition condition:resourceReplace.ConditionList) {
					std::vector<std::wstring> conditionSplits = MMTString_SplitString(condition.ConditionName, L"==");
					
					std::wstring conditionVarName = conditionSplits[0];

					if (conditionVarName == activeConditionVarName) {
						LOG.Info(L"activeConditionVarName: " + activeConditionVarName + L" current conditionVarName: " + conditionVarName);
						findActive = true;
						break;
					}
				}
				if (findActive) {
					break;
				}
			}

			if (!findActive) {
				//LOG.LogOutput(L"Can't find active flag in resourceReplace list, set ActiveResourceReplaceList to ResourceReplaceList ");
				textureOverride.ActiveResourceReplaceList = textureOverride.ResourceReplaceList;
				//LOG.LogOutput(textureOverride.SectionName + L" Size:" + std::to_wstring(textureOverride.ActiveResourceReplaceList.size()));
				matchedActivatedResourceReplaceTextureOverrideList.push_back(textureOverride);
				continue;
			}

			//LOG.LogOutput(L"--------------------find activate flag in : " + textureOverride.SectionName);
			std::vector<ResourceReplace> activeResourceReplaceList;
			for (ResourceReplace resourceReplace: textureOverride.ResourceReplaceList) {
				//LOG.LogOutput(L"resource replace name: " + resourceReplace.OriginalLine);

				bool findActiveVar = false;
				for (Condition condition : resourceReplace.ConditionList) {
					if (condition.ConditionName == L"") {
						continue;
					}
					std::vector<std::wstring> conditionSplits = MMTString_SplitString(condition.ConditionName, L"==");
					std::wstring conditionVarName = conditionSplits[0];
					std::wstring conditionVarValue = conditionSplits[1];

					if (conditionVarName == activeConditionVarName) {
						findActiveVar = true;
					}
				}
				if (!findActiveVar) {
					LOG.Info(L"Can't find activeVar so Activate this Resource directly: " + resourceReplace.OriginalLine);
					activeResourceReplaceList.push_back(resourceReplace);
				}
				else {
					bool resourceReplaceActivated = false;
					for (Condition condition : resourceReplace.ConditionList) {
						if (condition.ConditionName == L"") {
							continue;
						}
						std::vector<std::wstring> conditionSplits = MMTString_SplitString(condition.ConditionName, L"==");
						std::wstring conditionVarName = conditionSplits[0];
						std::wstring conditionVarValue = conditionSplits[1];

						if (conditionVarName == activeConditionVarName) {
							//LOG.LogOutput(conditionVarName);
							//LOG.LogOutput(activeConditionVarName);
							//LOG.LogOutput(L"Meet activate var: " + conditionVarName);
							if (condition.Positive && conditionVarValue == activeConditionVarValue) {
								LOG.Info(resourceReplace.OriginalLine + L" Activated!");
								resourceReplaceActivated = true;
								break;
							}
							else if (!condition.Positive && conditionVarValue != activeConditionVarValue) {
								LOG.Info(resourceReplace.OriginalLine + L" Activated! (reverse condition)");
								resourceReplaceActivated = true;
								break;
							}

						}
					}

					if (resourceReplaceActivated) {
						LOG.Info(L"Activate Resource: " + resourceReplace.OriginalLine);
						activeResourceReplaceList.push_back(resourceReplace);
					}

				}
			}

			textureOverride.ActiveResourceReplaceList = activeResourceReplaceList;

		}
		else {
			LOG.Info(L"ActiveResourceReplaceList is ResourceReplaceList!");
			textureOverride.ActiveResourceReplaceList = textureOverride.ResourceReplaceList;
		}

		LOG.Info(textureOverride.SectionName + L" Size:" + std::to_wstring(textureOverride.ActiveResourceReplaceList.size()));
		matchedActivatedResourceReplaceTextureOverrideList.push_back(textureOverride);

	}

	LOG.Info(L" match activated resource replace over ");
	LOG.NewLine();
	return matchedActivatedResourceReplaceTextureOverrideList;
}


std::vector<CycleKey> parseCycleKeyList(std::vector<std::wstring> lineList) {
	std::vector<CycleKey> cycleKeyList;
	bool isInCycleKeySection = false;

	CycleKey tmpCycleKey;
	for (std::wstring line : lineList) {
		if (boost::algorithm::starts_with(line, L"[") && boost::algorithm::ends_with(line, L"]") and isInCycleKeySection) {
			isInCycleKeySection = false;
			cycleKeyList.push_back(tmpCycleKey);
		}

		if (boost::algorithm::starts_with(line, L"[key") && boost::algorithm::ends_with(line, L"]") and !isInCycleKeySection) {
			isInCycleKeySection = true;
			tmpCycleKey = CycleKey();
		}

		if (isInCycleKeySection) {
			if (boost::algorithm::starts_with(line, L"key=")) {
				tmpCycleKey.Key = line.substr(4, line.length() - 4);
			}

			if (boost::algorithm::starts_with(line, L"$") && boost::algorithm::contains(line,L"=") && boost::algorithm::contains(line, L",")) {
				std::vector<std::wstring> varSplits;
				boost::split(varSplits, line, boost::is_any_of(L"="));
				std::wstring varName = varSplits[0];
				std::wstring varValue = varSplits[1];
				tmpCycleKey.VarName = varName;

				std::vector<std::wstring> varValueSplits;
				boost::split(varValueSplits, varValue, boost::is_any_of(L","));
				tmpCycleKey.VarValueList = varValueSplits;

			}
		}

	}

	if (isInCycleKeySection) {
		cycleKeyList.push_back(tmpCycleKey);
	}

	return cycleKeyList;
}


std::unordered_map<std::wstring, ModResource> parseResourceDict(std::vector<std::wstring> lineList,std::wstring iniLocationFolder,std::wstring filePath) {
	bool isInResourceSection = false;
	std::vector<ModResource> resourceList;
	ModResource tmpResource;

	for (std::wstring line : lineList) {
		if (boost::algorithm::starts_with(line, L"[") && boost::algorithm::ends_with(line, L"]") and isInResourceSection) {
			isInResourceSection = false;
			resourceList.push_back(tmpResource);
		}

		if (boost::algorithm::starts_with(line, L"[resource") && boost::algorithm::ends_with(line, L"]") and !isInResourceSection) {
			isInResourceSection = true;
			tmpResource = ModResource();
			tmpResource.SectionName = line.substr(1,line.length() -1 -1);
		}

		if (isInResourceSection) {
			if (boost::algorithm::starts_with(line, L"format=")) {
				tmpResource.Format = line.substr(7,line.length() - 7);
			}

			if (boost::algorithm::starts_with(line, L"stride=")) {
				tmpResource.Stride = std::stoi(line.substr(7, line.length() - 7));
			}

			if (boost::algorithm::starts_with(line, L"filename=")) {
				tmpResource.FileName = line.substr(9, line.length() - 9);
			}
		}
	}
	
	if (isInResourceSection) {
		resourceList.push_back(tmpResource);
	}


	std::vector<std::wstring> lines = MMTFile_ReadAllLinesW(filePath);
	std::vector<ModResource> resourceAllList = parseResourceAllList(lines, iniLocationFolder);

	std::unordered_map<std::wstring, ModResource> resourceDict;
	for (ModResource resource: resourceList) {
		if (resource.FileName == L"") {
			resource.Type = L"Container";
		}
		else {
			if (resource.Stride != 0) {
				resource.Type = L"VB";
			}
			else {
				if (resource.Format != L"") {
					resource.Type = L"IB";
				}
				else {
					resource.Type = L"Texture";
				}
			}
		}


		for (ModResource resourceVB: resourceAllList) {
			std::wstring resourceName = resourceVB.SectionName;
			boost::algorithm::to_lower(resourceName);
			if (resourceName == resource.SectionName) {
				resource.FileName = resourceVB.FileName;
				break;
			}
		}
		resourceDict[resource.SectionName] = resource;
	}
	return resourceDict;
}


void cartesianProductHelper(const std::vector<std::vector<std::wstring>>& data, std::vector<std::wstring>& current, std::vector<std::vector<std::wstring>>& result, size_t index) {
	if (index >= data.size()) {
		result.push_back(current);
		return;
	}

	for (const auto& str : data[index]) {
		current.push_back(str);
		cartesianProductHelper(data, current, result, index + 1);
		current.pop_back();
	}
}


std::vector<std::vector<std::wstring>> cartesianProduct(const std::vector<std::vector<std::wstring>>& data) {
	std::vector<std::vector<std::wstring>> result;
	std::vector<std::wstring> current;
	cartesianProductHelper(data, current, result, 0);
	return result;
}


std::vector<std::unordered_map<std::wstring, std::wstring>> calculateKeyCombination(std::vector<CycleKey> cycleKeyList) {
	LOG.Info(L"Start to calculateKeyCombination");
	std::vector<std::unordered_map<std::wstring, std::wstring>> keyCombinationDictList;

	std::vector<std::vector<std::wstring>> varValuesList;
	std::vector<std::wstring> varNameList;

	for (CycleKey cycleKey: cycleKeyList) {

		if (cycleKey.VarValueList.size() != 0) {
			LOG.Info(L"Key: " + cycleKey.Key);

			std::vector<std::wstring> trueVarValueList;
			for (std::wstring varValue : cycleKey.VarValueList) {
				LOG.Info(L"Value: " + varValue);
				trueVarValueList.push_back(varValue);
			}
			varValuesList.push_back(trueVarValueList);
			varNameList.push_back(cycleKey.VarName);
		}
	}

	std::vector<std::vector<std::wstring>> cartesianProductList = cartesianProduct(varValuesList);
	for (std::vector<std::wstring> keyCombinationValueList: cartesianProductList) {
		std::unordered_map<std::wstring, std::wstring> tmpDict;
		for (int i = 0; i < keyCombinationValueList.size();i++) {
			std::wstring cycleVarValue = keyCombinationValueList[i];
			std::wstring cycleVarName = varNameList[i];
			//LOG.LogOutput(cycleVarName + L" " + cycleVarValue);
			tmpDict[cycleVarName] = cycleVarValue;
		}
		keyCombinationDictList.push_back(tmpDict);
		//LOG.LogOutputSplitStr();
	}
	//LOG.LogOutputSplitStr();
	return keyCombinationDictList;
}


std::vector<CycleKey> parseCycleKeyListFromActiveResourceReplace(std::vector<TextureOverride> newTextureOverrideList) {

	std::unordered_map<std::wstring, std::vector<std::wstring>> keyNameKeyValuesDict;
	for (TextureOverride textureOverride: newTextureOverrideList) {
		LOG.Info(L"textureOverride"+ textureOverride.SectionName +  L"'s activeResourceReplaceList size: " +  std::to_wstring(textureOverride.ActiveResourceReplaceList.size()));
		for (ResourceReplace resourceReplace: textureOverride.ActiveResourceReplaceList) {
			for (Condition condition: resourceReplace.ConditionList) {
				std::wstring conditionName = condition.ConditionName;
				std::vector<std::wstring> conditionNameSplit = MMTString_SplitString(conditionName,L"==");
				std::wstring varName = conditionNameSplit[0];
				std::wstring varValue = conditionNameSplit[1];

				if (keyNameKeyValuesDict.contains(varName)) {
					std::vector<std::wstring> varValueList = keyNameKeyValuesDict[varName];
					varValueList.push_back(varValue);
					keyNameKeyValuesDict[varName] = varValueList;
				}
				else {
					std::vector<std::wstring> varValueList;
					varValueList.push_back(varValue);
					keyNameKeyValuesDict[varName] = varValueList;
				}

			}
		}
	}

	std::vector<CycleKey> cycleKeyList;
	for (const auto& pair: keyNameKeyValuesDict) {
		std::wstring keyName = pair.first;
		std::vector<std::wstring> keyValues = pair.second;
		CycleKey cycleKey;
		cycleKey.Key = L"";
		cycleKey.VarName = keyName;

		std::set<std::wstring> uniqueSet;
		for (std::wstring varValue: keyValues) {
			uniqueSet.insert(varValue);
		}
		cycleKey.VarValueList = std::vector<std::wstring>(uniqueSet.begin(), uniqueSet.end());
		cycleKeyList.push_back(cycleKey);
		
	}

	LOG.Info(L"Size for cycleKeyList.size(): " + std::to_wstring(cycleKeyList.size()));
	LOG.NewLine();


	return cycleKeyList;
}


void parseMergedInI(std::wstring filePath,std::wstring gameName,std::wstring ApplicationLocation) {
	std::wstring iniLocationFolder = MMTString_GetFolderPathFromFilePath(filePath);
	
	std::vector<std::wstring> lineList = MMTFile_ReadAllLinesW(filePath);
	std::vector<std::wstring> cleanLineList = cleanAllLineForIniParse(lineList);
	std::unordered_map<std::wstring, std::vector<ResourceReplace>> commandResourceReplaceListDict = parseCommandResourceReplaceListDict(cleanLineList);
	std::vector<TextureOverride> textureOverrideList = parseTextureOverrideList(cleanLineList);
	LOG.Info(L"Start to matchActiveResourceReplace: ");
	std::vector<TextureOverride> newTextureOverrideList = matchActiveResourceReplace(textureOverrideList, commandResourceReplaceListDict);

	//std::vector<CycleKey> cycleKeyList = parseCycleKeyList(cleanLineList);
	LOG.Info(L"Start to parseCycleKeyListFromActiveResourceReplace: ");
	std::vector<CycleKey> cycleKeyList = parseCycleKeyListFromActiveResourceReplace(newTextureOverrideList);
	std::unordered_map<std::wstring,CycleKey> cycleKeyDict;
	for (CycleKey cycleKey: cycleKeyList) {
		cycleKeyDict[cycleKey.VarName] = cycleKey;
		//cycleKey.show();
		LOG.Info(cycleKey.VarName);
	}


	std::unordered_map<std::wstring, ModResource> resourceDict = parseResourceDict(cleanLineList, iniLocationFolder, filePath);

	std::vector<std::unordered_map<std::wstring, std::wstring>> keyCombinationDictList = calculateKeyCombination(cycleKeyList);
	LOG.NewLine();


	LOG.Info(L"Start to combine and output reversed model");
	
	LOG.Info(L"Combination Size: " + std::to_wstring(keyCombinationDictList.size()));
	for (std::unordered_map<std::wstring, std::wstring>  keyCombinationDict : keyCombinationDictList) {
		std::wstring keyCombineStr = L"";

		for (const auto&pair: keyCombinationDict) {
			std::wstring keyName = pair.first;
			std::wstring keyValue = pair.second;
			keyCombineStr = keyCombineStr + keyName + keyValue + L"_";
		}
		LOG.Info(L"Current Processing: " + keyCombineStr);

		std::wstring outputPath = iniLocationFolder + L"\\" + keyCombineStr + L"\\";
		std::filesystem::create_directories(outputPath);


		LOG.Info(L"Start to calculate textureoverride's resource replace:");
		std::vector<TextureOverride> keyActivatedTextureOverrideList;
		for (TextureOverride textureOverride: newTextureOverrideList) {
			bool atleasetOneMatchResourceReplace = false;
			
			std::vector<ResourceReplace> keyActivatedResourceReplaceList;
			for (ResourceReplace resourceReplace: textureOverride.ActiveResourceReplaceList) {
				
				bool allMatchCondition = true;
				for (Condition condition: resourceReplace.ConditionList) {
					if (condition.ConditionName == L"") {
						continue;
					}

					std::vector<std::wstring> varSplits = MMTString_SplitString(condition.ConditionName, L"==");
					std::wstring conditionVarName = varSplits[0];
					std::wstring conditionVarValue = varSplits[1];
					
					if (!keyCombinationDict.contains(conditionVarName)) {
						continue;
					}

					if (conditionVarValue != keyCombinationDict[conditionVarName]) {
						CycleKey cycleKey = cycleKeyDict[conditionVarName];
						if (cycleKey.VarValueList.size() ==2 && condition.Positive == false) {
							//LOG.LogOutput(conditionVarValue);
							//LOG.LogOutput(keyCombinationDict[conditionVarName]);
							continue;
						}else{
							allMatchCondition = false;
							break;
						}
					}
					else {
						if (!condition.Positive) {
							allMatchCondition = false;
							break;
						}
					}


				}

				if (allMatchCondition) {
					LOG.Info(resourceReplace.OriginalLine + L" Activated!");
					resourceReplace.show();
					atleasetOneMatchResourceReplace = true;
					keyActivatedResourceReplaceList.push_back(resourceReplace);
				}
				else {
					//LOG.LogOutput(L"ResourceReplace didn't activated: " + resourceReplace.OriginalLine);
				}

				

			}

			if (atleasetOneMatchResourceReplace) {
				LOG.Info(textureOverride.SectionName);
				LOG.Info (L"Match Success!");
				LOG.NewLine();
				textureOverride.ActiveResourceReplaceList = keyActivatedResourceReplaceList;
				keyActivatedTextureOverrideList.push_back(textureOverride);
			}
			else {

			}

		}
		LOG.NewLine();


		if (keyActivatedTextureOverrideList.size() == 0) {
			LOG.Error(L"Can't find any TextureOverride with ResourceReplace Activated!");
		}


		std::vector<TextureOverrideIB> textureOverrideIBList;
		std::vector<ModResource> resourceVBList;
		std::vector<ModResource> resourceIBList;
		LOG.Info(L"Start to parse TextureOverrideIB and ResourceVB,ResourceIB:");
		for (TextureOverride textureOverride : keyActivatedTextureOverrideList) {
			std::vector<ResourceReplace> resourceReplaceList = textureOverride.ActiveResourceReplaceList;

			int validSize =(int)resourceReplaceList.size();
			if (validSize > 1) {
				LOG.Info(L"More than one ResourceReplace activated.");
			}
			if (validSize == 0) {
				LOG.Error(L"No ResourceReplace activated.");
			}


			for (ResourceReplace onlyResourceReplace: textureOverride.ActiveResourceReplaceList) {
				std::vector<std::wstring> reosurceOriginalLineSplits = MMTString_SplitString(onlyResourceReplace.OriginalLine, L"=");
				std::wstring type = reosurceOriginalLineSplits[0];
				std::wstring resourceName = reosurceOriginalLineSplits[1];
				ModResource resource = resourceDict[resourceName];
				resource.show();

				if (resource.Type == L"VB") {

					bool resourceVBRepeat = false;
					for (ModResource resourceVB :resourceVBList) {
						if (resource.SectionName == resourceVB.SectionName) {
							resourceVBRepeat = true;
						}
					}

					if (!resourceVBRepeat) {
						resourceVBList.push_back(resource);
					}
					else {
						LOG.Info(L"Detect repeated resourceVB, ignore this.");
						LOG.NewLine();
					}
				}

				if (resource.Type == L"Texture") {
					std::filesystem::copy_file(resource.FileName, outputPath + MMTString_GetFileNameFromFilePath(resource.FileName), std::filesystem::copy_options::overwrite_existing);
				}


				if (resource.Type == L"IB") {
					resourceIBList.push_back(resource);

					TextureOverrideIB textureOverrideIB;
					textureOverrideIB.SectionName = textureOverride.SectionName;
					textureOverrideIB.HashValue = textureOverride.HashValue;

					int num = 0;
					try {
						num = boost::lexical_cast<int>(textureOverride.MatchFirstIndex);
					}
					catch (const boost::bad_lexical_cast&) {
						LOG.Info(L"Warning: Can't turn first index into number,use default value: 0.");
					}
					textureOverrideIB.MatchFirstIndex = num;
					textureOverrideIB.IBResourceName = resourceName;
					textureOverrideIB.IBFileName = resource.FileName;
					textureOverrideIB.IBFileFormat = resource.Format;
					textureOverrideIBList.push_back(textureOverrideIB);
				}
			}


			
		
		}

		LOG.NewLine();

		LOG.Info(L"Try to combine mod and reverse: ");
		ReverseModelSingle reverseModelSingle;
		reverseModelSingle.drawIBModList = parseDrawIBModList(textureOverrideIBList,resourceVBList,resourceIBList);

		LOG.Info(L"drawIBModList size: " + std::to_wstring(reverseModelSingle.drawIBModList.size()));


		reverseModelSingle.outputFolderPath = outputPath;


		//TODO
		reverseModelSingle.ApplicationRunningLoaction = ApplicationLocation;
		reverseModelSingle.parseElementList(gameName);

		reverseModelSingle.reverseAndOutput();

		LOG.NewLine();
		LOG.NewLine();

		//NicoMico: Unlock this to test single one in merged.ini
		//break;
	}

	
	LOG.NewLine();
}