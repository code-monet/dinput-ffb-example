#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>

#include <vector>

namespace di_common {

extern const GUID kEffectGuid;
extern const int kNumUpdates;
extern const char kIgnoreDeviceWithName[];

HWND GetHwnd();
const char* DInputErrorToString(HRESULT hr);
bool CheckDInputResult(HRESULT hr, const char* functionName);
bool IsForceFeedbackSupported(IDirectInputDevice8* device);
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance, VOID* pContext);

}  // namespace di_common
