#include "baphomet/mgr/audiomgr.hpp"

#include "spdlog/spdlog.h"

//#if defined(BAPHOMET_PLATFORM_WINDOWS)
//#include <comdef.h>
//#endif

namespace baphomet {

AudioMgr::AudioMgr() {
//#if defined(BAPHOMET_PLATFORM_WINDOWS)
//  const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
//const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
//const HRESULT ok = CoCreateInstance(
//    CLSID_MMDeviceEnumerator, NULL,
//    CLSCTX_ALL, IID_IMMDeviceEnumerator,
//    (void **)&win32_dev_enumerator_
//);
//if (ok != S_OK) {
//  _com_error err(ok);
//  spdlog::error("Failed to create IMMDeviceEnumerator: {}", err.ErrorMessage());
//  spdlog::warn("Audio device changes will not happen automatically");
//} else {
//  win32_notif_client_ = new CMMNotificationClient();
//  win32_dev_enumerator_->RegisterEndpointNotificationCallback(win32_notif_client_);
//}
//#endif
}

AudioMgr::~AudioMgr() {
  for (const auto &[_, l] : sources_)
    for (const auto &source : l) {
      alDeleteSources(1, &source);
      check_al_errors();
    }
  sources_.clear();

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(ctx_);
}

void AudioMgr::update() {
  for (auto s_it = sources_.begin(); s_it != sources_.end(); ) {
    auto &l = s_it->second;

    for (auto it = l.begin(); it != l.end(); ) {
      ALint state;
      alGetSourcei(*it, AL_SOURCE_STATE, &state);
      check_al_errors();

      if (state == AL_STOPPED) {
        alDeleteSources(1, &(*it));
        check_al_errors();
        it = l.erase(it);
        continue;
      }
      it++;
    }

    if (s_it->second.empty()) {
      s_it = sources_.erase(s_it);
      continue;
    }
    s_it++;
  }
}

bool AudioMgr::open_device(const std::string &device_name) {
  const char *device_cp = nullptr;
  if (!device_name.empty())
    device_cp = device_name.c_str();

  device_ = alcOpenDevice(nullptr);
  if (!device_) {
    spdlog::error("Failed to open device: '{}'", device_cp ? device_name : "default");
    return false;
  } else
    spdlog::debug("Opened device: '{}'", device_cp ? device_name : "default");

  if (alcIsExtensionPresent(device_, "ALC_SOFTX_reopen_device")) {
    alcReopenDeviceSOFT_ = (ALCboolean (ALC_APIENTRY*)(ALCdevice *device, const ALCchar *name, const ALCint *attribs))alcGetProcAddress(device_, "alcReopenDeviceSOFT");
    reopen_supported_ = true;
  } else
    spdlog::warn("Reopen extension not supported on this system; change in audio device will require reloading all audio buffers");

  return true;
}

bool AudioMgr::reopen_device(const std::string &device_name) {
  if (reopen_supported_) {
    const char *device_cp = nullptr;
    if (!device_name.empty())
      device_cp = device_name.c_str();

    bool ok = alcReopenDeviceSOFT_(device_, device_cp, nullptr);
    check_alc_errors(device_);
    if (!ok) {
      spdlog::error("Failed to reopen device: '{}'", device_cp ? device_name : "default");
      return false;
    } else
      spdlog::debug("Reopened device: '{}'", device_cp ? device_name : "default");

  } else {
    spdlog::error("Failed to reopen device: '{}'; extension not supported on this system");
    return false;
  }

  return true;
}

bool AudioMgr::open_context() {
  ctx_ = alcCreateContext(device_, nullptr);
  bool ok = check_alc_errors(device_);
  if (!ok)
    spdlog::error("Failed to create OpenAL context");
  return ok;
}

bool AudioMgr::make_current() {
  bool ok1 = alcMakeContextCurrent(ctx_);
  bool ok2 = check_alc_errors(device_);
  if (!ok1 || !ok2)
    spdlog::error("Failed to make OpenAL context current");
  return ok1 && ok2;
}

bool AudioMgr::load(const std::string &name, const std::string &filename) {
  std::unique_ptr<nqr::AudioData> file_data = std::make_unique<nqr::AudioData>();
  audio_loader_.Load(file_data.get(), filename);

  ALuint buffer;
  alGenBuffers(1, &buffer);
  check_al_errors();

  ALenum format;
  if (file_data->channelCount == 1)
    format = AL_FORMAT_MONO_FLOAT32;
  else if (file_data->channelCount == 2)
    format = AL_FORMAT_STEREO_FLOAT32;
  else {
    spdlog::error("Unrecognized audio format: {} channels", file_data->channelCount);
    return false;
  }

  alBufferData(
      buffer, format,
      &file_data->samples[0],
      file_data->samples.size() * sizeof(float),
      file_data->sampleRate
  );
  bool ok = check_al_errors();
  if (!ok) {
    spdlog::error("Failed to load audio: '{}'", filename);
    return false;
  } else
    spdlog::debug("Loaded audio: '{}'", filename);
  buffers_[name] = buffer;

  return true;
}

void AudioMgr::play(const std::string &name, const PlayOptions &options) {
  ALuint source;
  alGenSources(1, &source);
  check_al_errors();
  sources_[name].emplace_back(source);

  alSourcef(sources_[name].back(), AL_PITCH, options.pitch);
  alSourcef(sources_[name].back(), AL_GAIN, options.volume);
  alSource3f(sources_[name].back(), AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(sources_[name].back(), AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcei(sources_[name].back(), AL_LOOPING, options.looping ? AL_TRUE : AL_FALSE);
  alSourcei(sources_[name].back(), AL_BUFFER, buffers_[name]);

  alSourcePlay(sources_[name].back());
  check_al_errors();
}

const std::vector<std::string> &AudioMgr::get_devices() {
  if (devices_.empty())
    get_available_devices_();
  return devices_;
}

void AudioMgr::get_available_devices_() {
  const ALCchar *devices = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
  if (!devices) {
    spdlog::error("Failed to get device list");
    return;
  }
  const char *ptr = devices;

  devices_.clear();
  do {
    devices_.emplace_back(ptr);
    ptr += devices_.back().size() + 1;
  } while (*ptr != '\0');
}

bool AudioMgr::check_al_errors() {
  ALenum error = alGetError();
  if (error != AL_NO_ERROR) {
    switch (error) {
      case AL_INVALID_NAME:
        spdlog::error("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
        break;
      case AL_INVALID_ENUM:
        spdlog::error("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
        break;
      case AL_INVALID_VALUE:
        spdlog::error("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
        break;
      case AL_INVALID_OPERATION:
        spdlog::error("AL_INVALID_OPERATION: the requested operation is not valid");
        break;
      case AL_OUT_OF_MEMORY:
        spdlog::error("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
        break;
      default:
        spdlog::error("UNKNOWN AL ERROR: {}", error);
    }
    return false;
  }
  return true;
}

bool AudioMgr::check_alc_errors(ALCdevice *device) {
  ALCenum error = alcGetError(device);
  if (error != ALC_NO_ERROR) {
    switch (error) {
      case ALC_INVALID_VALUE:
        spdlog::error("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
        break;
      case ALC_INVALID_DEVICE:
        spdlog::error("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
        break;
      case ALC_INVALID_CONTEXT:
        spdlog::error("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
        break;
      case ALC_INVALID_ENUM:
        spdlog::error("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
        break;
      case ALC_OUT_OF_MEMORY:
        spdlog::error("ALC_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
        break;
      default:
        spdlog::error("UNKNOWN ALC ERROR: {}", error);
    }
    return false;
  }
  return true;
}

bool AudioMgr::check_alc_errors() {
  return check_alc_errors(device_);
}

//#if defined(BAPHOMET_PLATFORM_WINDOWS)
//// Given an endpoint ID string, print the friendly device name.
//HRESULT CMMNotificationClient::PrintDeviceName(LPCWSTR pwstrId)
//{
//  HRESULT hr = S_OK;
//  IMMDevice *pDevice = NULL;
//  IPropertyStore *pProps = NULL;
//  PROPVARIANT varString;
//
//  CoInitialize(NULL);
//  PropVariantInit(&varString);
//
//  if (_pEnumerator == NULL)
//  {
//    // Get enumerator for audio endpoint devices.
//    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),
//                          NULL, CLSCTX_INPROC_SERVER,
//                          __uuidof(IMMDeviceEnumerator),
//                          (void**)&_pEnumerator);
//  }
//  if (hr == S_OK)
//  {
//    hr = _pEnumerator->GetDevice(pwstrId, &pDevice);
//  }
//  if (hr == S_OK)
//  {
//    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
//  }
//  if (hr == S_OK)
//  {
//    // Get the endpoint device's friendly-name property.
//    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varString);
//  }
//  printf("----------------------\nDevice name: \"%S\"\n"
//         "  Endpoint ID string: \"%S\"\n",
//         (hr == S_OK) ? varString.pwszVal : L"null device",
//         (pwstrId != NULL) ? pwstrId : L"null ID");
//
//  PropVariantClear(&varString);
//
//  SAFE_RELEASE(pProps)
//  SAFE_RELEASE(pDevice)
//  CoUninitialize();
//  return hr;
//}
//#endif

} // namespace baphomet
