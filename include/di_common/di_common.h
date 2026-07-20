#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>

#include <cstdint>
#include <string>
#include <vector>

namespace di_common {

extern const GUID kEffectGuid;
extern const int kNumUpdates;
extern const char kIgnoreDeviceWithName[];

HWND GetHwnd();
const char* DInputErrorToString(HRESULT hr);
bool CheckDInputResult(HRESULT hr, const char* functionName);
std::string NormalizeEffectName(const std::string& effectName);
bool IsForceFeedbackSupported(IDirectInputDevice8* device);
bool ResolveEffectGuid(const std::string& effectName, GUID& effectGuid);
bool BuildEffectParameters(const std::string& effectName, DIEFFECT& effect,
                           void*& typeSpecificParams, DWORD& typeSpecificParamSize,
                           std::uint64_t iterationIndex);
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance, VOID* pContext);

}  // namespace di_common
