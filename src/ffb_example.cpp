#include "di_common/di_common.h"

#include <cxxopts.hpp>

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
  cxxopts::Options options("ffb_example",
                           "Play a DirectInput force feedback effect");
  options.add_options()("e,effect",
                        "Effect to play: sine, square, sawtooth-down, "
                        "sawtooth-up, triangle, constant, ramp, spring, "
                        "damper, friction, inertia",
                        cxxopts::value<std::string>()->default_value("sine"))(
      "h,help", "Print help");

  try {
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      return 0;
    }

    std::string effectName = result["effect"].as<std::string>();
    GUID effectGuid = di_common::kEffectGuid;
    if (!di_common::ResolveEffectGuid(effectName, effectGuid)) {
      std::cerr << "Unknown effect '" << effectName
                << "'. Expected one of: sine, square, sawtooth-down, "
                   "sawtooth-up, triangle, constant, ramp, spring, damper, "
                   "friction, inertia"
                << std::endl;
      return 2;
    }

    std::cout << "Using effect: " << effectName << std::endl;

    std::string normalizedEffectName =
        di_common::NormalizeEffectName(effectName);

    IDirectInput8* directInput = nullptr;
    std::vector<IDirectInputDevice8*> forceFeedbackDevices;

    HRESULT hr =
        DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                           IID_IDirectInput8, (void**)&directInput, nullptr);
    if (!di_common::CheckDInputResult(hr, "DirectInput8Create")) {
      return 1;
    } else if (!directInput) {
      std::cerr << "directInput unexpectedly null" << std::endl;
      return 1;
    }

    hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL,
                                  di_common::EnumDevicesCallback,
                                  &forceFeedbackDevices,
                                  DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
    if (!di_common::CheckDInputResult(hr, "EnumDevices")) {
      directInput->Release();
      return 1;
    }

    std::cout << "Found " << forceFeedbackDevices.size()
              << " force feedback devices." << std::endl;
    for (IDirectInputDevice8* device : forceFeedbackDevices) {
    DIEFFECT effect;
    void* typeSpecificParams = nullptr;
    DWORD typeSpecificParamSize = 0;
    if (!di_common::BuildEffectParameters(effectName, effect, typeSpecificParams,
                                         typeSpecificParamSize, 0)) {
      std::cerr << "Unsupported effect type: " << effectName << std::endl;
      return 2;
    }

    IDirectInputEffect* effectInterface = nullptr;
    hr = device->CreateEffect(effectGuid, &effect, &effectInterface, nullptr);
    if (!di_common::CheckDInputResult(hr, "CreateEffect")) {
      return 1;
    } else {
      hr = effectInterface->Start(1, 0);
      if (!di_common::CheckDInputResult(hr, "Start")) {
        return 1;
      }
      const auto start_time = std::chrono::high_resolution_clock::now();
      for (int i = 0; i < di_common::kNumUpdates; ++i) {
        if (!di_common::BuildEffectParameters(effectName, effect,
                                             typeSpecificParams,
                                             typeSpecificParamSize, i)) {
          std::cerr << "Unsupported effect type: " << effectName << std::endl;
          return 2;
        }
        hr = effectInterface->SetParameters(&effect, DIEP_TYPESPECIFICPARAMS);
        if (!di_common::CheckDInputResult(hr, "SetParameters")) {
          return 1;
        }
      }
      const auto kEndTime = std::chrono::high_resolution_clock::now();
      const auto kDuration =
          std::chrono::duration_cast<std::chrono::milliseconds>(kEndTime -
                                                                start_time);
      const float kAverage =
          static_cast<float>(kDuration.count()) / di_common::kNumUpdates;
      std::cout << "Total run time for " << std::dec << di_common::kNumUpdates
                << " updates: " << kDuration.count()
                << " ms, average: " << kAverage << " ms" << std::endl;
      effectInterface->Stop();
      effectInterface->Release();
    }
      device->Release();
    }
    directInput->Release();
    return 0;
  } catch (const cxxopts::exceptions::exception& ex) {
    std::cerr << ex.what() << std::endl;
    std::cerr << options.help() << std::endl;
    return 2;
  }
}
