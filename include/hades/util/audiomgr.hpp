#ifndef HADES_UTIL_AUDIOMGR_HPP
#define HADES_UTIL_AUDIOMGR_HPP

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"

#include "libnyquist/Decoders.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>

namespace hades {

struct PlayOptions {
  float pitch{1.0f};    // valid range: 0.5 - 2.0
  float volume{1.0f};   // valid range: 0.0 - 1.0
  bool looping{false};
};

class AudioMgr {
public:
  AudioMgr();

  ~AudioMgr();

  void update();

  bool open_device(const std::string &device_name = "");
  bool reopen_device(const std::string &device_name = "");

  bool open_context();

  bool make_current();

  bool load(const std::string &name, const std::string &filename);

  void play(const std::string &name, const PlayOptions &options = {});

  const std::vector<std::string> &get_devices();

private:
  ALCcontext *ctx_{nullptr};

  ALCdevice *device_{nullptr};
  std::vector<std::string> devices_{};

  nqr::NyquistIO audio_loader_{};
  std::unordered_map<std::string, ALuint> buffers_{};
  std::unordered_map<std::string, std::list<ALuint>> sources_{};

  ALCboolean (ALC_APIENTRY *alcReopenDeviceSOFT_)(ALCdevice *device, const ALCchar *name, const ALCint *attribs);

  bool reopen_supported_{false};

  void get_available_devices_();

  bool check_al_errors();
  bool check_alc_errors(ALCdevice *device);
  bool check_alc_errors();
};

} // namespace hades

#endif //HADES_UTIL_AUDIOMGR_HPP
