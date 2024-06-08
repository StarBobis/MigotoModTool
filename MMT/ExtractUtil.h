#pragma once
#include <iostream>

std::wstring UnityAutoDetectGameType(std::wstring GameName, std::wstring DrawIB, std::wstring FrameAnalysisFolderPath, std::wstring ApplicationLoaction);

void ExtractUtil_MoveAllTextures(std::wstring FrameAnalysisFolderPath, std::wstring DrawIB, std::wstring OutputFolder);