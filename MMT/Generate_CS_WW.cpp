#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "MMTFormatUtils.h"
#include "WWUtil.h"


void Generate_CS_WW_Body() {

	for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        ExtractConfig extractConfig = pair.second;
       
        D3D11GameType d3d11GameType = G.GameTypeName_D3d11GameType_Map[extractConfig.WorkGameType];

        std::wstring timeStr = MMTString_GetFormattedDateTimeForFilename().substr(0, 10);
        std::wstring splitReadFolder = G.OutputFolder + DrawIB + L"\\";
        std::wstring splitOutputFolder = G.OutputFolder + timeStr + L"\\" + DrawIB + L"\\";
        std::filesystem::create_directories(splitOutputFolder);

        bool findValidFile = false;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
            if (std::filesystem::exists(splitReadFolder + VBFileName)) {
                findValidFile = true;
                break;
            }
        }
        if (!findValidFile) {
            LOG.Info(L"Detect didn't export vb file for DrawIB: " + DrawIB + L" , so skip this drawIB generate.");
            continue;
        }


        std::unordered_map<std::string, int> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(extractConfig.TmpElementList);
        std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(extractConfig.TmpElementList);
        //输出查看每个Category的步长
        for (const auto& pair : CategoryStrideMap) {
            const std::string& key = pair.first;
            int value = pair.second;
            LOG.Info("Category: " + key + ", CategoryStride: " + std::to_string(value));
        }
        LOG.NewLine();

        //查看CategoryList
        LOG.Info(L"CategoryList:");
        for (std::string categoryName : CategoryList) {
            LOG.Info("Ordered CategoryName: " + categoryName);
        }
        LOG.NewLine();

        

        //(1) 输出BUF文件
        //读取vb文件，每个vb文件都按照category分开装载不同category的数据
        int SplitStride = d3d11GameType.getElementListStride(extractConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
        int drawNumber = 0;
        std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;
        std::unordered_map<std::string, int> partNameOffsetMap;
        std::unordered_map<std::string, int> partNameVertexCountMap;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString_ToWideString(partName) + L".vb";
            uint64_t VBFileSize = MMTFile_GetFileSize(splitReadFolder + VBFileName);
            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;
         
            partNameVertexCountMap[partName] = vbFileVertexNumber;
            LOG.Info(L"Processing VB file: " + VBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(vbFileVertexNumber));
            VertexBufferBufFile vbBufFile(splitReadFolder + VBFileName, d3d11GameType, extractConfig.TmpElementList);
            partName_VBCategoryDaytaMap[MMTString_ToWideString(partName)] = vbBufFile.CategoryVBDataMap;


            //设置offset
            partNameOffsetMap[partName] = drawNumber;
            //添加到drawNumber
            drawNumber = drawNumber + vbFileVertexNumber;
        }



        //(2) 转换并输出每个IB文件,这里注意输出的IB要加上Offset，因为我们Blender导出的IB都是从0开始的
        //先设置读取ib文件所使用的Format,从1.fmt文件中自动读取
        std::wstring IBReadDxgiFormat = MMTFile_FindMigotoIniAttributeInFile(splitReadFolder + L"1.fmt", L"format");
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring IBFileName = MMTString_ToWideString(partName) + L".ib";
            std::wstring readIBFileName = splitReadFolder + IBFileName;
            std::wstring writeIBFileName = splitOutputFolder + IBFileName;
            LOG.Info(L"Converting IB file: " + IBFileName);
            IndexBufferBufFile ibBufFile(readIBFileName, IBReadDxgiFormat);
            ibBufFile.SaveToFile_UINT32(writeIBFileName, partNameOffsetMap[partName]);
        }
        LOG.Info(L"Output ib file over");
        LOG.NewLine();


        LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
        //将partName_VBCategoryDaytaMap里的内容，放入finalVBCategoryDataMap中组合成一个，供后续使用
        std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;
        for (std::string partName : extractConfig.PartNameList) {
            std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = partName_VBCategoryDaytaMap[MMTString_ToWideString(partName)];
            for (size_t i = 0; i < CategoryList.size(); ++i) {
                const std::string& category = CategoryList[i];
                std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString_ToWideString(category)];
                if (category == "Normal") {
                    for (int index = 0; index < tmpCategoryData.size(); index = index + 8) {
                        //1.获取NORMAL和TANGENT值
                        std::byte NormalValueX = tmpCategoryData[index + 0];
                        std::byte NormalValueY = tmpCategoryData[index + 1];
                        std::byte NormalValueZ = tmpCategoryData[index + 2];
                        std::byte TangentValueX = tmpCategoryData[index + 4];
                        std::byte TangentValueY = tmpCategoryData[index + 5];
                        std::byte TangentValueZ = tmpCategoryData[index + 6];

                        //2.经过观察NORMAL的值为TANGENT前三位直接放过来，最后一位设为0x7F
                        tmpCategoryData[index + 0] = TangentValueX;
                        tmpCategoryData[index + 1] = TangentValueY;
                        tmpCategoryData[index + 2] = TangentValueZ;
                        tmpCategoryData[index + 3] = std::byte(0x7F);

                        //3.翻转NORMAL的前三位并放到TANGENT的前三位，NORMAL的W设为0x7F
                        tmpCategoryData[index + 4] = MMTFormat_ReverseSNORMValueSingle(NormalValueX);
                        tmpCategoryData[index + 5] = MMTFormat_ReverseSNORMValueSingle(NormalValueY);
                        tmpCategoryData[index + 6] = MMTFormat_ReverseSNORMValueSingle(NormalValueZ);
                    }
                }
                std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString_ToWideString(category)];
                finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

            }
        }
        LOG.NewLine();

        //直接输出
        for (const auto& pair : finalVBCategoryDataMap) {
            const std::wstring& categoryName = pair.first;
            const std::vector<std::byte>& categoryData = pair.second;
            LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));
            //如果没有那就不输出
            if (categoryData.size() == 0) {
                LOG.Info(L"Current category's size is 0, can't output, skip this.");
                continue;
            }
            std::wstring categoryGeneratedName = DrawIB + categoryName;
            // 构建输出文件路径
            std::wstring outputDatFilePath = splitOutputFolder + categoryGeneratedName + L".buf";
            // 打开输出文件 将std::vecto的内容写入文件
            std::ofstream outputFile(MMTString_ToByteString(outputDatFilePath), std::ios::binary);
            outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
            outputFile.close();
            LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
        }
        LOG.NewLine();


        //(3) 生成ini文件
        std::wstring outputIniFileName = splitOutputFolder + extractConfig.DrawIB  + L".ini";
        std::wofstream outputIniFile(outputIniFileName);

        //如果是使用CS计算，则突破顶点数量限制
        //TODO 除此之外，还需要调用Dispatch,所以CSReplace需要和store命令配合使用
        //比如在1ff924db9d4048d1中，读取索引3上的计算次数到变量，通过下面填写判断变量值是否为原始顶点数
        //来执行cs-cb0替换，并且执行我们自定义数量的Dispatch
        //那么是否可以通过其中一个cs-tX的Hash来替换其它cs-tX槽位上的数据呢？如果不能，Dispatch要在哪里调用呢？是需要CustomShader调用嘛？
        //TODO 有空了测试上述理论，现在cs-cb0数值已经可以更改，就差最后一个Dispatch调用位置就可以突破了

        //TODO 能否绕过Dispatch的问题？比如修改一下ComputeShader，如果检测到需要变更的Dispatch数量，则使用我们自己的数值填入变量来进行替换计算次数
        //以此绕过Dispatch限制？有空进行测试。
        bool debug = false;
        if (d3d11GameType.GPUPreSkinning && debug) {
            outputIniFile << std::endl;
            outputIniFile << L"; -------------- Break Vertex Count Limit -----------------" << std::endl << std::endl;
            outputIniFile << L"[TextureOverride_CSReplace_VertexLimitBreak]" << std::endl;
            //TODO 需要收集cs-cb0的hash值
            outputIniFile << L"hash = " << MMTString_ToWideString(extractConfig.VertexLimitVB) << std::endl;

            //先读取csinfo.json里的数据
            WuwaCSInfoJsonObject wwcsinfoObject(splitReadFolder);

            //遍历每个partName
            int currentOffset = 0;
            for (std::string partName :extractConfig.PartNameList) {
                int currentVertexCount = partNameVertexCountMap[partName];
                WuwaCSInfo wwcsInfo = wwcsinfoObject.PartNameWuwaCSInfoMap[partName];
                int originalVertexCount = wwcsInfo.CalculateTime;
                int originalOffset = wwcsInfo.Offset;
                LOG.Info("CurrentVertexCount: " + std::to_string(currentVertexCount) + " OriginalVertexCount: " + std::to_string(originalVertexCount));
                outputIniFile << L";------------" + MMTString_ToWideString(partName) + L"-----------" << std::endl;

                if (originalVertexCount == currentVertexCount) {
                    //顶点数相同时，无需改变顶点数，但是如果偏移不同还是需要改偏移的。
                    if (currentOffset != originalOffset) {
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //修改3个偏移数
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //修改2个偏移数
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                        }
                        
                    }
                }
                else {
                    if (currentOffset != originalOffset) {
                        //顶点数不同，偏移也不同时全部都要修改
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //修改3个偏移数
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl << std::endl;
                            //修改1个顶点数
                            outputIniFile << L"csreplace = cs-cb0, 4, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //修改2个偏移数
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl;
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << std::endl << std::endl;
                            //修改1个顶点数
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                    }
                    else {
                        //顶点数不同，偏移相同时，只需要改顶点数
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //修改1个顶点数
                            outputIniFile << L"csreplace = cs-cb0, 4, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //修改1个顶点数
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << std::endl;

                        }
                    }
                }

                currentOffset = currentOffset + currentVertexCount;
            }

        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- TextureOverride VB -----------------" << std::endl << std::endl;
        //1.写出VBResource部分
        for (std::string categoryName : CategoryList) {
            std::wstring fileName = extractConfig.DrawIB + MMTString_ToWideString(categoryName) + L".buf";
            std::wstring filePath = splitOutputFolder + fileName;
            int fileSize = MMTFile_GetFileSize(filePath);
            std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
            outputIniFile << L"[TextureOverride_" + MMTString_ToWideString(categoryName) + L"_Replace]" << std::endl;
            outputIniFile << L"hash = " << MMTString_ToWideString(categoryHash) << "" << std::endl;
            outputIniFile << "this = " << L"Resource_VB_" + MMTString_ToWideString(categoryName) + L"" << std::endl << std::endl;
        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- Resource VB -----------------" << std::endl << std::endl;
        //ResourceVB
        for (std::string categoryName : CategoryList) {
            std::wstring fileName = extractConfig.DrawIB + MMTString_ToWideString(categoryName) + L".buf";
            std::wstring filePath = splitOutputFolder + fileName;
            int fileSize = MMTFile_GetFileSize(filePath);
            std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];


            outputIniFile << L"[Resource_VB_" + MMTString_ToWideString(categoryName) + L"]" << std::endl;
            outputIniFile << L"byte_width = " << std::to_wstring(fileSize) << std::endl;
            if (categoryName == "Texcoord") {
                outputIniFile << L"type = Buffer" << std::endl;
                outputIniFile << L"FORMAT = R16G16_FLOAT" << std::endl;
            }
            else if (categoryName == "Normal") {
                outputIniFile << L"type = Buffer" << std::endl;
                outputIniFile << L"FORMAT = R8G8B8A8_SNORM" << std::endl;
            }
            else if (categoryName == "Position" && categorySlot== "vb0") {
                outputIniFile << L"type = Buffer" << std::endl;
                outputIniFile << L"FORMAT = R32G32B32_FLOAT" << std::endl;
            }
            else {
                outputIniFile << L"type = ByteAddressBuffer" << std::endl;
                outputIniFile << "stride = " << CategoryStrideMap[categoryName] << std::endl;
            }
            outputIniFile << "filename = " << fileName << std::endl << std::endl;

            
        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- IB Skip -----------------" << std::endl << std::endl;

        //IB SKIP部分
        outputIniFile << L"[TextureOverride_" + extractConfig.DrawIB + L"_IB_SKIP]" << std::endl;
        outputIniFile << L"hash = " + extractConfig.DrawIB << std::endl;
        outputIniFile << "handling = skip" << std::endl;
        outputIniFile << std::endl;

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- TextureOverride IB & Resource IB-----------------" << std::endl << std::endl;

        //TextureOverride IB部分
        for (int i = 0; i < extractConfig.PartNameList.size(); ++i) {
            std::string partName = extractConfig.PartNameList[i];
            LOG.Info(L"Start to output UE4 ini file.");

            //按键开关支持
            bool generateSwitchKey = false;
            std::wstring activateFlagName = L"ActiveFlag_" + extractConfig.DrawIB;
            std::wstring switchVarName = L"SwitchVar_" + extractConfig.DrawIB;
            std::wstring replace_prefix = L"";
            if (extractConfig.SwitchKey != L"") {
                generateSwitchKey = true;
                replace_prefix = L"  ";
                //添加对应Constants和KeySwitch部分
                outputIniFile << "[Constants]" << std::endl;
                outputIniFile << "global persist $" << switchVarName << " = 1" << std::endl;
                outputIniFile << "global $" << activateFlagName << " = 0" << std::endl;
                outputIniFile << std::endl;

                outputIniFile << "[Key" << switchVarName << "]" << std::endl;
                outputIniFile << "condition = $" << activateFlagName << " == 1" << std::endl;
                outputIniFile << "key = " << extractConfig.SwitchKey << std::endl;
                outputIniFile << "type = cycle" << std::endl;
                outputIniFile << "$" << switchVarName << " = 0,1" << std::endl;
                outputIniFile << std::endl;

                outputIniFile << "[Present]" << std::endl;
                outputIniFile << "post $" << activateFlagName << " = 0" << std::endl;
                outputIniFile << std::endl;
            }
            LOG.Info(L"Generate Switch Key ini :" + std::to_wstring(generateSwitchKey));

            //4.IBOverride部分
            std::string IBFirstIndex = extractConfig.MatchFirstIndexList[i];
            outputIniFile << L"[Resource_BakIB" + MMTString_ToWideString(partName) + L"]" << std::endl;
            outputIniFile << L"[TextureOverride_IB_" + extractConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
            outputIniFile << L"hash = " + extractConfig.DrawIB << std::endl;
            outputIniFile << L"Resource_BakIB" + MMTString_ToWideString(partName) + L" = ref ib" << std::endl;
            outputIniFile << L"match_first_index = " + MMTString_ToWideString(IBFirstIndex) << std::endl;
            if (generateSwitchKey) {
                outputIniFile << L"if $" + switchVarName + L" == 1" << std::endl;
            }
            outputIniFile << replace_prefix << L"ib = Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString_ToWideString(partName) << std::endl;
            outputIniFile << replace_prefix << "drawindexed = auto" << std::endl;
            outputIniFile << L"ib = Resource_BakIB" + MMTString_ToWideString(partName) << std::endl;

            if (generateSwitchKey) {
                outputIniFile << "endif" << std::endl;
            }
            outputIniFile << std::endl;
        }

        outputIniFile << std::endl;
        outputIniFile << L"; -------------- IB Resource -----------------" << std::endl << std::endl;

        //2.写出IBResource部分
        for (int i = 0; i < extractConfig.PartNameList.size(); ++i) {
            std::string partName = extractConfig.PartNameList[i];
            outputIniFile << L"[Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString_ToWideString(partName) + L"]" << std::endl;
            outputIniFile << "type = Buffer" << std::endl;
            outputIniFile << "format = DXGI_FORMAT_R32_UINT" << std::endl;
            outputIniFile << "filename = " << MMTString_ToWideString(partName) + L".ib" << std::endl << std::endl;
        }

        outputIniFile << L"; Mod Generated by MMT-Community." << std::endl ;
        outputIniFile << L"; Github: https://github.com/StarBobis/MigotoModTool"  << std::endl;
        outputIniFile << L"; Discord: https://discord.gg/Cz577BcRf5" << std::endl;

        outputIniFile.close();

        LOG.NewLine();
        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

    }

}