using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NMBT_GUI
{
    partial class Main
    {
        void AutoDetectLanguage()
        {
            // 获取当前的区域设置
            CultureInfo currentCulture = CultureInfo.CurrentCulture;
            // 获取区域设置的语言名称
            string language = currentCulture.TwoLetterISOLanguageName;
            // 判断语言是否为中文系列语言，如果是就直接切换到中文，否则使用英文
            if (language == "zh")
            {
                switchToChinese();
            }
            else
            {
                switchToEnglish();
            }
        }

        //设计上采用所有GUI的标签全部可以切换语言显示
        //所有的报错和提示都只提供英文和中文版本
        //在工具设计时，全部使用英文作为初始化界面
        //在切换语言时，才切换到对应的中文界面，此外在启动时会自动读取当前系统语言来选择是否为中文
        //要注意工具的UI布局不要出现切换语言后部分UI无法显示完全的状况
        void ShowMessageBox(string EnglishTip, string ChineseTip)
        {
            if (CurrentLanguage == "en")
            {
                MessageBox.Show(EnglishTip);
            }
            else if (CurrentLanguage == "zh-cn")
            {
                MessageBox.Show(ChineseTip);
            }
        }

        void updateCurrentLangugageToJson()
        {
            JObject languageJsonObject = new JObject();
            languageJsonObject["language"] = CurrentLanguage;
            string json_string = languageJsonObject.ToString(Formatting.Indented);
            File.WriteAllText(Path_Languages, json_string);
        }


        void switchToEnglish()
        {
            //设置当前语言为en
            CurrentLanguage = "en";

            //将当前语言写入到Json文件
            updateCurrentLangugageToJson();

            //语言菜单
            Menu_languages_ToolStripMenuItem.Text = "Languages";

            //文件菜单
            Menu_File_ToolStripMenuItem.Text = "File";
            Menu_File_OpenOutputFolder_ToolStripMenuItem.Text = "Open OutputFolder";
            Menu_File_OpenModGenerateFolder_ToolStripMenuItem.Text = "Open Mod Generate Folder";
            Menu_File_OpenModsFolder_ToolStripMenuItem.Text = "Open Mods Folder";
            Menu_File_OpenLatestFrameAnalysisFolder_ToolStripMenuItem.Text = "Open Latest FrameAnalysis Folder";
            Menu_File_Open3DmigotoFolder_ToolStripMenuItem.Text = "Open 3Dmigoto Folder";
            Menu_File_Open3DmigotosD3dxini_ToolStripMenuItem.Text = "Open 3Dmigoto's d3dx.ini";
            Menu_File_OpenShaderFixesFolder_ToolStripMenuItem.Text = "Open ShaderFixes Folder";

            Menu_File_OpenLatestLogFile_ToolStripMenuItem.Text = "Open Latest LOG File";
            Menu_File_OpenLogsFolder_ToolStripMenuItem.Text = "Open Logs Folder";
            Menu_File_OpenMMTLocationFolder_ToolStripMenuItem.Text = "Open MMT's Location Folder";
            Menu_File_OpenBackupsFolder_ToolStripMenuItem.Text = "Open Backups Folder";
            Menu_File_OpenConfigsFolder_ToolStripMenuItem.Text = "Open Configs Folder";
            Menu_File_OpenTypesFolderToolStripMenuItem.Text = "Open Types Folder";

            //设置菜单
            Menu_Setting_ToolStripMenuItem.Text = "Setting";

            //BasicMod
            groupBoxBasicMod.Text = "Basic Mod";
            Menu_BasicMod_InitializeConfig_ToolStripMenuItem.Text = "Initialize Config";
            Menu_BasicMod_SaveConfig_ToolStripMenuItem.Text = "Save Config";

            //IB列表
            ColumnDrawIB.HeaderText = "DrawIB";
            ColumnConfig.HeaderText = "Config";
            ColumnStatus.HeaderText = "Status";

            //Basic Mod的按钮
            button_ExtractModel.Text = "Extract Model";
            button_GenerateMod.Text = "Generate Mod";


            //about
            Menu_about_ToolStripMenuItem.Text = "About";
            Menu_checkForUpdates_ToolStripMenuItem.Text = "Check For Updates";
            //Menu_SupportAuthor_ToolStripMenuItem.Text = "Support NicoMico";

            //Reverse
            Menu_Reverse_ToolStripMenuItem.Text = "Reverse";
            Menu_Reverse_reverseSingleModToolStripMenuItem.Text = "Reverse Single Mod";
            Menu_Reverse_reverseMergedModToolStripMenuItem.Text = "Reverse Merged Mod";
            Menu_Reverse_reverseOutfitCompilerCompressedMergedModToolStripMenuItem.Text = "Reverse OutfitCompiler Compressed Merged Mod";
            Menu_Reverse_extractReverseModelToolStripMenuItem.Text = "Extract Reverse Model";


            //Run
            Menu_RunToolStripMenuItem.Text = "Run";
            Menu_Run_run3DmigotoLoaderexeToolStripMenuItem.Text = "Run 3Dmigoto Loader.exe";
            Menu_Run_run3DmigotoLoaderByPassACEexeToolStripMenuItem.Text = "Run 3Dmigoto Loader-ByPassACE.exe";

        }


        void switchToChinese()
        {
            //设置当前语言为zh-cn
            CurrentLanguage = "zh-cn";

            //将当前语言写入到Json文件
            updateCurrentLangugageToJson();

            //语言菜单
            Menu_languages_ToolStripMenuItem.Text = "语言";

            //文件菜单
            Menu_File_ToolStripMenuItem.Text = "文件";
            Menu_File_OpenOutputFolder_ToolStripMenuItem.Text = "打开Output文件夹";
            Menu_File_OpenModGenerateFolder_ToolStripMenuItem.Text = "打开Mod生成文件夹";
            Menu_File_OpenModsFolder_ToolStripMenuItem.Text = "打开Mods文件夹";
            Menu_File_OpenLatestFrameAnalysisFolder_ToolStripMenuItem.Text = "打开最新的FrameAnalysis文件夹";
            Menu_File_Open3DmigotoFolder_ToolStripMenuItem.Text = "打开3Dmigoto文件夹";
            Menu_File_Open3DmigotosD3dxini_ToolStripMenuItem.Text = "打开d3dx.ini";
            Menu_File_OpenShaderFixesFolder_ToolStripMenuItem.Text = "打开ShaderFixes文件夹";

            Menu_File_OpenLatestLogFile_ToolStripMenuItem.Text = "打开最新的日志文件";
            Menu_File_OpenLogsFolder_ToolStripMenuItem.Text = "打开Logs文件夹";
            Menu_File_OpenMMTLocationFolder_ToolStripMenuItem.Text = "打开MMT所在文件夹";
            Menu_File_OpenBackupsFolder_ToolStripMenuItem.Text = "打开Backups文件夹";
            Menu_File_OpenConfigsFolder_ToolStripMenuItem.Text = "打开Configs文件夹";
            Menu_File_OpenTypesFolderToolStripMenuItem.Text = "打开Types文件夹";


            //设置菜单
            Menu_Setting_ToolStripMenuItem.Text = "设置";


            //BasicMod
            groupBoxBasicMod.Text = "基础Mod";
            Menu_BasicMod_InitializeConfig_ToolStripMenuItem.Text = "初始化配置";
            Menu_BasicMod_SaveConfig_ToolStripMenuItem.Text = "保存配置";


            //IB列表
            ColumnDrawIB.HeaderText = "绘制IB值";
            ColumnConfig.HeaderText = "配置";
            ColumnStatus.HeaderText = "状态";

            //Basic Mod的按钮
            button_ExtractModel.Text = "提取模型";
            button_GenerateMod.Text = "生成二创模型";

            //about
            Menu_about_ToolStripMenuItem.Text = "帮助";
            Menu_checkForUpdates_ToolStripMenuItem.Text = "打开本工具更新地址";
            //Menu_SupportAuthor_ToolStripMenuItem.Text = "去爱发电赞助支持NicoMico";

            //Reverse
            Menu_Reverse_ToolStripMenuItem.Text = "逆向Mod";
            Menu_Reverse_reverseSingleModToolStripMenuItem.Text = "逆向普通单个Mod";
            Menu_Reverse_reverseMergedModToolStripMenuItem.Text = "逆向普通多合一切换Mod";
            Menu_Reverse_reverseOutfitCompilerCompressedMergedModToolStripMenuItem.Text = "逆向OutfitCompiler压缩后的多合一切换Mod";
            Menu_Reverse_extractReverseModelToolStripMenuItem.Text = "提取Mod模型";

            //Run
            Menu_RunToolStripMenuItem.Text = "运行";
            Menu_Run_run3DmigotoLoaderexeToolStripMenuItem.Text = "运行 3Dmigoto Loader.exe";
            Menu_Run_run3DmigotoLoaderByPassACEexeToolStripMenuItem.Text = "运行 3Dmigoto Loader-ByPassACE.exe";
            
        }
    }
}
