#include <chrono>
#include <csignal>
#include <cxxopts.hpp>
#include <iostream>
#include <thread>

#include "di_common/di_common.h"

namespace {
volatile std::sig_atomic_t g_shouldStop = 0;

void HandleSignal(int) { g_shouldStop = 1; }
} // namespace

int main(int argc, char **argv) {
  cxxopts::Options options(
      "force_spikes",
      "Create a background sine effect with periodic constant-force spikes");
  options.add_options()("b,background_strength",
                        "Background sine strength percentage, from 0 to 100.",
                        cxxopts::value<int>()->default_value("10"))(
      "s,spike_duration_ms",
      "Duration of each spike in milliseconds, from 1 to 10000.",
      cxxopts::value<int>()->default_value("30"))(
      "d,spike_delay_ms",
      "Delay between spikes in milliseconds, greater than 1.",
      cxxopts::value<int>()->default_value("500"))("h,help", "Print help");

  try {
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      std::cout << options.help() << std::endl;
      return 0;
    }

    std::signal(SIGINT, HandleSignal);

    int backgroundStrength = result["background_strength"].as<int>();
    int spikeDurationMs = result["spike_duration_ms"].as<int>();
    int spikeDelayMs = result["spike_delay_ms"].as<int>();

    if (backgroundStrength < 0 || backgroundStrength > 100) {
      std::cerr << "background_strength must be between 0 and 100."
                << std::endl;
      return 2;
    }
    if (spikeDurationMs < 1 || spikeDurationMs > 10000) {
      std::cerr << "spike_duration_ms must be between 1 and 10000."
                << std::endl;
      return 2;
    }
    if (spikeDelayMs <= 1) {
      std::cerr << "spike_delay_ms must be greater than 1." << std::endl;
      return 2;
    }

    std::cout << "Background sine strength: " << backgroundStrength << "%"
              << std::endl;
    std::cout << "Spike duration: " << spikeDurationMs << " ms" << std::endl;
    std::cout << "Spike delay: " << spikeDelayMs << " ms" << std::endl;

    IDirectInput8 *directInput = nullptr;
    std::vector<IDirectInputDevice8 *> forceFeedbackDevices;

    HRESULT hr =
        DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                           IID_IDirectInput8, (void **)&directInput, nullptr);
    if (!di_common::CheckDInputResult(hr, "DirectInput8Create")) {
      return 1;
    }
    if (!directInput) {
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
      std::cout << "Using device for force spike output." << std::endl;

      DIEFFECT backgroundEffect;
      void *backgroundParams = nullptr;
      DWORD backgroundParamSize = 0;
      GUID backgroundGuid = di_common::kSineEffectGuid;
      if (!di_common::ResolveEffectGuid("sine", backgroundGuid)) {
        std::cerr << "Failed to resolve sine effect GUID." << std::endl;
        device->Release();
        directInput->Release();
        return 1;
      }
      if (!di_common::BuildEffectParameters(
              "sine", backgroundEffect, backgroundParams, backgroundParamSize,
              0, backgroundStrength)) {
        std::cerr << "Failed to build background sine effect." << std::endl;
        device->Release();
        directInput->Release();
        return 1;
      }

      DIEFFECT spikeEffect;
      void *spikeParams = nullptr;
      DWORD spikeParamSize = 0;
      GUID spikeGuid = di_common::kSineEffectGuid;
      if (!di_common::ResolveEffectGuid("constant", spikeGuid)) {
        std::cerr << "Failed to resolve constant-force effect GUID."
                  << std::endl;
        device->Release();
        directInput->Release();
        return 1;
      }
      if (!di_common::BuildEffectParameters(
              "constant", spikeEffect, spikeParams, spikeParamSize, 0, 100)) {
        std::cerr << "Failed to build constant-force spike effect."
                  << std::endl;
        device->Release();
        directInput->Release();
        return 1;
      }

      IDirectInputEffect *backgroundEffectInterface = nullptr;
      hr = device->CreateEffect(backgroundGuid, &backgroundEffect,
                                &backgroundEffectInterface, nullptr);
      if (!di_common::CheckDInputResult(hr, "CreateEffect(background)")) {
        device->Release();
        directInput->Release();
        return 1;
      }

      IDirectInputEffect *spikeEffectInterface = nullptr;
      hr = device->CreateEffect(spikeGuid, &spikeEffect, &spikeEffectInterface,
                                nullptr);
      if (!di_common::CheckDInputResult(hr, "CreateEffect(spike)")) {
        backgroundEffectInterface->Release();
        device->Release();
        directInput->Release();
        return 1;
      }

      if (!di_common::CheckDInputResult(backgroundEffectInterface->Start(1, 0),
                                        "Start(background)")) {
        spikeEffectInterface->Release();
        backgroundEffectInterface->Release();
        device->Release();
        directInput->Release();
        return 1;
      }

      while (!g_shouldStop) {
        std::cout << "Starting spike" << std::endl;
        hr = spikeEffectInterface->Start(1, 0);
        if (!di_common::CheckDInputResult(hr, "Start(spike)")) {
          break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(spikeDurationMs));

        hr = spikeEffectInterface->Stop();
        if (!di_common::CheckDInputResult(hr, "Stop(spike)")) {
          break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(spikeDelayMs));
      }

      backgroundEffectInterface->Stop();
      backgroundEffectInterface->Release();
      spikeEffectInterface->Release();
      device->Release();
      break;
    }

    directInput->Release();
    return 0;
  } catch (const cxxopts::exceptions::exception &ex) {
    std::cerr << ex.what() << std::endl;
    std::cerr << options.help() << std::endl;
    return 2;
  }
}
