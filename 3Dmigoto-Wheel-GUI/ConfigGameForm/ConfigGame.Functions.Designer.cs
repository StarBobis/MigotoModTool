using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace NMBT_GUI
{
    partial class ConfigGame
    {
        

        public void resetConfig()
        {
            checkBoxBackUp.Checked = true;
            checkBoxBackUp.Checked = false;

            checkBoxDeleteOutputFolder.Checked = true;
            checkBoxDeleteOutputFolder.Checked = false;


            checkBoxAutoCleanLog.Checked = true;
            checkBoxAutoCleanLog.Checked = false;
            textBoxLogReserveNumber.Text = string.Empty;

            checkBoxAutoCleanFrameAnalysisFolder.Checked = true;
            checkBoxAutoCleanFrameAnalysisFolder.Checked = false;
            textBoxFrameAnalysisFolderReserveNumber.Text = string.Empty;

        }


        public void readConfig()
        {
            //读取json配置文件
            
            if (File.Exists(Path_Setting))
            {
                string json = File.ReadAllText(Path_Setting); // 读取文件内容
                JObject gameObject = JObject.Parse(json);

                 
                checkBoxBackUp.Checked = (bool)gameObject["BackUp"];
                checkBoxDeleteOutputFolder.Checked = (bool)gameObject["DeleteOutputFolder"];
                  

                //自动清理FrameAnalysis文件夹
                bool autoCleanFrameAnalysisFolder = (bool)gameObject["AutoCleanFrameAnalysisFolder"];
                int frameAnalysisFolderReserveNumber = (int)gameObject["FrameAnalysisFolderReserveNumber"];
                if (autoCleanFrameAnalysisFolder)
                {
                    checkBoxAutoCleanFrameAnalysisFolder.Checked = true;
                    textBoxFrameAnalysisFolderReserveNumber.Text = frameAnalysisFolderReserveNumber + "";
                }

                bool autoCleanLogFile = (bool)gameObject["AutoCleanLogFile"];
                int logFileReserveNumber = (int)gameObject["LogFileReserveNumber"];
                if (autoCleanLogFile)
                {
                    checkBoxAutoCleanLog.Checked = true;
                    textBoxLogReserveNumber.Text = logFileReserveNumber + "";
                }
                   
            }

            //读取完如果还是空的，那就设置默认值
            string loaderFolderPath = basePath + "3Dmigoto\\" + current_game +  "\\";
            string outputFolderPath = loaderFolderPath + "Mods\\output\\";
            
        }


        public bool saveConfig()
        {
            //检查数量是否输入完成要保留的数字
            if (checkBoxAutoCleanFrameAnalysisFolder.Checked)
            {
                if (string.IsNullOrEmpty(textBoxFrameAnalysisFolderReserveNumber.Text))
                {
                    ShowMessageBox("Please fill reserve number for FrameAnalysisFolder.","请输入要保留的FrameAnalysisFolder的数量");
                    return false;
                }
            }

            if (checkBoxAutoCleanLog.Checked)
            {
                if (string.IsNullOrEmpty(textBoxLogReserveNumber.Text))
                {
                    ShowMessageBox("Please fill reserve number for log files.", "请输入要保留的日志文件的数量");
                    return false;
                }
            }


            if (File.Exists(Path_Setting))
            {
                //获取游戏类型
                JObject gameObject = new JObject();

                gameObject["BackUp"] = checkBoxBackUp.Checked;
                gameObject["DeleteOutputFolder"] = checkBoxDeleteOutputFolder.Checked;
               

                if (checkBoxAutoCleanFrameAnalysisFolder.Checked)
                {
                    gameObject["AutoCleanFrameAnalysisFolder"] = true;
                    gameObject["FrameAnalysisFolderReserveNumber"] = int.Parse(textBoxFrameAnalysisFolderReserveNumber.Text);
                }
                else
                {
                    gameObject["AutoCleanFrameAnalysisFolder"] = false;
                    gameObject["FrameAnalysisFolderReserveNumber"] = 0;
                }

                if (checkBoxAutoCleanLog.Checked)
                {
                    gameObject["AutoCleanLogFile"] = true;
                    gameObject["LogFileReserveNumber"] = int.Parse(textBoxLogReserveNumber.Text);
                }
                else
                {
                    gameObject["AutoCleanLogFile"] = false;
                    gameObject["LogFileReserveNumber"] = 0;
                }


                // 将JObject转换为JSON字符串
                string json_string = gameObject.ToString(Formatting.Indented);

                // 将JSON字符串写入文件
                File.WriteAllText(Path_Setting, json_string);

            }
            else
            {
                MessageBox.Show("Didn't exists");
            }
            return true;
        }


    }
}
