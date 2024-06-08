using Microsoft.VisualBasic.FileIO;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace NMBT_GUI
{
    partial class Main
    {
        public bool ContainsChinese(string input)
        {
            // 使用正则表达式匹配中文字符
            Regex regex = new Regex(@"[\u4e00-\u9fa5]");
            return regex.IsMatch(input);
        }

        private void cleanLogFiles()
        {
            string logsPath = basePath + "Logs";

            if (!Directory.Exists(logsPath))
            {
                return;
            }

            //移除log文件
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

            if (logFileList.Count == 0)
            {
                return;
            }

            logFileList.Sort();
            int n = this.LogFileReserveNumber; // 你想移除的元素数量
            if (n > 0 && logFileList.Count > n)
            {
                logFileList.RemoveRange(logFileList.Count - n, n);

            }
            else if (logFileList.Count <= n)
            {
                // 如果 n 大于等于列表的长度，就清空整个列表
                logFileList.Clear();
            }
            if (logFileList.Count > 0)
            {
                foreach (string logfileName in logFileList)
                {
                    File.Delete(logsPath + "\\" + logfileName);

                    //移动到回收站有时无法生效
                    //FileSystem.DeleteFile();
                    //Directory.Delete(latestFrameAnalysisFolder, true);
                }
            }
        }


        private void cleanFrameAnalysisFiles()
        {
            if (!Directory.Exists(Path_LoaderFolder))
            {
                return;
            }

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

            if (frameAnalysisFileList.Count == 0)
            {
                return;
            }

            //Get FA numbers to reserve
            frameAnalysisFileList.Sort();

            int n = this.FrameAnalysisFolderReserveNumber; // 你想移除的元素数量
            if (n > 0 && frameAnalysisFileList.Count > n)
            {
                frameAnalysisFileList.RemoveRange(frameAnalysisFileList.Count - n, n);

            }
            else if (frameAnalysisFileList.Count <= n)
            {
                // 如果 n 大于等于列表的长度，就清空整个列表
                frameAnalysisFileList.Clear();
            }
            if (frameAnalysisFileList.Count > 0)
            {
                foreach (string directoryName in frameAnalysisFileList)
                {
                    string latestFrameAnalysisFolder = Path_LoaderFolder.Replace("/", "\\") + directoryName;
                    //FileSystem.DeleteDirectory(latestFrameAnalysisFolder, UIOption.AllDialogs, RecycleOption.SendToRecycleBin);
                    Directory.Delete(latestFrameAnalysisFolder, true);
                }
            }

        }



    }
}
