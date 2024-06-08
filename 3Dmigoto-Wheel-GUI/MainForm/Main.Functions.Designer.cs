using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.VisualBasic.FileIO;

namespace NMBT_GUI
{
    partial class Main
    {

        void openLatestLogFile()
        {
            //然后打开最新的Log文件
            string logsPath = basePath + "Logs";
            if (!Directory.Exists(logsPath))
            {
                return;
            }
            string[] logFiles = Directory.GetFiles(logsPath); ;
            List<string> logFileList = new List<string>();
            foreach (string logFile in logFiles)
            {
                string logfileName = Path.GetFileName(logFile);
                if (logfileName.EndsWith(".log") && logfileName.Length > 15)
                {
                    logFileList.Add(logfileName);
                }
            }

            logFileList.Sort();
            Process.Start(logsPath + "\\" + logFileList[logFileList.Count - 1]);
        }

        
        public bool runCommand(string arguments,string targetExe = "")
        {
            //把当前运行的命令保存到RunInput.json
            string json = File.ReadAllText(this.Path_RunInputJson); // 读取文件内容
            JObject runInputJson = JObject.Parse(json);
            runInputJson["RunCommand"] = arguments;
            string runInputJsonStr = runInputJson.ToString(Formatting.Indented);
            File.WriteAllText(this.Path_RunInputJson, runInputJsonStr);

            //运行merge前需要保存配置
            if (arguments == "merge" || arguments == "mergeReverse")
            {
                bool saveResult = saveConfig();
                if (!saveResult)
                {
                    ShowMessageBox("Auto save before run failed, please check your config.", "运行前自动保存配置失败！请检查您的配置！");
                    //MessageBox.Show("运行前自动保存配置失败！请检查您的配置！");
                    return false;
                }
            }

            //运行程序前，设置RunResult.json填充默认值，结果由MMT.exe进行纠正。
            JObject jsonObject = new JObject();
            jsonObject["result"] = "Unknown Error!";
            File.WriteAllText(Path_RunResultJson, jsonObject.ToString());

            // 创建一个 Process 对象
            Process process = new Process();
            // 设置要执行的程序和参数，主要是路径这里我们这样可以支持专业版与基础版区分，以及其他插件的添加
            if (targetExe == "")
            {
                process.StartInfo.FileName = basePath + WHEEL_EXE_PATH;
            }
            else
            {
                process.StartInfo.FileName = basePath + targetExe;
            }
            //运行前必须检查路径
            if (!File.Exists(process.StartInfo.FileName))
            {
                ShowMessageBox("Current run path didn't exsits: " + process.StartInfo.FileName, "当前要执行的路径不存在: " + process.StartInfo.FileName);
                //MessageBox.Show("当前要执行的路径不存在: "+ process.StartInfo.FileName);
                return false;
            }
            
            process.StartInfo.Arguments = arguments;  // 可选，如果该程序接受命令行参数
            //MessageBox.Show("当前运行参数： " + arguments);

            // 配置进程启动信息
            process.StartInfo.UseShellExecute = false;  // 不使用操作系统的shell启动程序
            process.StartInfo.RedirectStandardOutput = false;  // 重定向标准输出
            process.StartInfo.RedirectStandardError = false;   // 重定向标准错误输出
            process.StartInfo.CreateNoWindow = true;  // 不创建新窗口
             

            // 启动程序
            process.Start();
            process.WaitForExit();
            if (arguments == "merge" )
            {
                if (CurrentLanguage == "zh-cn")
                {
                    setDataGridViewStatus("等待生成");
                }
                else
                {
                    setDataGridViewStatus("Extracted");
                }
            }

            string runResultJson = File.ReadAllText(Path_RunResultJson);
            JObject resultJsonObject = JObject.Parse(runResultJson);
            string runResult = (string)resultJsonObject["result"];

            if (runResult != "success")
            {
                ShowMessageBox("Run result: " + runResult + ". \n1.Please check your config.\n2.Please check log for more information.\n3.Please ask NicoMico for help.",
                    "运行结果: " + runResult + ". \n1.请检查你的配置是否正确.\n2.请查看日志获取更多细节信息.\n3.请联系NicoMico寻求帮助或反馈BUG.");
                //MessageBox.Show("Run result: " + runResult + ". \n1.Please check your config.\n2.Please check log for more information.\n3.Please ask NicoMico for help.");
                openLatestLogFile();
                return false;
            }
            else
            {
                ShowMessageBox("Run result: " + runResult, "运行结果: " + runResult);
                //MessageBox.Show("Run result: " + runResult);
            }

            return true;

        }

        public void setDataGridViewStatus(string value)
        {
            
            foreach (DataGridViewRow row in DataGridView_BasicIBList.Rows)
            {
                if (!row.IsNewRow && row.Cells.Count > 2)
                {
                    row.Cells[2].Value = value;
                }
            }
        }


        public void addLineToDataGridDrawIBList(string drawIB, string buttonValue,string status)
        {
            DataGridViewRow row = new DataGridViewRow();

            DataGridViewTextBoxCell textBoxCellDrawIB = new DataGridViewTextBoxCell();
            textBoxCellDrawIB.Value = drawIB;
            row.Cells.Add(textBoxCellDrawIB);

            DataGridViewButtonCell textBoxCellButton = new DataGridViewButtonCell();
            textBoxCellButton.Value = buttonValue;
            row.Cells.Add(textBoxCellButton);

            DataGridViewTextBoxCell statusCell = new DataGridViewTextBoxCell();
            statusCell.Value = status;
            row.Cells.Add(statusCell);

            DataGridView_BasicIBList.Rows.Add(row);
        }


        
        private void setCurrentGame(string gameName)
        {
            //读取并设置一些配置变量，用于传递给每个DrawIB的ConfigMod窗口
            if (File.Exists(this.Path_Game_SettingJson))
            {
                string json = File.ReadAllText(this.Path_Game_SettingJson); // 读取文件内容
                JObject jsonObject = JObject.Parse(json);
                this.AutoCleanFrameAnalysisFolder = (bool)jsonObject["AutoCleanFrameAnalysisFolder"];
                this.FrameAnalysisFolderReserveNumber = (int)jsonObject["FrameAnalysisFolderReserveNumber"];
                this.AutoCleanLogFile = (bool)jsonObject["AutoCleanLogFile"];
                this.LogFileReserveNumber = (int)jsonObject["LogFileReserveNumber"];
                this.DeleteOutputFolder = (bool)jsonObject["DeleteOutputFolder"];
                this.BackUpFiles = (bool)jsonObject["BackUp"];
            }

            this.CurrentGameName = gameName;

            //先清空dataGridView，切换游戏类型后重新读取
            DataGridView_BasicIBList.Rows.Clear();

            //同时也要设置一下配置文件路径
            this.Path_Game_ConfigJson = "Games\\" + CurrentGameName + "\\Config.json";
            //读取configs\\游戏名Config.json，然后判断是否含有此drawIB来决定按钮的

            if (File.Exists(this.Path_Game_ConfigJson) )
            {
                //切换到对应配置
                string jsonData = File.ReadAllText(this.Path_Game_ConfigJson);
                JArray jsonArray = JArray.Parse(jsonData);

                foreach (JObject obj in jsonArray)
                {
                    string readDrawIB = (string)obj["DrawIB"];
                    addLineToDataGridDrawIBList(readDrawIB, "Configured", "");
                }
            }

            this.Path_LoaderFolder = "Games\\" + this.CurrentGameName + "\\3Dmigoto\\";
            this.Path_OutputFolder = this.Path_LoaderFolder + "Mods\\output\\";

            


            //Update window name only if we can success run to here.
            if (CurrentLanguage == "en")
            {
                this.Text = window_name + " CurrentGame: " + gameName;
            }
            else if (CurrentLanguage == "zh-cn")
            {
                this.Text = window_name + " 当前游戏: " + gameName;
            }

            //最后把当前游戏名称和类型保存到配置文件，做到和Blender联动。
            saveCurrentGameNameAndGameType();
        }
        

        private void initializeFolders()
        {
            
            //关于是否需要删除OutputFolder并重新生成
            if (File.Exists(this.Path_Game_SettingJson))
            {
                string json = File.ReadAllText(this.Path_Game_SettingJson); // 读取文件内容
                JObject jsonObject = JObject.Parse(json);
                if (jsonObject.ContainsKey(this.CurrentGameName))
                {
                    bool deleteOutputFolder = (bool)jsonObject[this.CurrentGameName]["DeleteOutputFolder"];
                    string outputFolder = (string)jsonObject[this.CurrentGameName]["OutputFolder"];
                    if (deleteOutputFolder)
                    {
                        if (Directory.Exists(outputFolder))
                        {
                            DialogResult result = DialogResult.None;

                            if (CurrentLanguage == "zh-cn")
                            {
                                result = MessageBox.Show("您设置了在融合模型文件前删除OutputFolder，此操作非常危险，请再次检查防止误删除文件，您的OutputFolder路径为：" + outputFolder, "是否确认删除？", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                            }else if (CurrentLanguage == "en")
                            {
                                result = MessageBox.Show("You checked delete OutputFolder before extract,this operation is danger,please check your OutputFolder path again to prevent mistakely delete file,your current OutputFolder path is" + outputFolder, " Do you make sure to delete?", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);

                            }

                            // 根据用户的选择返回结果
                            if (result == DialogResult.Yes)
                            {

                                //已证实移动到回收站在部分情况下并不生效
                                //FileSystem.DeleteDirectory(outputFolder, UIOption.AllDialogs, RecycleOption.SendToRecycleBin);
                                Directory.Delete(outputFolder,true);
                                Directory.CreateDirectory(outputFolder);
                            }

                        }
                        else
                        {
                            Directory.CreateDirectory(outputFolder);
                        }
                    }
                }
                
            }
        }



        private void backupFiles()
        {
            if (this.BackUpFiles)
            {
                //创建用于备份的文件夹
                if (!Directory.Exists("Backups"))
                {
                    Directory.CreateDirectory("Backups");
                }

                //确定备份文件夹的名称
                DateTime now = DateTime.Now;
                // 使用特定的格式字符串将日期和时间格式化为文件名字符串
                string formattedDateTime = now.ToString("yyyyMMdd_HHmmss");
                string targetFolder = "Backups\\" + formattedDateTime;
                Directory.CreateDirectory(targetFolder);

                // 创建每个drawIB的备份文件夹
                string[] draw_ib_directories = Directory.GetDirectories(this.Path_OutputFolder);
                foreach (string directoryPath in draw_ib_directories)
                {
                    string directoryName = Path.GetFileName(directoryPath);
                    string draw_ib_backup_directory = targetFolder + "/" + directoryName;
                    Directory.CreateDirectory(draw_ib_backup_directory);

                    //复制所有文件
                    DirectoryCopy(directoryPath, draw_ib_backup_directory, false);

                    //TODO 备份PsHashTextures
                    string ps_hash_textures_folder_path = directoryPath + "/" + "PsHashTextures";
                    if (Directory.Exists(ps_hash_textures_folder_path))
                    {
                        string target_ps_hash_textures_backup_path = draw_ib_backup_directory + "/" + "PsHashTextures";
                        Directory.CreateDirectory(target_ps_hash_textures_backup_path);
                        DirectoryCopy(ps_hash_textures_folder_path, target_ps_hash_textures_backup_path, false);
                    }

                }
                //备份OutputFolder下面的纯文件
                DirectoryCopy(this.Path_OutputFolder, targetFolder, false);

                // 创建Mod的备份文件夹
                string output_mod_folder_name = now.ToString("yyyy_MM_dd");
                string output_mod_folder_path = this.Path_OutputFolder + output_mod_folder_name;
                string backup_mod_folder_path = targetFolder + "/" + output_mod_folder_name;
                Directory.CreateDirectory(backup_mod_folder_path);
                DirectoryCopy(output_mod_folder_path, backup_mod_folder_path, true);

            }

        }


        private void start3Dmigoto()
        {
            string migotoLoaderPath = basePath + "3Dmigoto\\" + CurrentGameName + "\\3Dmigoto Loader.exe";

            if (!File.Exists(migotoLoaderPath))
            {
                ShowMessageBox("Can't find " + migotoLoaderPath + ", please check if your config.", "无法找到" + migotoLoaderPath + "，请检查你的配置是否正确");
                //MessageBox.Show("Can't find " + migotoLoaderPath  + ", please check if your config.");
                return;
            }
            ProcessStartInfo startInfo = new ProcessStartInfo(migotoLoaderPath);
            string workingDirectory = Path.GetDirectoryName(migotoLoaderPath);
            startInfo.WorkingDirectory = workingDirectory;
            startInfo.CreateNoWindow = true;
            startInfo.UseShellExecute = true;
            Process.Start(startInfo);
        }

    }
}
