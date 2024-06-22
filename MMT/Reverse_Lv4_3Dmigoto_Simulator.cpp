#include "GlobalConfigs.h"
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "ModFormatExtra.h"
#include "GlobalFunctions.h"
#include "IndexBufferBufFile.h"
#include "FmtData.h"

// This function is designed for any kinds of 3Dmigoto mod
// it will simulate 3Dmigoto's ini parsing logic and resource loading logic
// to fully simulate how a mod is loaded and running in 3Dmigoto
// then it try to extract every single mod it can extract from all files in target folder

// Notice:This method may take a lot of time if you use it in large mod folder.
//TODO Lv3已经过时了,完成这个之前需要先Toggle的解析支持
void Reverse_Lv4_3Dmigoto_Simulator() {


}