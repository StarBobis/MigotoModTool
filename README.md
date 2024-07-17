# MMT-Community
It's a tool developed only for fun, if you find MMT can't work good on specific game, try to switch to other tool instead.

It's designed to works with our modified fork version of blender_3dmigoto.py forked from DarkStarSword's 3DFix repo, download link: https://github.com/StarBobis/MMT-Blender-Plugin

It's designed to works with our modified fork version of 3Dmigoto originally forked from 3Dmigoto repo, download link: https://github.com/StarBobis/3Dmigoto-GameMod-Fork
(Our d3d11.dll has stop to develop and maintain due to no enough time, try SinsOfSeven and SpectrumQT's new open source code instead: https://github.com/SinsOfSeven/3Dmigoto)

Features:
- Reverse 3Dmigoto mod with a simple click, reverse extract mod model from game dump files. (Reverse function has been hide since V1.0.4.8 to avoid abuse and drama, you need to verify a MMT-Reverse role in our discord to unlock it.)
- Support multiple game's model extract and buffer mod make as it can.
- Save your time with user frendly GUI.
- Detailed log system, easy to debug and solve problems.
- Compatible with other similar tools.
- Fast POC.

This project is still work in process so make sure always use the latest release version,I only have less than 2 hours everyday to maintain it, so be patient and welcome to send me feedback.

# How to use
1.Download files from https://github.com/StarBobis/3Dmigoto-Loaders which is different game loader i have preconfigured dedicated for MMT usage,
then copy Games folder and put it to where our MMT.exe located, then you can run MMT-GUI.exe to start play.

2.Use IndexBuffer Hash copied from press Numpad9 in Hunting GUI to make mod.

3.Use MMT.zip as your blender's 3Dmigoto plugin, it's work on Blender 3.6 LTS (official release).
MMT.zip can be downloaded from : https://github.com/StarBobis/MMT-Blender-Plugin

You will need to pack the MMT folder into MMT.zip to install it into blender or download it from latest release.

4.How to easily upgrade to new version? Just copy all your new version MMT files and folders to your older version's directory and replace them all.

# Technique support
Discord: [https://discord.gg/eCXdNyVnsM](https://discord.gg/eCXdNyVnsM)

# Under Reconstruct
MMT's is write with shit code so it's under reconstruct and still WIP, new code will be open source later.
![image](https://github.com/user-attachments/assets/e1684d03-96e9-4209-915e-a7dad0b9ba8d)


MMT's dependency code is seperate out and open source in different repository for reuse purpose:

[https://github.com/StarBobis/MMT-Common](https://github.com/StarBobis/MMT-Common)

[https://github.com/StarBobis/MMT-Utils](https://github.com/StarBobis/MMT-Utils)

# Credit 
MMT's code keep learned from lots of similar projects and make improvement and upgrade, here is a list for main of them.
- https://github.com/SpectrumQT/WWMI-TOOLS/tree/main
- https://github.com/SilentNightSound/GI-Model-Importer
- https://github.com/SilentNightSound/SR-Model-Importer
- https://github.com/SilentNightSound/HI-Model-Importer
