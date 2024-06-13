# Project design
- C++ 20标准
- 使用Boost库
- 使用Json for morden C++ 库处理Json文件 https://github.com/nlohmann/json
- 使用easyloggingpp库处理日志记录
- Visual Studio Community 2022 & Platform Set V143

# 代码规范
- 尽量使用string，部分特殊情况才考虑使用wstring，以做到兼容
- 所有工具类以MMT开头以Utils结尾，方法名要拆分写在.h 和.cpp中，要做到复制粘贴.h和.cpp文件就能在其它项目中使用
- 导包时用到什么include什么，不要提前include
- 禁止使用using namespace防止命名空间混淆，太长的话就用工具类进行包裹，简化接口，但绝对不能使用using namespace
- 工具类中的方法要以MMT开头+功能类型+下划线，驼峰命名，比如MMTString_To_Wide_String() , MMTFile_Exists()
- 重视每一个警告，尽可能去解决，解决不了的可以尝试寻找替代方法，尽量不要除第三方代码外的自己的代码出现警告。
- 工具类中的函数如果处理的是宽字符串类型，则名称必须以W结尾
- 禁止使用boost::algorithm::split，因为有严重bug无法以L"=="作为分隔符正常分割wstring.
- 复制文件要使用std::filesystem::copy_file
- Boost.log和easyloggingpp都只能使用std::string都不能使用std::wstring.
- 不能一个解决方案多个项目，尤其是多个项目之间文件名和方法名都差不多，会导致Visual Studio无法正确分析符号链接，无法正确的使用对应的符号。

# Build from source code
Some preconfig you will need to have:
- VisualStudio 2022 VS143 toolset. 
- Install Boost 1.84.0  (or latest version)
- (1) Download Boost 1.84.0 and unzip it to somewhere.
- (2) Open your terminal and switch into your boost unziped folder.
- (3) to initialize boost config, type this command in your terminal: .\bootstrap.bat
- (4) then type this in your terminal to build boost library for MMT:

- .\b2.exe variant=release link=static runtime-link=static threading=multi runtime-debugging=off
- (5) Change your project's setting path, on my computer i put the unziped boost on C:\AppLibraries, so for my computer it's path looks like below,but you need to change it to your own path:
The following directory should be added to compiler include paths:

    C:\AppLibraries\boost_1_84_0

The following directory should be added to linker library paths:

    C:\AppLibraries\boost_1_84_0\stage\lib

After these steps above ,you will be able to build MMT on X64 DEBUG or RELEASE, x32-bit is also work but not official supported by MMT for low speed reason.

# MMT Third party used:
- easylogging
- Json for morden C++ (nlohmann json)
- Boost.
MMT-GUI will need these packages, you need to install it in NuGet manager.
- Newtonsoft.Json


# Join development and you can make it better!