using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NMBT_GUI
{
    partial class Main
    {
        public void readConfig(string configPath)
        {
            //判断MainSetting.json是否存在
            if (File.Exists(configPath))
            {
                string json = File.ReadAllText(configPath); // 读取文件内容
                JObject jsonObject = JObject.Parse(json);
                string gameName = (string)jsonObject["GameName"];
                // 设置当前游戏类型
                if (gameName != "")
                {
                    Menu_GameName_toolStripComboBox.Text = gameName;


                    //这里不需要调用设置Game，因为Combobox.Text改变后会调用设置game
                    //setCurrentGame(gameName);
                }
            }
        }

        public void saveCurrentGameNameAndGameType()
        {
            if (File.Exists(Path_MainConfig))
            {
                string json = File.ReadAllText(Path_MainConfig); // 读取文件内容
                JObject jsonObject = JObject.Parse(json);
                jsonObject["GameName"] = CurrentGameName;
                File.WriteAllText(Path_MainConfig, jsonObject.ToString());
            }
            else
            {
                JObject jsonObject = new JObject();
                jsonObject["GameName"] = CurrentGameName;
                File.WriteAllText(Path_MainConfig, jsonObject.ToString());
            }
        }

        public bool saveConfig()
        {
            //(1) 检查游戏类型是否设置
            if (CurrentGameName == "")
            {
                ShowMessageBox("Please select a game before this.", "请先选择游戏类型");
                return false;
            }

            //(2) 接下来要把当前的游戏名称+类型保存到MainSetting.json里
            saveCurrentGameNameAndGameType();

            //(3) 接下来把所有的drawIBList中的合法的保留下来存储到对应配置文件。
            List<string> drawIBList = new List<string>();
            foreach (DataGridViewRow row in DataGridView_BasicIBList.Rows)
            {
                if (!row.IsNewRow && row.Cells[0].Value != null)
                {
                    string drawIB = row.Cells[0].Value.ToString();
                    drawIBList.Add(drawIB);
                }
            }

            //因为在ConfigMod窗口里，已经进行了对应的DrawIB的保存，所以在这里只需要去除不在DrawIBList里的就行了。
            JArray goodJarray = new JArray();

            //已存在，则只保留出现在DrawIBList里的
            if (File.Exists(this.Path_Game_ConfigJson))
            {
                string jsonData = File.ReadAllText(this.Path_Game_ConfigJson);
                JArray jsonArray = JArray.Parse(jsonData);

                foreach (JObject obj in jsonArray)
                {
                    string drawIB = (string)obj["DrawIB"];
                    if (drawIBList.Contains(drawIB))
                    {
                        goodJarray.Add(obj);
                    }
                }

                //保存覆盖
                // 将JObject转换为JSON字符串
                string json_string = goodJarray.ToString(Formatting.Indented);

                // 将JSON字符串写入文件
                File.WriteAllText(this.Path_Game_ConfigJson, json_string);
                return true;

            }

            return true;

        }

    }
}
