# MMT社区版
![image](https://github.com/user-attachments/assets/e1684d03-96e9-4209-915e-a7dad0b9ba8d)

本工具的开发仅为娱乐性质，如果你发现MMT不太好用，请切换到其它专业的Mod工具。

MMT需配套MMT的Blender插件使用，下载地址：[https://github.com/StarBobis/MMT-Blender-Plugin](https://github.com/StarBobis/MMT-Blender-Plugin)

MMT需配套我们的3Dmigoto-Armor版本的d3d11.dll使用，部分技术已开源至[https://github.com/StarBobis/3Dmigoto-GameMod-Fork](https://github.com/StarBobis/3Dmigoto-GameMod-Fork)

特性列表:
- 一键逆向提取任意3Dmigoto Mod中的模型，从游戏Dump中逆向提取Mod模型
- 支持多个游戏，但只需一套简单的操作流程。
- 全图形化界面操作，方便易上手
- 详细的日志系统，便于排查问题。
- 和大部分其它Mod工具兼容。
- 快速迭代开发。

此项目仍在不断开发测试改进中，仅使用我业余时间为爱发电开发，如有疑问和建议以及BUG提交，请联系我或在Github的issue中反馈，谢谢。

# 如何使用
1.从[https://github.com/StarBobis/3Dmigoto-Loaders](https://github.com/StarBobis/3Dmigoto-Loaders)这里下载我提前准备好的各个游戏的加载器，
然后把里面的Games文件夹复制到MMT.exe所在位置，运行MMT-GUI.exe即可使用。

2.MMT使用IndexBuffer来工作，小键盘0打开Hunting界面，小键盘7和8查找IB，小键盘9复制IB的Hash值，填到MMT里来用。

3.MMT需要安装我们配套的Blender插件来使用：[https://github.com/StarBobis/MMT-Blender-Plugin](https://github.com/StarBobis/MMT-Blender-Plugin)
你可以从Release里下载已经打包好的稳定版或者从master中下载MMT文件夹手动打包为MMT.zip来安装开发板。

4.旧版本MMT如何一键升级到新版本MMT?
把新版本MMT中所有文件直接复制并覆盖旧版本对应文件即可。

# 技术支持
中文技术支持：

加入BiliBili上的QQ粉丝群652223957 获取技术支持，注意QQ等级及综合素质达标才能加入，你也可以在赞助平台联系我加入MMT技术社群获取更全面的技术支持。

English Support:

Discord: [https://discord.gg/eCXdNyVnsM](https://discord.gg/eCXdNyVnsM)


# 部分开源声明
由于MMT重构后的新架构特别易于开发维护，为避免海盗人出现直接性的源码复制和工具倒卖行为，新版本MMT核心逻辑代码将不会开源，
新版本MMT只开源部分了公用数据类型和底层工具类，作为理解底层实现的意义已经足够，也可以作为你项目的依赖节省大量底层数据结构抽象时间。
相信你也可以理解这是为了防止直接性的和潜在的不遵守开源协议的商业使用情况出现，另外，请遵守开源协议使用MMT所有相关内容。

[https://github.com/StarBobis/MMT-Common](https://github.com/StarBobis/MMT-Common)

[https://github.com/StarBobis/MMT-Utils](https://github.com/StarBobis/MMT-Utils)

旧版本MMT源码在master分支中基于GPL3.0协议开源，不再更新，仅用于教学参考意义。

由于中文语言用户占据了95%以上，后续将不再提供英文版README，参考资料，代码注释等，请自行使用AI翻译软件阅读理解。

# 注意事项

为方便使用和增加功能集成，Release中将含有第三方闭源工具和闭源代码，如有介意，请勿使用。



