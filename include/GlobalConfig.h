#ifndef PROJECT_WG_GLOBAL_CONFIG_H
#define PROJECT_WG_GLOBAL_CONFIG_H

namespace cw {

struct GlobalConfig {
  bool multisampling = true;
  int multisamplingSamples = 8;
  bool lineSmoothHint = true;
  bool anisotropyFilter = true;

  enum class ControlMode {
    None, VTS, OSF, MP, Manual
  };
  ControlMode defaultControlMode = ControlMode::None;

  int vtsWebsocketPort = 8001;

  int osfUdpPort = 11573;
  float osfCorrectionX = 0;
  float osfCorrectionY = 0;
  float osfCorrectionZ = 0;
  int osfSmooth = 8;

  static GlobalConfig Instance;
  static char const* ControlModeToString(ControlMode mode);
};

void InitGlobalConfig();

} // namespace cw

#endif // PROJECT_WG_GLOBAL_CONFIG_H
