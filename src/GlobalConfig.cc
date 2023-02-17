#include <QString>

#include "GlobalConfig.h"
#include "util/FileUtil.h"
#include "util/IniLoader.h"

namespace cw {

GlobalConfig GlobalConfig::Instance;

void InitGlobalConfig() {
  QString iniFileContent = ReadToString("config.ini");
  IniFileData config = IniFileData::Parse(iniFileContent);

  IniSection const* commonConfig = config.GetSection("common");
  if (commonConfig) {
    GlobalConfig::Instance.stayOnTop =
      commonConfig->GetBoolValue("stay_on_top");
    GlobalConfig::Instance.fillBackground =
      commonConfig->GetBoolValue("fill_background");

    GlobalConfig::Instance.backgroundColor.r =
      commonConfig->GetFloatValue("background_r");
    GlobalConfig::Instance.backgroundColor.g =
      commonConfig->GetFloatValue("background_g");
    GlobalConfig::Instance.backgroundColor.b =
      commonConfig->GetFloatValue("background_b");
  }

  IniSection const* renderConfig = config.GetSection("render");
  if (renderConfig) {
    GlobalConfig::Instance.multisampling =
      renderConfig->GetBoolValue("multisampling");
    GlobalConfig::Instance.multisamplingSamples =
      renderConfig->GetIntValue("multisampling_samples");
    GlobalConfig::Instance.lineSmoothHint =
      renderConfig->GetBoolValue("line_smooth");
    GlobalConfig::Instance.anisotropyFilter =
      renderConfig->GetBoolValue("anisotropy_filter");
  }

  IniSection const* controlConfig = config.GetSection("control");
  if (controlConfig) {
    QString defaultControlMode = controlConfig->GetData("default_mode").toLower();
    if (defaultControlMode == "vts") {
      GlobalConfig::Instance.defaultControlMode = GlobalConfig::ControlMode::VTS;
    } else if (defaultControlMode == "osf") {
      GlobalConfig::Instance.defaultControlMode = GlobalConfig::ControlMode::OSF;
    } else if (defaultControlMode == "mp") {
      GlobalConfig::Instance.defaultControlMode = GlobalConfig::ControlMode::MP;
    } else if (defaultControlMode == "manual") {
      GlobalConfig::Instance.defaultControlMode = GlobalConfig::ControlMode::Manual;
    } else {
      GlobalConfig::Instance.defaultControlMode = GlobalConfig::ControlMode::None;
    }
  }

  IniSection const* vtsConfig = config.GetSection("control.vts");
  if (vtsConfig) {
    GlobalConfig::Instance.vtsWebsocketPort = vtsConfig->GetIntValue("websocket_port");
  }

  IniSection const* osfConfig = config.GetSection("control.osf");
  if (osfConfig) {
    GlobalConfig::Instance.osfUdpPort = osfConfig->GetIntValue("udp_port");
    GlobalConfig::Instance.osfCorrectionX = osfConfig->GetFloatValue("correction_x");
    GlobalConfig::Instance.osfCorrectionY = osfConfig->GetFloatValue("correction_y");
    GlobalConfig::Instance.osfCorrectionZ = osfConfig->GetFloatValue("correction_z");
    GlobalConfig::Instance.osfSmooth = osfConfig->GetIntValue("smooth");
  }
}

char const *
GlobalConfig::ControlModeToString(GlobalConfig::ControlMode mode) {
  switch (mode) {
    case ControlMode::None:
      return "none";
    case ControlMode::VTS:
      return "vts";
    case ControlMode::OSF:
      return "osf";
    case ControlMode::MP:
      return "mp";
    case ControlMode::Manual:
      return "manual";
  }
}

} // namespace cw
