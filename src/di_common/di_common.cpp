#include "di_common/di_common.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <iostream>
#include <vector>

using std::max;
using std::min;

namespace di_common {

int ClampStrengthPercentage(int strengthPercentage) {
  return max(0, min(100, strengthPercentage));
}

const GUID kEffectGuid = GUID_Sine;
const int kNumUpdates = 1000;
const char kIgnoreDeviceWithName[] = "vJoy Device";

static HWND g_hwnd = nullptr;

HWND GetHwnd() {
  if (!g_hwnd) {
    g_hwnd = CreateWindow("STATIC", "FFB_EXAMPLE", SS_BLACKRECT, 0, 0, 10, 10,
                          NULL, NULL, GetModuleHandle(0), NULL);
  }
  return g_hwnd;
}

const char* DInputErrorToString(HRESULT hr) {
  switch (hr) {
    case DI_OK:
      return "DI_OK: The operation completed successfully.";
    case S_FALSE:
      return "S_FLASE: Could be one of: DI_NOTATTACHED, DI_BUFFEROVERFLOW, "
             "DI_PROPNOEFFECT, DI_NOEFFECT.";
    case DI_POLLEDDEVICE:
      return "DI_POLLEDDEVICE: The device is a polled device. As a result, "
             "device buffering will not collect any data and event "
             "notifications will not be signalled until GetDeviceState is "
             "called.";
    case DI_DOWNLOADSKIPPED:
      return "DI_DOWNLOADSKIPPED: The parameters of the effect were "
             "successfully updated, but the effect was not downloaded because "
             "the device is not exclusively acquired or because the "
             "DIEP_NODOWNLOAD flag was passed.";
    case DI_EFFECTRESTARTED:
      return "DI_EFFECTRESTARTED: The parameters of the effect were "
             "successfully updated, but in order to change the parameters, "
             "the effect needed to be restarted.";
    case DI_TRUNCATED:
      return "DI_TRUNCATED: The parameters of the effect were successfully "
             "updated, but some of them were beyond the capabilities of the "
             "device and were truncated.";
    case DI_SETTINGSNOTSAVED:
      return "DI_SETTINGSNOTSAVED: The settings have been successfully applied "
             "but could not be persisted.";
    case DI_TRUNCATEDANDRESTARTED:
      return "DI_TRUNCATEDANDRESTARTED: Equal to DI_EFFECTRESTARTED | "
             "DI_TRUNCATED.";
    case DI_WRITEPROTECT:
      return "DI_WRITEPROTECT: A SUCCESS code indicating that settings cannot "
             "be modified.";
    case DIERR_OLDDIRECTINPUTVERSION:
      return "DIERR_OLDDIRECTINPUTVERSION: The application requires a newer "
             "version of DirectInput.";
    case DIERR_BETADIRECTINPUTVERSION:
      return "DIERR_BETADIRECTINPUTVERSION: The application was written for an "
             "unsupported prerelease version of DirectInput.";
    case DIERR_BADDRIVERVER:
      return "DIERR_BADDRIVERVER: The object could not be created due to an "
             "incompatible driver version or mismatched or incomplete driver "
             "components.";
    case DIERR_DEVICENOTREG:
      return "DIERR_DEVICENOTREG (REGDB_E_CLASSNOTREG): The device or device "
             "instance or effect is not registered with DirectInput.";
    case DIERR_NOTFOUND:
      return "DIERR_NOTFOUND/OBJECTNOTFOUND (ERROR_FILE_NOT_FOUND): The "
             "requested object does not exist.";
    case DIERR_INVALIDPARAM:
      return "DIERR_INVALIDPARAM (E_INVALIDARG): An invalid parameter was "
             "passed to the returning function, or the object was not in a "
             "state that admitted the function to be called.";
    case DIERR_NOINTERFACE:
      return "DIERR_NOINTERFACE (E_NOINTERFACE): The specified interface is "
             "not supported by the object.";
    case DIERR_GENERIC:
      return "DIERR_GENERIC (E_FAIL): An undetermined error occurred inside "
             "the DInput subsystem.";
    case DIERR_OUTOFMEMORY:
      return "DIERR_OUTOFMEMORY (E_OUTOFMEMORY): The DInput subsystem couldn't "
             "allocate sufficient memory to complete the caller's request.";
    case DIERR_UNSUPPORTED:
      return "DIERR_UNSUPPORTED (E_NOTIMPL): The function called is not "
             "supported at this time.";
    case DIERR_NOTINITIALIZED:
      return "DIERR_NOTINITIALIZED (ERROR_NOT_READY): This object has not been "
             "initialized.";
    case DIERR_ALREADYINITIALIZED:
      return "DIERR_ALREADYINITIALIZED (ERROR_ALREADY_INITIALIZED): This "
             "object is already initialized.";
    case DIERR_NOAGGREGATION:
      return "DIERR_NOAGGREGATION (CLASS_E_NOAGGREGATION): This object does "
             "not support aggregation.";
    case E_ACCESSDENIED:
      return "E_ACCESSDENIED: Access Denied. Could be DIERR_OTHERAPPHASPRIO "
             "(another app has priority), DIERR_READONLY (property cannot be "
             "changed), or DIERR_HANDLEEXISTS (event notification already "
             "exists).";
    case DIERR_INPUTLOST:
      return "DIERR_INPUTLOST (ERROR_READ_FAULT): Access to the device has "
             "been lost. It must be re-acquired.";
    case DIERR_ACQUIRED:
      return "DIERR_ACQUIRED (ERROR_BUSY): The operation cannot be performed "
             "while the device is acquired.";
    case DIERR_NOTACQUIRED:
      return "DIERR_NOTACQUIRED (ERROR_INVALID_ACCESS): The operation cannot "
             "be performed unless the device is acquired.";
    case E_PENDING:
      return "E_PENDING: Data is not yet available.";
    case DIERR_INSUFFICIENTPRIVS:
      return "DIERR_INSUFFICIENTPRIVS: Unable to acquire privileges needed, "
             "e.g., change joystick configuration.";
    case DIERR_DEVICEFULL:
      return "DIERR_DEVICEFULL: The device is full.";
    case DIERR_MOREDATA:
      return "DIERR_MOREDATA: Not all the requested information fit into the "
             "buffer.";
    case DIERR_NOTDOWNLOADED:
      return "DIERR_NOTDOWNLOADED: The effect is not downloaded.";
    case DIERR_HASEFFECTS:
      return "DIERR_HASEFFECTS: The device cannot be reinitialized because "
             "there are still effects attached to it.";
    case DIERR_NOTEXCLUSIVEACQUIRED:
      return "DIERR_NOTEXCLUSIVEACQUIRED: The operation cannot be performed "
             "unless the device is acquired in DISCL_EXCLUSIVE mode.";
    case DIERR_INCOMPLETEEFFECT:
      return "DIERR_INCOMPLETEEFFECT: The effect could not be downloaded "
             "because essential information is missing (e.g., no axes "
             "associated).";
    case DIERR_NOTBUFFERED:
      return "DIERR_NOTBUFFERED: Attempted to read buffered device data from a "
             "device that is not buffered.";
    case DIERR_EFFECTPLAYING:
      return "DIERR_EFFECTPLAYING: An attempt was made to modify parameters of "
             "an effect while it is playing. Not all hardware devices support "
             "this.";
    case DIERR_UNPLUGGED:
      return "DIERR_UNPLUGGED: The operation could not be completed because "
             "the device is not plugged in.";
    case DIERR_REPORTFULL:
      return "DIERR_REPORTFULL: SendDeviceData failed because more information "
             "was requested than can be sent (e.g., limit on simultaneous "
             "buttons).";
    case DIERR_MAPFILEFAIL:
      return "DIERR_MAPFILEFAIL: A mapper file function failed because reading "
             "or writing the user or IHV settings file failed.";
    case E_HANDLE:
      return "E_HANDLE: Invalid handle.";
    case E_POINTER:
      return "E_POINTER: Invalid pointer.";
    default:
      return "Unknown DirectInput HRESULT";
  }
}

bool CheckDInputResult(HRESULT hr, const char* functionName) {
  if (FAILED(hr)) {
    std::cerr << functionName << " failed: " << DInputErrorToString(hr)
              << " (0x" << std::hex << hr << ")" << std::endl;
    return false;
  } else if (hr != S_OK) {
    std::cout << functionName
              << " completed with status: " << DInputErrorToString(hr) << " (0x"
              << std::hex << hr << ")" << std::endl;
    return true;
  }
  return true;
}

std::string NormalizeEffectName(const std::string& effectName) {
  std::string normalized = effectName;
  std::transform(
      normalized.begin(), normalized.end(), normalized.begin(),
      [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

  normalized.erase(std::remove(normalized.begin(), normalized.end(), '-'),
                   normalized.end());
  normalized.erase(std::remove(normalized.begin(), normalized.end(), '_'),
                   normalized.end());
  return normalized;
}

bool ResolveEffectGuid(const std::string& effectName, GUID& effectGuid) {
  std::string normalized = NormalizeEffectName(effectName);

  if (normalized == "sine") {
    effectGuid = GUID_Sine;
    return true;
  }
  if (normalized == "square") {
    effectGuid = GUID_Square;
    return true;
  }
  if (normalized == "sawtoothdown") {
    effectGuid = GUID_SawtoothDown;
    return true;
  }
  if (normalized == "sawtoothup") {
    effectGuid = GUID_SawtoothUp;
    return true;
  }
  if (normalized == "triangle") {
    effectGuid = GUID_Triangle;
    return true;
  }
  if (normalized == "constant") {
    effectGuid = GUID_ConstantForce;
    return true;
  }
  if (normalized == "ramp") {
    effectGuid = GUID_RampForce;
    return true;
  }
  if (normalized == "spring") {
    effectGuid = GUID_Spring;
    return true;
  }
  if (normalized == "damper") {
    effectGuid = GUID_Damper;
    return true;
  }
  if (normalized == "friction") {
    effectGuid = GUID_Friction;
    return true;
  }
  if (normalized == "inertia") {
    effectGuid = GUID_Inertia;
    return true;
  }

  return false;
}

bool BuildEffectParameters(const std::string& effectName, DIEFFECT& effect,
                           void*& typeSpecificParams,
                           DWORD& typeSpecificParamSize,
                           std::uint64_t iterationIndex,
                           int strengthPercentage) {
  std::string normalized = NormalizeEffectName(effectName);

  static DIPERIODIC periodic;
  static DICONSTANTFORCE constantForce;
  static DIRAMPFORCE rampForce;
  static DICONDITION condition;
  static DWORD axes[1] = {DIJOFS_X};
  static LONG direction[1] = {1};

  ZeroMemory(&effect, sizeof(effect));
  effect.dwSize = sizeof(DIEFFECT);
  effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
  effect.dwDuration = INFINITE;
  effect.dwSamplePeriod = 0;
  const int clampedStrength = ClampStrengthPercentage(strengthPercentage);
  const double strengthScale = clampedStrength / 100.0;
  effect.dwGain = static_cast<DWORD>(DI_FFNOMINALMAX * strengthScale);
  effect.dwTriggerButton = DIEB_NOTRIGGER;
  effect.dwTriggerRepeatInterval = 0;
  effect.cAxes = 1;
  effect.rgdwAxes = axes;
  effect.rglDirection = direction;

  if (normalized == "sine" || normalized == "square" ||
      normalized == "sawtoothdown" || normalized == "sawtoothup" ||
      normalized == "triangle") {
    ZeroMemory(&periodic, sizeof(periodic));
    periodic.dwMagnitude = static_cast<DWORD>(DI_FFNOMINALMAX * strengthScale);
    periodic.lOffset = 0;
    periodic.dwPhase = 0;
    periodic.dwPeriod = 500 * 1000;
    typeSpecificParams = &periodic;
    typeSpecificParamSize = sizeof(periodic);
  } else if (normalized == "constant") {
    ZeroMemory(&constantForce, sizeof(constantForce));
    constantForce.lMagnitude =
        static_cast<LONG>(DI_FFNOMINALMAX * strengthScale);
    typeSpecificParams = &constantForce;
    typeSpecificParamSize = sizeof(constantForce);
  } else if (normalized == "ramp") {
    ZeroMemory(&rampForce, sizeof(rampForce));
    rampForce.lStart = static_cast<LONG>(-DI_FFNOMINALMAX * strengthScale);
    rampForce.lEnd = static_cast<LONG>(DI_FFNOMINALMAX * strengthScale);
    typeSpecificParams = &rampForce;
    typeSpecificParamSize = sizeof(rampForce);
  } else if (normalized == "spring" || normalized == "damper" ||
             normalized == "friction" || normalized == "inertia") {
    ZeroMemory(&condition, sizeof(condition));
    condition.lOffset = 0;
    condition.lPositiveCoefficient =
        static_cast<LONG>(DI_FFNOMINALMAX * strengthScale);
    condition.lNegativeCoefficient =
        static_cast<LONG>(-DI_FFNOMINALMAX * strengthScale);
    condition.dwPositiveSaturation = DI_FFNOMINALMAX;
    condition.dwNegativeSaturation = DI_FFNOMINALMAX;
    condition.lDeadBand = 0;
    typeSpecificParams = &condition;
    typeSpecificParamSize = sizeof(condition);
  } else {
    return false;
  }

  if (normalized == "sine" || normalized == "square" ||
      normalized == "sawtoothdown" || normalized == "sawtoothup" ||
      normalized == "triangle") {
    periodic.dwMagnitude = static_cast<DWORD>((iterationIndex * 100) % 10000);
  } else if (normalized == "constant") {
    constantForce.lMagnitude = static_cast<LONG>((iterationIndex * 20) % 10000);
  } else if (normalized == "ramp") {
    rampForce.lStart = static_cast<LONG>((iterationIndex * 50) % 10000);
    rampForce.lEnd = -DI_FFNOMINALMAX;
  } else if (normalized == "spring" || normalized == "damper" ||
             normalized == "friction" || normalized == "inertia") {
    condition.lPositiveCoefficient =
        static_cast<LONG>((iterationIndex * 100) % 10000);
    condition.lNegativeCoefficient = condition.lPositiveCoefficient;
  }

  effect.cbTypeSpecificParams = typeSpecificParamSize;
  effect.lpvTypeSpecificParams = typeSpecificParams;
  return true;
}

bool IsForceFeedbackSupported(IDirectInputDevice8* device) {
  DIEFFECTINFO effectInfo;
  effectInfo.dwSize = sizeof(DIEFFECTINFO);
  HRESULT hr = device->GetEffectInfo(&effectInfo, kEffectGuid);
  return CheckDInputResult(hr, "GetEffectInfo");
}

BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance,
                                  VOID* pContext) {
  std::cout << "Found device: " << instance->tszInstanceName << std::endl;
  if (strstr(instance->tszInstanceName, kIgnoreDeviceWithName)) {
    std::cout
        << "  -> Ignoring device because in ignore list [kIgnoreDeviceWithName]"
        << std::endl;
    return DIENUM_CONTINUE;
  }
  auto devices = reinterpret_cast<std::vector<IDirectInputDevice8*>*>(pContext);
  IDirectInput8* directInput = nullptr;
  HWND hwnd = GetHwnd();
  if (!hwnd) {
    std::cerr << "  -> ERROR: Could not get console window handle!"
              << std::endl;
    if (directInput) {
      directInput->Release();
    }
    return false;
  }
  HRESULT hr =
      DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                         IID_IDirectInput8, (void**)&directInput, nullptr);
  if (!CheckDInputResult(hr, "DirectInput8Create")) {
    return DIENUM_STOP;
  } else if (!directInput) {
    std::cerr << "directInput unexpectedly null" << std::endl;
    return DIENUM_STOP;
  }

  IDirectInputDevice8* device = nullptr;
  hr = directInput->CreateDevice(instance->guidInstance, &device, nullptr);
  if (CheckDInputResult(hr, "CreateDevice")) {
    if (IsForceFeedbackSupported(device)) {
      if (!CheckDInputResult(device->SetDataFormat(&c_dfDIJoystick2),
                             "SetDataFormat") ||
          !CheckDInputResult(device->SetCooperativeLevel(
                                 hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE),
                             "SetCooperativeLevel") ||
          !CheckDInputResult(device->Acquire(), "Acquire")) {
        device->Release();
        return false;
      }
      devices->push_back(device);
    } else {
      device->Release();
    }
  }
  directInput->Release();
  return DIENUM_CONTINUE;
}

}  // namespace di_common
