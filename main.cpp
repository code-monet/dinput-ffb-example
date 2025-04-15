#include <iostream>
#include <vector>
#include <dinput.h>

// Function to check if a device supports force feedback
bool IsForceFeedbackSupported(IDirectInputDevice8* device) {
    DIEFFECTINFO effectInfo;
    effectInfo.dwSize = sizeof(DIEFFECTINFO);
    HRESULT hr = device->GetEffectInfo(&effectInfo, GUID_ConstantForce);
    return SUCCEEDED(hr);
}

// Callback function for device enumeration
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* instance, VOID* pContext) {
    auto devices = reinterpret_cast<std::vector<IDirectInputDevice8*>*>(pContext);
    IDirectInput8* directInput = nullptr;
    HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to re-initialize DirectInput in callback: " << std::hex << hr << std::endl;
        return DIENUM_STOP;
    }

    IDirectInputDevice8* device = nullptr;
    hr = directInput->CreateDevice(instance->guidInstance, &device, nullptr);
    if (SUCCEEDED(hr)) {
        if (IsForceFeedbackSupported(device)) {
            devices->push_back(device);
        } else {
            device->Release();
        }
    }
    if (directInput) directInput->Release();
    return DIENUM_CONTINUE;
}

int main() {
    IDirectInput8* directInput = nullptr;
    std::vector<IDirectInputDevice8*> forceFeedbackDevices;

    // Initialize DirectInput
    HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize DirectInput: " << std::hex << hr << std::endl;
        return 1;
    }

    // Enumerate devices
    hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, &forceFeedbackDevices);
    if (FAILED(hr)) {
        std::cerr << "Failed to enumerate devices: " << std::hex << hr << std::endl;
        if (directInput) directInput->Release();
        return 1;
    }

    if (directInput) directInput->Release();

    std::cout << "Found " << forceFeedbackDevices.size() << " force feedback devices." << std::endl;
    for (IDirectInputDevice8* device : forceFeedbackDevices) {
        DIEFFECT effect;
        ZeroMemory(&effect, sizeof(effect));
        effect.dwSize = sizeof(DIEFFECT);
        effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
        effect.dwSamplePeriod = 0; // Continuous effect
        effect.dwGain = DI_FFNOMINALMAX;
        effect.dwTriggerButton = DIEB_NOTRIGGER;
        effect.dwTriggerRepeatInterval = 0;
        effect.cAxes = 1;
        LONG direction = 0;
        effect.lpDirection = &direction;

        // Create the sine wave parameters
        DIPERIODIC periodic;
        ZeroMemory(&periodic, sizeof(periodic));
        periodic.dwMagnitude = DI_FFNOMINALMAX;
        periodic.lOffset = 0;
        periodic.dwPhase = 0;
        periodic.dwPeriod = 1000; // 1 second period

        effect.guid = GUID_Sine;
        effect.cbTypeSpecificParams = sizeof(DIPERIODIC);
        effect.lpvTypeSpecificParams = &periodic;

        IDirectInputEffect* effectInterface = nullptr;
        hr = device->CreateEffect(GUID_Sine, &effect, &effectInterface, nullptr);
        if (FAILED(hr)) {
            std::cerr << "Failed to create effect: " << std::hex << hr << std::endl;
        }
        else
        {
            hr = effectInterface->Start(1, 0, 1000); // Play the effect 1000 times
            if (FAILED(hr)) {
                std::cerr << "Failed to start effect: " << std::hex << hr << std::endl;
            }

            effectInterface->Release();
        }


        device->Release();
    }

    return 0;
}
