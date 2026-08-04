#include <chrono>
#include <csignal>
#include <cstdint>
#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <thread>

#include "di_common/di_common.h"

namespace {
volatile std::sig_atomic_t g_shouldStop = 0;

void HandleSignal(int) { g_shouldStop = 1; }
} // namespace

int main(int argc, char **argv) {
  cxxopts::Options options("ffb_example",
                           "Play a DirectInput force feedback effect");
  options.add_options()("e,effect",
                        "Effect to play: sine, square, sawtooth-down, "
                        "sawtooth-up, triangle, constant, ramp, spring, "
                        "damper, friction, inertia",
                        cxxopts::value<std::string>()->default_value("sine"))(
      "n,num_updates",
      "Number of updates to send before stopping. Use 0 to send a single "
      "initial update and then wait for Ctrl+C.",
      cxxopts::value<std::uint64_t>()->default_value("1000"))(
      "m,mutate_forces",
      "If true, mutate the force parameters on each update. If false, keep "
      "the initial force values and only resend them.",
      cxxopts::value<bool>()->default_value("false"))(
      "s,strength_percentage",
      "Strength percentage to apply to the effect, from 0 to 100. Defaults to "
      "25.",
      cxxopts::value<int>()->default_value("25"))("h,help", "Print help");

  try {
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      return 0;
    }

    std::signal(SIGINT, HandleSignal);

    std::string effectName = result["effect"].as<std::string>();
    std::uint64_t numUpdates = result["num_updates"].as<std::uint64_t>();
    bool mutateForces = result["mutate_forces"].as<bool>();
    int strengthPercentage = result["strength_percentage"].as<int>();
    GUID effectGuid = di_common::kSineEffectGuid;
    if (!di_common::ResolveEffectGuid(effectName, effectGuid)) {
      std::cerr << "Unknown effect '" << effectName
                << "'. Expected one of: sine, square, sawtooth-down, "
                   "sawtooth-up, triangle, constant, ramp, spring, damper, "
                   "friction, inertia"
                << std::endl;
      return 2;
    }

    std::cout << "Using effect: " << effectName << std::endl;
    if (numUpdates == 0) {
      std::cout << "Sending one initial update and waiting for Ctrl+C."
                << std::endl;
    } else {
      std::cout << "Sending " << numUpdates << " updates." << std::endl;
    }
    if (mutateForces) {
      std::cout << "Force values will mutate on each update." << std::endl;
    } else {
      std::cout << "Force values will remain fixed across updates."
                << std::endl;
    }
    const int clampedStrength =
        di_common::ClampStrengthPercentage(strengthPercentage);
    std::cout << "Strength percentage: " << clampedStrength << "%" << std::endl;

    std::string normalizedEffectName =
        di_common::NormalizeEffectName(effectName);

    IDirectInput8 *directInput = nullptr;
    std::vector<IDirectInputDevice8 *> forceFeedbackDevices;

    HRESULT hr =
        DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                           IID_IDirectInput8, (void **)&directInput, nullptr);
    if (!di_common::CheckDInputResult(hr, "DirectInput8Create")) {
      return 1;
    } else if (!directInput) {
      std::cerr << "directInput unexpectedly null" << std::endl;
      return 1;
    }

    di_common::EnumDevicesContext enumContext{directInput, &forceFeedbackDevices};
    hr = directInput->EnumDevices(
        DI8DEVCLASS_GAMECTRL, di_common::EnumDevicesCallback, &enumContext,
        DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
    if (!di_common::CheckDInputResult(hr, "EnumDevices")) {
      directInput->Release();
      return 1;
    }

    std::cout << "Found " << forceFeedbackDevices.size()
              << " force feedback devices." << std::endl;
    for (IDirectInputDevice8 *device : forceFeedbackDevices) {
      DIEFFECT effect;
      void *typeSpecificParams = nullptr;
      DWORD typeSpecificParamSize = 0;
      if (!di_common::BuildEffectParameters(
              effectName, effect, typeSpecificParams, typeSpecificParamSize, 0,
              strengthPercentage)) {
        std::cerr << "Unsupported effect type: " << effectName << std::endl;
        return 2;
      }

      IDirectInputEffect *effectInterface = nullptr;
      hr = device->CreateEffect(effectGuid, &effect, &effectInterface, nullptr);
      if (!di_common::CheckDInputResult(hr, "CreateEffect")) {
        return 1;
      } else {
        hr = effectInterface->Start(1, 0);
        if (!di_common::CheckDInputResult(hr, "Start")) {
          return 1;
        }
        const auto start_time = std::chrono::high_resolution_clock::now();
        std::uint64_t updateCount = 0;
        if (numUpdates == 0) {
          // Special case; run until interrupted.
          if (!di_common::BuildEffectParameters(
                  effectName, effect, typeSpecificParams, typeSpecificParamSize,
                  0, strengthPercentage)) {
            std::cerr << "Unsupported effect type: " << effectName << std::endl;
            return 2;
          }
          hr = effectInterface->SetParameters(&effect, DIEP_TYPESPECIFICPARAMS);
          if (!di_common::CheckDInputResult(hr, "SetParameters")) {
            return 1;
          }
          ++updateCount;
          while (!g_shouldStop) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
        } else {
          while (!g_shouldStop && updateCount < numUpdates) {
            const std::uint64_t iterationIndex = mutateForces ? updateCount : 0;
            if (!di_common::BuildEffectParameters(
                    effectName, effect, typeSpecificParams,
                    typeSpecificParamSize, iterationIndex,
                    strengthPercentage)) {
              std::cerr << "Unsupported effect type: " << effectName
                        << std::endl;
              return 2;
            }
            hr = effectInterface->SetParameters(&effect,
                                                DIEP_TYPESPECIFICPARAMS);
            if (!di_common::CheckDInputResult(hr, "SetParameters")) {
              return 1;
            }
            ++updateCount;
          }
        }
        const auto kEndTime = std::chrono::high_resolution_clock::now();
        const auto kDuration =
            std::chrono::duration_cast<std::chrono::milliseconds>(kEndTime -
                                                                  start_time);
        const float kAverage =
            updateCount > 0
                ? static_cast<float>(kDuration.count()) / updateCount
                : 0.0f;
        std::cout << "Total run time for " << std::dec << updateCount
                  << " updates: " << kDuration.count()
                  << " ms, average: " << kAverage << " ms" << std::endl;
        if (g_shouldStop) {
          std::cout << "Interrupted by Ctrl+C; stopping cleanly." << std::endl;
        }
        effectInterface->Stop();
        effectInterface->Release();
      }
      device->Release();
    }
    directInput->Release();
    return 0;
  } catch (const cxxopts::exceptions::exception &ex) {
    std::cerr << ex.what() << std::endl;
    std::cerr << options.help() << std::endl;
    return 2;
  }
}
