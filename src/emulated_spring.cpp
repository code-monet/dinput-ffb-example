#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "di_common/di_common.h"

namespace {
volatile std::sig_atomic_t g_shouldStop = 0;

void HandleSignal(int) { g_shouldStop = 1; }
}  // namespace

int main() {
  std::signal(SIGINT, HandleSignal);

  IDirectInput8* directInput = nullptr;
  std::vector<IDirectInputDevice8*> forceFeedbackDevices;

  HRESULT hr =
      DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                         IID_IDirectInput8, (void**)&directInput, nullptr);
  if (!di_common::CheckDInputResult(hr, "DirectInput8Create")) {
    return 1;
  }
  if (!directInput) {
    std::cerr << "directInput unexpectedly null" << std::endl;
    return 1;
  }

  hr = directInput->EnumDevices(
      DI8DEVCLASS_GAMECTRL, di_common::EnumDevicesCallback,
      &forceFeedbackDevices, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
  if (!di_common::CheckDInputResult(hr, "EnumDevices")) {
    directInput->Release();
    return 1;
  }

  std::cout << "Found " << forceFeedbackDevices.size()
            << " force feedback devices." << std::endl;

  for (IDirectInputDevice8* device : forceFeedbackDevices) {
    std::cout << "Using device for emulated spring output." << std::endl;

    DIEFFECT effect;
    void* typeSpecificParams = nullptr;
    DWORD typeSpecificParamSize = 0;

    if (!di_common::BuildEffectParameters("constant", effect,
                                          typeSpecificParams,
                                          typeSpecificParamSize, 0, 100)) {
      std::cerr << "Failed to build constant-force effect parameters."
                << std::endl;
      device->Release();
      directInput->Release();
      return 1;
    }

    GUID effectGuid = di_common::kEffectGuid;
    if (!di_common::ResolveEffectGuid("constant", effectGuid)) {
      std::cerr << "Failed to resolve constant-force effect GUID." << std::endl;
      device->Release();
      directInput->Release();
      return 1;
    }

    IDirectInputEffect* effectInterface = nullptr;
    hr = device->CreateEffect(effectGuid, &effect, &effectInterface, nullptr);
    if (!di_common::CheckDInputResult(hr, "CreateEffect")) {
      device->Release();
      directInput->Release();
      return 1;
    }

    if (!di_common::CheckDInputResult(effectInterface->Start(1, 0), "Start")) {
      effectInterface->Release();
      device->Release();
      directInput->Release();
      return 1;
    }

    while (!g_shouldStop) {
      LONG rawXAxisValue = 0;
      if (!di_common::ReadXAxisValue(device, rawXAxisValue)) {
        break;
      }

      const double normalizedXAxis = rawXAxisValue / 32767.0 - 1.0;
      const double scaledXAxis = normalizedXAxis * 10000.0;
      const double clampedXAxis =
          std::max(-10000.0, std::min(10000.0, scaledXAxis));
      const LONG scaledForceValue = static_cast<LONG>(clampedXAxis);

      std::cout << std::dec << "Raw X axis value: " << rawXAxisValue
                << ", scaled force value: " << scaledForceValue << std::endl;

      effect.dwGain = DI_FFNOMINALMAX;
      DICONSTANTFORCE constantForce;
      ZeroMemory(&constantForce, sizeof(constantForce));
      constantForce.lMagnitude = scaledForceValue;

      effect.cbTypeSpecificParams = sizeof(constantForce);
      effect.lpvTypeSpecificParams = &constantForce;

      hr = effectInterface->SetParameters(&effect,
                                          DIEP_GAIN | DIEP_TYPESPECIFICPARAMS);
      if (!di_common::CheckDInputResult(hr, "SetParameters")) {
        break;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    effectInterface->Stop();
    effectInterface->Release();
    device->Release();
    break;
  }

  directInput->Release();
  return 0;
}
