using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NMBT_GUI
{
    public partial class ConfigMod : Form
    {
        private string Path_Languages_Json = "Configs\\Languages.json";
        private string DrawIB = "";
        private string CurrentGame = "";
        private string Path_Game_ConfigJson = "";
        private Dictionary<string, List<string>> game_type_dict = new Dictionary<string, List<string>>();
        private string OutputFolder = "";
        string currentLanguage;

        public ConfigMod()
        {
            InitializeComponent();
        }

        public ConfigMod(string DrawIB, string CurrentGame)
        {
            InitializeComponent();
            this.DrawIB = DrawIB;
            this.CurrentGame = CurrentGame;
            this.OutputFolder = "Games\\" + CurrentGame + "\\3Dmigoto\\Mods\\output";

            //读取并设置语言
            string json = File.ReadAllText(Path_Languages_Json); // 读取文件内容
            JObject jsonObject = JObject.Parse(json);
            currentLanguage = (string)jsonObject["language"];

            if (currentLanguage == "en")
            {
                switchToEnglish();

            }
            else if (currentLanguage == "zh-cn")
            {
                switchToChinese();

            }

            this.Path_Game_ConfigJson = "Games\\" + this.CurrentGame + "\\Config.json";
            //读取GameListSetting.json文件里的游戏类型列表

            string[] game_types = Directory.GetFiles("Games\\" + this.CurrentGame + "\\Types\\");
            List<string> currentGameTypeList = new List<string>();

            if (this.CurrentGame == "WW")
            {
                currentGameTypeList.Add("Auto");
            }
            else
            {
                currentGameTypeList.Add("Auto");
            }

            foreach (string game_type in game_types)
            {

                string game_type_filename = Path.GetFileName(game_type);
                string game_type_name = game_type_filename.Substring(0, game_type_filename.Length - 5);
                currentGameTypeList.Add(game_type_name);
            }
            game_type_dict.Add(this.CurrentGame, currentGameTypeList);


            

            //接下来根据传递的DrawIB读取配置文件，如果不存在配置，则为从新开始配置。
            resetConfig();

            //游戏类型要从固定的游戏类型中读取
            comboBoxGameType.Items.Clear();
            comboBoxGameType.Items.AddRange(game_type_dict[this.CurrentGame].ToArray());
            comboBoxGameType.SelectedIndex = 0;

            readConfig();
        }

        private void ConfigMod_Load(object sender, EventArgs e)
        {
            
        }

        private void 初始化ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            resetConfig();
        }

       
        
        private void ConfigMod_FormClosed(object sender, FormClosedEventArgs e)
        {
            
            bool checkResult = checkConfig();
            if (checkResult)
            {
                saveConfig();

                //不再弹出提示，而是在保存的过程中如果有报错，则弹出报错，否则无痕保存
                //必须有反馈提示
                //ShowMessageBox("Save success!", "保存成功");
                //MessageBox.Show("保存成功");
            }
            else
            {
                ShowMessageBox("Save failed! Please check your config!", "保存失败，请检查您的配置！");
            }
        }

        private void checkBoxColorRGBR_CheckedChanged(object sender, EventArgs e)
        {
            textBoxColorRGBR.Enabled = checkBoxColorRGBR.Checked;
        }



        private void checkBoxColorRGBB_CheckedChanged(object sender, EventArgs e)
        {
            textBoxColorRGBB.Enabled = checkBoxColorRGBB.Checked;
        }

        private void checkBoxColorRGBG_CheckedChanged(object sender, EventArgs e)
        {
            textBoxColorRGBG.Enabled = checkBoxColorRGBG.Checked;
        }

        private void checkBoxColorRGBA_CheckedChanged(object sender, EventArgs e)
        {
            textBoxColorRGBA.Enabled = checkBoxColorRGBA.Checked;
        }

        private void checkBoxTANGENT_CheckedChanged(object sender, EventArgs e)
        {
            comboBoxTANGENT.Enabled =   checkBoxTANGENT.Checked;
        }

        private void checkBoxSwitchKey_CheckedChanged(object sender, EventArgs e)
        {
            textBoxSwitchKey.Enabled = checkBoxSwitchKey.Checked;
        }


        private void checkBoxColorRecalculate_CheckedChanged(object sender, EventArgs e)
        {
            comboBoxCOLORAlgorithm.Enabled = checkBoxColorRecalculate.Checked;
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void hSRToolStripMenuItem_Click(object sender, EventArgs e)
        {
            checkBoxColorRGBR.Checked = true;
            checkBoxColorRGBG.Checked = true;
            checkBoxColorRGBB.Checked = true;
            checkBoxColorRGBA.Checked = true;
            textBoxColorRGBR.Text = "255";
            textBoxColorRGBG.Text = "1";
            textBoxColorRGBB.Text = "128";
            textBoxColorRGBA.Text = "77";
        }
    }
}
