#pragma once

#include "baphomet/app/internal/messenger.hpp"
#include "baphomet/util/platform.hpp"

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/alext.h"

#include "libnyquist/Decoders.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>

//#if defined(BAPHOMET_PLATFORM_WINDOWS)
//#define WIN32_LEAN_AND_MEAN
//#define NOMINMAX
//#include <Windows.h>
//#include <mmdeviceapi.h>
//#include <functiondiscoverykeys.h>
//
//#pragma comment(lib, "uuid.lib")
//#endif

namespace baphomet {

//#if defined(BAPHOMET_PLATFORM_WINDOWS)
////-----------------------------------------------------------
//// Example implementation of IMMNotificationClient interface.
//// When the status of audio endpoint devices change, the
//// MMDevice module calls these methods to notify the client.
////-----------------------------------------------------------
//
//#define SAFE_RELEASE(punk)  \
//              if ((punk) != NULL)  \
//                { (punk)->Release(); (punk) = NULL; }
//
//class CMMNotificationClient : public IMMNotificationClient {
//  LONG _cRef;
//  IMMDeviceEnumerator *_pEnumerator;
//
//  // Private function to print device-friendly name
//  HRESULT PrintDeviceName(LPCWSTR  pwstrId);
//
//public:
//  CMMNotificationClient() : _cRef(1), _pEnumerator(NULL) {}
//
//  ~CMMNotificationClient() {
//    SAFE_RELEASE(_pEnumerator)
//  }
//
//  // IUnknown methods -- AddRef, Release, and QueryInterface
//
//  ULONG STDMETHODCALLTYPE AddRef() {
//    return InterlockedIncrement(&_cRef);
//  }
//
//  ULONG STDMETHODCALLTYPE Release() {
//    ULONG ulRef = InterlockedDecrement(&_cRef);
//    if (0 == ulRef)
//      delete this;
//    return ulRef;
//  }
//
//  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID **ppvInterface) {
//    if (IID_IUnknown == riid) {
//      AddRef();
//      *ppvInterface = (IUnknown*)this;
//    } else if (__uuidof(IMMNotificationClient) == riid) {
//      AddRef();
//      *ppvInterface = (IMMNotificationClient*)this;
//    } else {
//      *ppvInterface = NULL;
//      return E_NOINTERFACE;
//    }
//    return S_OK;
//  }
//
//  // Callback methods for device-event notifications.
//
//  HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId) {
//    const char  *pszFlow = "?????";
//    const char  *pszRole = "?????";
//
//    PrintDeviceName(pwstrDeviceId);
//
//    switch (flow) {
//      case eRender:
//        pszFlow = "eRender";
//        break;
//      case eCapture:
//        pszFlow = "eCapture";
//        break;
//    }
//
//    switch (role) {
//      case eConsole:
//        pszRole = "eConsole";
//        break;
//      case eMultimedia:
//        pszRole = "eMultimedia";
//        break;
//      case eCommunications:
//        pszRole = "eCommunications";
//        break;
//    }
//
//    printf("  -->New default device: flow = %s, role = %s\n",
//           pszFlow, pszRole);
//    return S_OK;
//  }
//
//  HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId) {
//    PrintDeviceName(pwstrDeviceId);
//
//    printf("  -->Added device\n");
//    return S_OK;
//  };
//
//  HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
//    PrintDeviceName(pwstrDeviceId);
//
//    printf("  -->Removed device\n");
//    return S_OK;
//  }
//
//  HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) {
//    const char  *pszState = "?????";
//
//    PrintDeviceName(pwstrDeviceId);
//
//    switch (dwNewState) {
//      case DEVICE_STATE_ACTIVE:
//        pszState = "ACTIVE";
//        break;
//      case DEVICE_STATE_DISABLED:
//        pszState = "DISABLED";
//        break;
//      case DEVICE_STATE_NOTPRESENT:
//        pszState = "NOTPRESENT";
//        break;
//      case DEVICE_STATE_UNPLUGGED:
//        pszState = "UNPLUGGED";
//        break;
//    }
//
//    printf("  -->New device state is DEVICE_STATE_%s (0x%8.8x)\n",
//           pszState, dwNewState);
//
//    return S_OK;
//  }
//
//  HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
//    PrintDeviceName(pwstrDeviceId);
//
//    printf("  -->Changed device property "
//           "{%8.8x-%4.4x-%4.4x-%2.2x%2.2x-%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x}#%d\n",
//           key.fmtid.Data1, key.fmtid.Data2, key.fmtid.Data3,
//           key.fmtid.Data4[0], key.fmtid.Data4[1],
//           key.fmtid.Data4[2], key.fmtid.Data4[3],
//           key.fmtid.Data4[4], key.fmtid.Data4[5],
//           key.fmtid.Data4[6], key.fmtid.Data4[7],
//           key.pid);
//    return S_OK;
//  }
//};
//#endif

struct PlayOptions {
  float pitch{1.0f};    // valid range: 0.5 - 2.0
  float volume{1.0f};   // valid range: 0.0 - 1.0
  bool looping{false};
};

class AudioMgr {
public:
  AudioMgr(std::shared_ptr<Messenger> msgr);

  ~AudioMgr();

  bool open_device(const std::string &device_name = "");
  bool reopen_device(const std::string &device_name = "");

  bool open_context();

  bool make_current();

  bool load(const std::string &name, const std::string &filename);

  void play(const std::string &name, const PlayOptions &options = {});

  const std::vector<std::string> &get_devices();

private:
  std::shared_ptr<Messenger> msgr_;

  ALCcontext *ctx_{nullptr};

  ALCdevice *device_{nullptr};
  std::vector<std::string> devices_{};

  nqr::NyquistIO audio_loader_{};
  std::unordered_map<std::string, ALuint> buffers_{};
  std::unordered_map<std::string, std::list<ALuint>> sources_{};

  ALCboolean (ALC_APIENTRY *alcReopenDeviceSOFT_)(ALCdevice *device, const ALCchar *name, const ALCint *attribs);

  bool reopen_supported_{false};

  void update_(Duration dt);

  void received_message_(const MsgCat &category, const std::any &payload);

  void get_available_devices_();

  bool check_al_errors();
  bool check_alc_errors(ALCdevice *device);
  bool check_alc_errors();

//#if defined(BAPHOMET_PLATFORM_WINDOWS)
//  IMMDeviceEnumerator *win32_dev_enumerator_{nullptr};
//  CMMNotificationClient *win32_notif_client_{nullptr};
//#endif
};

} // namespace baphomet