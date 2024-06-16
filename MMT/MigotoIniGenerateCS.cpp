#include "MigotoIniFile.h"
#include "MMTStringUtils.h"
#include "MMTJsonUtils.h"
#include <cmath>


MigotoIniFile::MigotoIniFile() {

}


MigotoIniFile::MigotoIniFile(GlobalConfigs wheelConfig, ExtractConfig basicConfig, D3D11GameType d3d11GameType) {
    this->wheelConfig = wheelConfig;
    this->basicConfig = basicConfig;
    this->d3d11GameType = d3d11GameType;
}



