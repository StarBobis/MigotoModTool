using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NMBT_GUI
{
    partial class ConfigGame
    {
        void ShowMessageBox(string EnglishTip, string ChineseTip)
        {
            if (currentLanguage == "en")
            {
                MessageBox.Show(EnglishTip);
            }
            else if (currentLanguage == "zh-cn")
            {
                MessageBox.Show(ChineseTip);
            }
        }
    
        void switchToChinese()
        {
            checkBoxAutoCleanLog.Text = "在退出MMT之前自动清理生成的日志文件，保留数量：";
            checkBoxAutoCleanFrameAnalysisFolder.Text = "在退出MMT之前自动清理生成的帧分析文件夹，保留数量：";
            checkBoxDeleteOutputFolder.Text = "在提取模型之前自动删除Output文件夹";
            checkBoxBackUp.Text = "在生成二创模型之前自动备份之前的Output文件夹";
            
        }
    
    }
}
