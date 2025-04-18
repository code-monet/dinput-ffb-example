#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <windows.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

// Parameters for this example.

// Uncomment exactly one of the following periodic effects to play it.
const auto kEffectGuid = GUID_Sine;
// const auto kEffectGuid = GUID_Square;
// const auto kEffectGuid = GUID_SawtoothDown;
// const auto kEffectGuid = GUID_SawtoothUp;
// const auto kEffectGuid = GUID_Triangle;

const int kNumUpdates = 1000;  // The number of times to update the effect.
const char kIgnoreDeviceWithName[] = "Device Name to Ignore";

HWND g_hwnd = nullptr;

HWND GetHwnd() {
  if (!g_hwnd) {
    g_hwnd = CreateWindow("STATIC", "FFB_EXAMPLE", SS_BLACKRECT, 0, 0, 10, 10,
                          NULL, NULL, GetModuleHandle(0), NULL);
  }
  return g_hwnd;
}

/**
 * @brief Converts a DirectInput HRESULT code into a human-readable string.
 *
 * @param hr The HRESULT code returned by a DirectInput function.
 * @return A const char* pointer to a string literal describing the error or
 * status. Returns "Unknown DirectInput HRESULT" if the code is not recognized.
 */
const char* DInputErrorToString(HRESULT hr) {
  switch (hr) {
    // --- Success Codes (S_...) ---
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
             "successfully updated, but in order to change the parameters, the "
             "effect needed to be restarted.";
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

    // --- Error Codes (E_... or DIERR_...) ---
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
    case DIERR_DEVICENOTREG:  // Also REGDB_E_CLASSNOTREG
      return "DIERR_DEVICENOTREG (REGDB_E_CLASSNOTREG): The device or device "
             "instance or effect is not registered with DirectInput.";
    case DIERR_NOTFOUND:  // Also DIERR_OBJECTNOTFOUND, ERROR_FILE_NOT_FOUND
      return "DIERR_NOTFOUND/OBJECTNOTFOUND (ERROR_FILE_NOT_FOUND): The "
             "requested object does not exist.";
    // case DIERR_OBJECTNOTFOUND: // Same value as DIERR_NOTFOUND
    case DIERR_INVALIDPARAM:  // Also E_INVALIDARG
      return "DIERR_INVALIDPARAM (E_INVALIDARG): An invalid parameter was "
             "passed to the returning function, or the object was not in a "
             "state that admitted the function to be called.";
    case DIERR_NOINTERFACE:  // Also E_NOINTERFACE
      return "DIERR_NOINTERFACE (E_NOINTERFACE): The specified interface is "
             "not supported by the object.";
    case DIERR_GENERIC:  // Also E_FAIL
      return "DIERR_GENERIC (E_FAIL): An undetermined error occurred inside "
             "the DInput subsystem.";
    case DIERR_OUTOFMEMORY:  // Also E_OUTOFMEMORY
      return "DIERR_OUTOFMEMORY (E_OUTOFMEMORY): The DInput subsystem couldn't "
             "allocate sufficient memory to complete the caller's request.";
    case DIERR_UNSUPPORTED:  // Also E_NOTIMPL
      return "DIERR_UNSUPPORTED (E_NOTIMPL): The function called is not "
             "supported at this time.";
    case DIERR_NOTINITIALIZED:  // Also ERROR_NOT_READY
      return "DIERR_NOTINITIALIZED (ERROR_NOT_READY): This object has not been "
             "initialized.";
    case DIERR_ALREADYINITIALIZED:  // Also ERROR_ALREADY_INITIALIZED
      return "DIERR_ALREADYINITIALIZED (ERROR_ALREADY_INITIALIZED): This "
             "object is already initialized.";
    case DIERR_NOAGGREGATION:  // Also CLASS_E_NOAGGREGATION
      return "DIERR_NOAGGREGATION (CLASS_E_NOAGGREGATION): This object does "
             "not support aggregation.";
    // Note: E_ACCESSDENIED maps to multiple DIERR codes.
    case E_ACCESSDENIED:  // DIERR_OTHERAPPHASPRIO, DIERR_READONLY,
                          // DIERR_HANDLEEXISTS
      return "E_ACCESSDENIED: Access Denied. Could be DIERR_OTHERAPPHASPRIO "
             "(another app has priority), DIERR_READONLY (property cannot be "
             "changed), or DIERR_HANDLEEXISTS (event notification already "
             "exists).";
    case DIERR_INPUTLOST:  // Also ERROR_READ_FAULT
      return "DIERR_INPUTLOST (ERROR_READ_FAULT): Access to the device has "
             "been lost. It must be re-acquired.";
    case DIERR_ACQUIRED:  // Also ERROR_BUSY
      return "DIERR_ACQUIRED (ERROR_BUSY): The operation cannot be performed "
             "while the device is acquired.";
    case DIERR_NOTACQUIRED:  // Also ERROR_INVALID_ACCESS
      return "DIERR_NOTACQUIRED (ERROR_INVALID_ACCESS): The operation cannot "
             "be performed unless the device is acquired.";
    // case DIERR_READONLY: // Same value as E_ACCESSDENIED
    // case DIERR_HANDLEEXISTS: // Same value as E_ACCESSDENIED
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

    // --- Other Common HRESULTs ---
    case E_HANDLE:
      return "E_HANDLE: Invalid handle.";
    case E_POINTER:
      return "E_POINTER: Invalid pointer.";
      // Add more generic HRESULTs if needed

    default:
      return "Unknown DirectInput HRESULT";
  }
}

bool CheckDInputResult(HRESULT hr, const char* functionName) {
  if (FAILED(hr)) {
    std::cerr << functionName << " failed: " << DInputErrorToString(hr)
              << " (0x" << std::hex << hr << ")" << std::endl;
    return false;
  } else if (hr != S_OK) {  // Handle S_FALSE codes which indicate success but
                            // with a condition
    std::cout << functionName
              << " completed with status: " << DInputErrorToString(hr) << " (0x"
              << std::hex << hr << ")" << std::endl;
    return true;
  }
  return true;
  // else {
  //     std::cout << functionName << " succeeded." << std::endl;
  // }
}

// Function to check if a device supports the desired effect.
bool IsForceFeedbackSupported(IDirectInputDevice8* device) {
  DIEFFECTINFO effectInfo;
  effectInfo.dwSize = sizeof(DIEFFECTINFO);
  HRESULT hr = device->GetEffectInfo(&effectInfo, kEffectGuid);
  return CheckDInputResult(hr, "GetEffectInfo");
}

// Prepare each device for this example.
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance,
                                  VOID* pContext) {
  std::cout << "Found device: " << instance->tszInstanceName << std::endl;
  if (strstr(instance->tszInstanceName, kIgnoreDeviceWithName)) {
    return DIENUM_CONTINUE;
  }
  auto devices = reinterpret_cast<std::vector<IDirectInputDevice8*>*>(pContext);
  IDirectInput8* directInput = nullptr;
  HWND hwnd = GetHwnd();
  if (!hwnd) {
    std::cerr << "  -> ERROR: Could not get console window handle!"
              << std::endl;
    directInput->Release();
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

int main() {
  IDirectInput8* directInput = nullptr;
  std::vector<IDirectInputDevice8*> forceFeedbackDevices;

  HRESULT hr =
      DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                         IID_IDirectInput8, (void**)&directInput, nullptr);
  if (!CheckDInputResult(hr, "DirectInput8Create")) {
    return 1;
  } else if (!directInput) {
    std::cerr << "directInput unexpectedly null" << std::endl;
    return 1;
  }

  hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback,
                                &forceFeedbackDevices, DIEDFL_ATTACHEDONLY);
  if (!CheckDInputResult(hr, "EnumDevices")) {
    directInput->Release();
    return 1;
  }

  std::cout << "Found " << forceFeedbackDevices.size()
            << " force feedback devices." << std::endl;
  for (IDirectInputDevice8* device : forceFeedbackDevices) {
    DIEFFECT effect;
    ZeroMemory(&effect, sizeof(effect));
    effect.dwSize = sizeof(DIEFFECT);
    effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    effect.dwDuration = INFINITE;
    effect.dwSamplePeriod = 0;
    effect.dwGain = DI_FFNOMINALMAX / 4;
    effect.dwTriggerButton = DIEB_NOTRIGGER;
    effect.dwTriggerRepeatInterval = 0;
    effect.cAxes = 1;
    DWORD axes[1] = {DIJOFS_X};
    effect.rgdwAxes = axes;

    LONG direction[1] = {1};
    effect.rglDirection = direction;

    // Create the type specific periodic parameters.
    DIPERIODIC periodic;
    ZeroMemory(&periodic, sizeof(periodic));
    periodic.dwMagnitude = DI_FFNOMINALMAX;
    periodic.lOffset = 0;
    periodic.dwPhase = 0;
    periodic.dwPeriod = 500 * 1000;  // 500 ms.

    effect.cbTypeSpecificParams = sizeof(DIPERIODIC);
    effect.lpvTypeSpecificParams = &periodic;

    IDirectInputEffect* effectInterface = nullptr;
    hr = device->CreateEffect(kEffectGuid, &effect, &effectInterface, nullptr);
    if (!CheckDInputResult(hr, "CreateEffect")) {
      return 1;
    } else {
      hr = effectInterface->Start(1, 0);
      if (!CheckDInputResult(hr, "Start")) {
        return 1;
      }
      const auto start_time = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < kNumUpdates; ++i) {
        periodic.dwMagnitude = (i * 10) % 10000;
        hr = effectInterface->SetParameters(&effect, DIEP_TYPESPECIFICPARAMS);
        if (!CheckDInputResult(hr, "SetParameters")) {
          return 1;
        }
      }
      const auto kEndTime = std::chrono::high_resolution_clock::now();
      const auto kDuration =
          std::chrono::duration_cast<std::chrono::milliseconds>(kEndTime -
                                                                start_time);
      const float kAverage =
          static_cast<float>(kDuration.count()) / kNumUpdates;
      std::cout << "Total run time for " << std::dec << kNumUpdates
                << " updates: " << kDuration.count()
                << " ms, average: " << kAverage << std::endl;
      effectInterface->Stop();
      effectInterface->Release();
    }
    device->Release();
  }
  directInput->Release();
  return 0;
}
