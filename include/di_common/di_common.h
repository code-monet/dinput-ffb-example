#pragma once

#define DIRECTINPUT_VERSION 0x0800
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <dinput.h>
#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace di_common {

extern const GUID kSineEffectGuid;
extern const int kNumUpdates;
extern const char kIgnoreDeviceWithName[];

HWND GetHwnd();
const char* DInputErrorToString(HRESULT hr);
bool CheckDInputResult(HRESULT hr, const char* functionName);
std::string NormalizeEffectName(const std::string& effectName);
int ClampStrengthPercentage(int strengthPercentage);
bool ReadXAxisValue(IDirectInputDevice8* device, LONG& rawXAxisValue);
bool IsForceFeedbackSupported(IDirectInputDevice8* device);
bool ResolveEffectGuid(const std::string& effectName, GUID& effectGuid);
bool BuildEffectParameters(const std::string& effectName, DIEFFECT& effect,
                           void*& typeSpecificParams, DWORD& typeSpecificParamSize,
                           std::uint64_t iterationIndex,
                           int strengthPercentage);
struct EnumDevicesContext {
  IDirectInput8* directInput = nullptr;
  std::vector<IDirectInputDevice8*>* devices = nullptr;
};

BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance, VOID* pContext);

}  // namespace di_common
