using Microsoft.VisualBasic.FileIO;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;


namespace NMBT_GUI
{
    public partial class Main : Form
    {
        //程序名称的基础前缀
        private string MMTName = "MMT-Community V";
        //当前选择的游戏名称
        private string CurrentGameName = "";
        //当前的语言类型
        private string CurrentLanguage = "";

        //由C++开发的核心算法进程
        private string WHEEL_EXE_PATH = "MMT.exe";

        //当前程序运行所在位置的路径,注意这里已经包含了结尾的\\
        private string basePath = AppDomain.CurrentDomain.BaseDirectory.ToString();
        
        private string Path_MainConfig = "Configs\\Main.json";
        private string Path_RunResultJson = "Configs\\RunResult.json";
        private string Path_RunInputJson = "Configs\\RunInput.json";
        private string Path_Languages = "Configs\\Languages.json";
        
        //运行后程序动态生成
        private string window_name = "";
        private string Path_Game_ConfigJson = "";
        private string Path_Game_SettingJson = "Configs\\Setting.json";
        private string Path_OutputFolder = "";
        private string Path_LoaderFolder = "";
        private bool AutoCleanFrameAnalysisFolder = false;
        private int FrameAnalysisFolderReserveNumber = 0;
        private bool AutoCleanLogFile = false;
        private int LogFileReserveNumber = 0;

        private bool DeleteOutputFolder;
        private bool BackUpFiles;


        
        public Main()
        {
            InitializeComponent();
        }

        

        private void Main_Load(object sender, EventArgs e)
        {
            AutoDetectLanguage();

            //检查当前程序是否为位于中文路径下
            if (ContainsChinese(basePath))
            {
                ShowMessageBox("MMT can't be put in a path that contains Chinese, please put MMT in pure english path!", "MMT所在路径不能含有中文，请重新将MMT放置到纯英文路径！");
                Application.Exit();
            }

            //检查MMT是否存在
            if (!File.Exists(basePath + WHEEL_EXE_PATH))
            {
                ShowMessageBox("Can't find " + basePath + WHEEL_EXE_PATH + ",please put it under this program's location。", "未找到" + basePath + WHEEL_EXE_PATH + ",请将其放在本程序所在目录下，即将退出程序。");
                Application.Exit();
            }

            //根据版本号设置窗口标题
            FileVersionInfo version_info = FileVersionInfo.GetVersionInfo(WHEEL_EXE_PATH);
            window_name = MMTName + version_info.FileVersion;
            this.Text = window_name;

            //填充游戏名称列表
            string[] game_folders = Directory.GetDirectories("Games\\");
            //转换提取出目录名称
            List<string> game_folder_names = new List<string>();
            foreach (string game_folder_path in game_folders)
            {
                string folder_name = Path.GetFileName(game_folder_path);
                game_folder_names.Add(folder_name);
            }

            Menu_GameName_toolStripComboBox.Items.Clear();
            Menu_GameName_toolStripComboBox.Items.AddRange(game_folder_names.ToArray());



        

            //读取游戏类型和DrawIB列表
            readConfig(Path_MainConfig);
        }


        private void DataGridViewIBList_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == 1 && e.RowIndex >= 0)
            {
                //获取drawIB的值，先校验DrawIB和游戏类型是否已设置
                object draw_ib_object = DataGridView_BasicIBList.Rows[e.RowIndex].Cells[0].Value;
                if (draw_ib_object == null)
                {
                    ShowMessageBox("Please fill the DrawIB", "请填写用于绘制的IB的哈希值");
                    //MessageBox.Show("Please fill the DrawIB");
                    return;
                }
                string draw_ib = draw_ib_object.ToString();
                if (string.IsNullOrEmpty(draw_ib))
                {
                    ShowMessageBox("Please fill the DrawIB", "请填写用于绘制的IB的哈希值");
                    //MessageBox.Show("Please fill the DrawIB");
                    return;
                }
                if (CurrentGameName == "")
                {
                    ShowMessageBox("Please choose your work game.", "请选择你当前工作的游戏");
                    //MessageBox.Show("Please choose your work game.");
                    return;
                }

                //设置好DrawIB和游戏类型才能执行后面的选项
                //先调出配置界面
                ConfigMod anotherForm = new ConfigMod(draw_ib, CurrentGameName);
                anotherForm.ShowDialog();

                //然后根据是否配置完成来显示对应信息
                if (File.Exists(Path_Game_ConfigJson))
                {
                    string jsonData = File.ReadAllText(Path_Game_ConfigJson);
                    JArray jsonArray = JArray.Parse(jsonData);
                    bool find_draw_ib = false;
                    foreach (JObject obj in jsonArray)
                    {
                        string drawIB = (string)obj["DrawIB"];
                        if (draw_ib == drawIB)
                        {
                            find_draw_ib = true;
                            break;
                        }

                    }

                    if (find_draw_ib)
                    {
                        //设置按钮显示为已配置
                        DataGridView_BasicIBList.Rows[e.RowIndex].Cells[e.ColumnIndex].Value = "Configured";
                    }
                    else
                    {
                        DataGridView_BasicIBList.Rows[e.RowIndex].Cells[e.ColumnIndex].Value = "Unconfigured";
                    }

                }

            }
        }

        private void DataGridViewIBList_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex == 0 && e.RowIndex >= 0)
            {
                //如果当前行这个button显示为空，则设置显示为未配置
                if (DataGridView_BasicIBList.Rows[e.RowIndex].Cells[1].Value == null)
                {
                    DataGridViewCell buttonCell = DataGridView_BasicIBList.Rows[e.RowIndex].Cells[1];
                    if (CurrentLanguage == "zh-cn")
                    {
                        buttonCell.Value = "未配置";
                    }
                    else
                    {
                        buttonCell.Value = "Unconfigured";
                    }
                }

                //如果当前行的DrawIB显示为空，则设置显示也为空
                if (DataGridView_BasicIBList.Rows[e.RowIndex].Cells[0].Value == null)
                {

                    DataGridViewCell buttonCell = DataGridView_BasicIBList.Rows[e.RowIndex].Cells[1];
                    buttonCell.Value = "";
                }
                else
                {
                    //TODO 在Config.json中查找是否有这个DrawIB，如果没有则显示为未配置，和上面差不多

                    if (File.Exists(this.Path_Game_ConfigJson))
                    {
                        string jsonData = File.ReadAllText(this.Path_Game_ConfigJson);
                        JArray jsonArray = JArray.Parse(jsonData);

                        bool findDrawIB = false;
                        foreach (JObject obj in jsonArray)
                        {
                            string drawIB = (string)obj["DrawIB"];
                            if (drawIB == DataGridView_BasicIBList.Rows[e.RowIndex].Cells[0].Value.ToString())
                            {
                                findDrawIB = true;
                                break;
                            }
                        }

                        if (!findDrawIB)
                        {
                            DataGridViewCell buttonCell = DataGridView_BasicIBList.Rows[e.RowIndex].Cells[1];
                            buttonCell.Value = "";
                        }
                    }
                }

            }
        }

        //打开设置界面
        private void configToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(CurrentGameName))
            {
                ShowMessageBox("Please select your game before open config", "在打开配置之前请您先选择一种游戏名称");
                //MessageBox.Show("在打开配置之前请您先选择一种游戏名称");
            }
            else
            {
                ConfigGame configBasic = new ConfigGame();
                configBasic.ShowDialog();
                setCurrentGame(this.CurrentGameName);
            }

        }



        //清空DrawIB界面
        private void 清空ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DataGridView_BasicIBList.Rows.Clear();
            saveConfig();

        }
        //保存DrawIB界面
        private void 保存ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool saveResult = saveConfig();
            if (saveResult)
            {
                ShowMessageBox("Save Success","保存成功");
                //MessageBox.Show("Save Success");
            }
            else
            {
                ShowMessageBox("Save failed, please check your config.", "保存失败，请检查你的配置");
                //MessageBox.Show("Save failed, please check your config.");
            }
        }
        //提取模型
        private void mergeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            preDoBeforeMerge();
            bool command_run_result = runCommand("merge");
            if (command_run_result)
            {
                Process.Start(this.Path_OutputFolder);
            }
        }



        //生成模型
        private void splitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            runCommand("split");
            backupFiles();
            //openModGenerateFolder();
        }

        private void DrawIB_清除当前配置ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DataGridView_BasicIBList.Rows.Clear();
            saveConfig();
        }

        private void DrawIB_保存配置ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            bool saveResult = saveConfig();
            if (saveResult)
            {
                ShowMessageBox("Save Success", "保存成功");

            }
            else
            {
                ShowMessageBox("Save Failed, please check your config", "保存失败，请检查配置");

                //MessageBox.Show("保存失败，请检查配置");
            }
        }



        private void File_打开Output文件夹ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(this.Path_OutputFolder))
            {
                if (Directory.Exists(Path_OutputFolder))
                {
                    Process.Start(this.Path_OutputFolder);
                }
                else
                {
                    ShowMessageBox("This folder doesn't exists,please check if your OutputFolder is correct.", "此目录不存在，请检查您的Output文件夹是否设置正确");
                    //MessageBox.Show("此目录不存在，请检查您的Output文件夹是否设置正确");
                }
            }
        }

        void openModGenerateFolder()
        {
            if (!string.IsNullOrEmpty(this.Path_OutputFolder))
            {
                DateTime currentDate = DateTime.Now;
                string formattedDate = currentDate.ToString("yyyy_MM_dd");
                string targetFolder = this.Path_OutputFolder + "/" + formattedDate + "/";
                if (Directory.Exists(targetFolder))
                {
                    Process.Start(targetFolder);
                }
                else
                {
                    ShowMessageBox("You have not generate any mod yet", "您还未生成二创模型");
                    //MessageBox.Show("您还未生成二创模型");
                }
            }
        }

        private void File_打开二创模型输出文件夹ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openModGenerateFolder();
        }

        private void File_打开工具所在目录ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start(this.basePath);
        }

        private void 打开Logs目录ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start(this.basePath + "Logs\\");
        }

        private void 打开Mods目录ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string modsFolder = this.Path_LoaderFolder + "Mods/";
            if (!string.IsNullOrEmpty(modsFolder))
            {
                if (Directory.Exists(modsFolder))
                {
                    Process.Start(modsFolder);
                }
                else
                {
                    ShowMessageBox("This path didn't exists, please check if your Mods folder is correct", "此目录不存在，请检查您的Mods文件夹是否设置正确");
                    //MessageBox.Show("此目录不存在，请检查您的Mods文件夹是否设置正确");
                }
            }
        }

        private void 打开最新的FrameAnalysis文件夹ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string[] directories = Directory.GetDirectories(Path_LoaderFolder);
                List<string> frameAnalysisFileList = new List<string>();
                foreach (string directory in directories)
                {
                    string directoryName = Path.GetFileName(directory);

                    if (directoryName.StartsWith("FrameAnalysis-"))
                    {
                        frameAnalysisFileList.Add(directoryName);
                    }
                }

                //
                if (frameAnalysisFileList.Count > 0)
                {
                    frameAnalysisFileList.Sort();

                    string latestFrameAnalysisFolder = Path_LoaderFolder.Replace("/", "\\") + frameAnalysisFileList.Last();

                    Process.Start( latestFrameAnalysisFolder);
                    //MessageBox.Show(latestFrameAnalysisFolderName);
                }
                else
                {
                    ShowMessageBox("Target directory didn't have any FrameAnalysisFolder.","目标目录没有任何帧分析文件夹");
                    //MessageBox.Show("Target directory didn't have any FrameAnalysisFolder.");
                }


            }
            catch (IOException ex)
            {
                MessageBox.Show("An IO exception has occurred: " + ex.Message);
            }
            catch (UnauthorizedAccessException ex)
            {
                MessageBox.Show("You do not have permission to access one or more folders: " + ex.Message);
            }
            catch (Exception ex)
            {
                MessageBox.Show("An unexpected exception has occurred: " + ex.Message);
            }
        }

        private void 打开3Dmigoto文件夹ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(Path_LoaderFolder))
            {
                Process.Start( Path_LoaderFolder.Replace("/", "\\"));
            }
            else
            {
                ShowMessageBox("This directory doesn't exists.","此目录不存在，请检查3Dmigoto文件夹是否配置正确");
                //MessageBox.Show("This directory doesn't exists.");
            }
        }


        private void 打开ShaderFixs文件夹ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string modsFolder = this.Path_LoaderFolder + "ShaderFixes/";
            if (!string.IsNullOrEmpty(modsFolder))
            {
                if (Directory.Exists(modsFolder))
                {
                    Process.Start(modsFolder.Replace("/","\\"));
                }
                else
                {
                    ShowMessageBox("This folder didn't exsits,please check if your ShaderFixes folder is correct.", "此目录不存在，请检查您的ShaderFixs文件夹是否设置正确");
                    //MessageBox.Show("此目录不存在，请检查您的ShaderFixs文件夹是否设置正确");
                }
            }
        }



        private void Main_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (AutoCleanFrameAnalysisFolder)
            {
                cleanFrameAnalysisFiles();

            }
            if (AutoCleanLogFile)
            {
                cleanLogFiles();

            }
        }



        private void openLatestLOGFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openLatestLogFile();
        }

        private void 简体中文ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            switchToChinese();
            setCurrentGame(CurrentGameName);
        }

        private void englishToolStripMenuItem_Click(object sender, EventArgs e)
        {
            switchToEnglish();
            setCurrentGame(CurrentGameName);

        }



        private void Menu_File_OpenBackupsFolder_ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start(this.basePath + "Backups\\");
        }

        private void Menu_File_OpenConfigsFolder_ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start(this.basePath + "Configs\\");
        }

        private void Menu_File_Open3DmigotosD3dxini_ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Directory.Exists(Path_LoaderFolder))
            {
                Process.Start(Path_LoaderFolder.Replace("/", "\\") + "d3dx.ini") ;
            }
            else
            {
                ShowMessageBox("Your 3Dmigoto directory doesn't exists.", "您的3Dmigoto目录不存在，请检查3Dmigoto文件夹是否配置正确");
            }
        }

        private void Menu_GameName_toolStripComboBox_TextChanged(object sender, EventArgs e)
        {
            setCurrentGame(Menu_GameName_toolStripComboBox.Text.ToString());
        }

        private void Menu_checkForUpdates_ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start("https://github.com/StarBobis/MigotoModTool/releases");
        }

        

        void preDoBeforeMerge()
        {
            if (this.DeleteOutputFolder)
            {
                if (Directory.Exists(this.Path_OutputFolder))
                {
                    Directory.Delete(this.Path_OutputFolder, true);
                    Directory.CreateDirectory(this.Path_OutputFolder);
                }

            }

            //Before we run extract, we need to check if all the draw ib is configured.
            foreach (DataGridViewRow row in DataGridView_BasicIBList.Rows)
            {
                if (!row.IsNewRow && row.Cells[1].Value != null)
                {
                    string configStatus = row.Cells[1].Value.ToString();
                    if (configStatus == "Unconfigured")
                    {
                        ShowMessageBox("Please config your DrawIB's config before run.", "在运行之前请配置一下您的绘制IB的配置");
                        return;
                    }
                }
            }

            if (DataGridView_BasicIBList.Rows.Count == 1)
            {
                ShowMessageBox("Please fill your DrawIB and config it before run.", "在运行之前请填写您的绘制IB的哈希值并进行配置");
                return;
            }


            initializeFolders();

            

        }

        private void button_extractReverseModel_Click(object sender, EventArgs e)
        {
            preDoBeforeMerge();
            bool command_run_result = runCommand("mergeReverse");
            if (command_run_result)
            {
                Process.Start(this.Path_OutputFolder);
            }
        }

        private void extractReverseModelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            preDoBeforeMerge();
            bool command_run_result = runCommand("mergeReverse");
            if (command_run_result)
            {
                Process.Start(this.Path_OutputFolder);
            }
        }


        void RunReverseCommand(string commandStr)
        {
            if (string.IsNullOrEmpty(CurrentGameName))
            {
                ShowMessageBox("Please select your current game before reverse.", "在逆向Mod之前请选择当前要进行格式转换的二创模型的所属游戏");
                return;
            }

            openFileDialog1.Filter = "INI Files (*.ini)|*.ini";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                string filePath = openFileDialog1.FileName;
                string json = File.ReadAllText(this.Path_RunInputJson); // 读取文件内容
                JObject runInputJson = JObject.Parse(json);
                runInputJson["GameName"] = CurrentGameName;
                runInputJson["ReverseFilePath"] = filePath;
                File.WriteAllText(this.Path_RunInputJson, runInputJson.ToString());

                runCommand(commandStr);
            }
        }

        private void reverseOutfitCompilerCompressedMergedModToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RunReverseCommand("reverseOutfitCompiler");
        }

        private void reverseSingleModToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RunReverseCommand("reverseSingle");
        }

        private void reverseMergedModToolStripMenuItem_Click(object sender, EventArgs e)
        {
            RunReverseCommand("reverseMerged");
        }

        private void Menu_Run_run3DmigotoLoaderexeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string LoaderExePath = this.Path_LoaderFolder + "3Dmigoto Loader.exe";
            if (File.Exists(LoaderExePath))
            {
                ProcessStartInfo startInfo = new ProcessStartInfo(LoaderExePath);
                startInfo.Verb = "runas";
                startInfo.UseShellExecute = false;
                startInfo.WorkingDirectory = Path.GetDirectoryName(LoaderExePath);
                Process.Start(startInfo);
            }
            else
            {
                ShowMessageBox("Can't find 3Dmigoto Loader.exe in your game's [3Dmigoto] folder.","在您当前游戏的[3Dmigoto]目录中未找到3Dmigoto Loader.exe");
            }
        }

        private void Menu_Run_run3DmigotoLoaderByPassACEexeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string LoaderExePath = this.Path_LoaderFolder + "3Dmigoto Loader-ByPassACE.exe";
            if (File.Exists(LoaderExePath))
            {
                ProcessStartInfo startInfo = new ProcessStartInfo(LoaderExePath);
                startInfo.Verb = "runas";
                startInfo.WorkingDirectory = Path.GetDirectoryName(LoaderExePath);
                startInfo.UseShellExecute = false;
                Process.Start(startInfo);
            }
            else
            {
                ShowMessageBox("Can't find 3Dmigoto Loader-ByPassACE.exe in your game's [3Dmigoto] folder.", "在您当前游戏的[3Dmigoto]目录中未找到3Dmigoto Loader-ByPassACE.exe");
            }
        }

        private void developedByNicoMicoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
            Process.Start("https://afdian.net/a/HSMico666");
        }

        private void openTypesFolderToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string TypeDirStr = "Games\\" + this.CurrentGameName + "\\Types\\";
            if (Directory.Exists(TypeDirStr))
            {
                Process.Start(TypeDirStr);
            }
        }
    }
}
