# dinput-ffb-example
DirectInput Force Feedback example code

## Prerequisites

*   **Windows Operating System:** Windows 10/11.
*   **Development Environment:**
    *   **C++ Development Environment:** Visual Studio version 2017 or later is
        recommended. Make sure the "Desktop development with C++" workload, as
        well as "C++ CMake Tools", is installed.
    *   **Windows SDK:** A recent version of the Windows SDK is required, which
        usually comes bundled with Visual Studio's C++ workload. DirectInput
        headers (`dinput.h`) and libraries (`dinput8.lib`, `dxguid.lib`) are
        part of the SDK.
*   **Hardware:** A DirectInput-compatible Force Feedback device (joystick,
    steering wheel, etc.) connected to your PC with appropriate drivers
    installed.

## Building the Code

1.  **Clone the repository:**
    ```bash
    git clone <your-repository-url>
    cd dinput-ffb-example
    ```
    Alternatively you can use the "Clone a Repository" option from the launch
    screen of Visual Studio.
2.  **Open the solution in Visual Studio:**
    *   If you cloned via command line rather than through Visual Studio, use
        the "Open Folder" option to open the project folder; it should be
        auto-detected as a CMake project.
3.  **Select Build Configuration:**
    *   Choose a configuration (e.g., `Debug` or `Release`) and platform
        (e.g., `x64` or `x86`) from the dropdown menus in the Visual Studio
        toolbar.
4.  **Build the Solution:**
    *   Go to the `Build` menu and select `Build Solution` (or press `F7`).
    *   Check the `Output` window in Visual Studio for any compilation errors.

## Running the Examples

1.  **Ensure Device is Connected:** Make sure your force feedback device is
    plugged in and recognized by Windows.
2.  **Run from Visual Studio:**
    *   You can run the application directly from Visual Studio by pressing `F5`
        (Start Debugging) or `Ctrl+F5` (Start Without Debugging).
3.  **Run Executable Directly:**
    *   Alternatively, navigate to the build output directory (e.g.,
        `build\bin` relative to your solution file, look inside the folders
        named after hte chosen variant and architecture).
    *   Find the compiled executable (e.g., `ffb_example.exe`) and
        double-click it to run.

### FFB Example

Allows running a single force feedback effect, with the option to run it with varied gains
or fixed. Can also be used to benchmark the device's command update rate.

The code is at [`ffb_example.cpp`](./src/ffb_example.cpp).

#### Command-line Options

The example supports the following command-line arguments:

*   `-e, --effect <name>`: Select the effect to play. Supported values include
    `sine`, `square`, `sawtooth-down`, `sawtooth-up`, `triangle`,
    `constant`, `ramp`, `spring`, `damper`, `friction`, and `inertia`.
*   `-n, --num_updates <count>`: Send this many updates before stopping. Use `0`
    to send a single initial update and then wait for Ctrl+C.
*   `-m, --mutate_forces`: If provided, mutate the force values on each update.
    If omitted, the initial force values are reused for each update.
*   `-s, --strength_percentage <percent>`: Scale the applied force strength from
    `0` to `100`. The value is clamped to that range and defaults to `25`.
*   `-h, --help`: Print the command-line help text.

#### Example Commands

Run spring with no updates indefinitely:

```bash
ffb_example.exe --effect spring --num_updates 0
```

Run constant with mutating updates 2000 times and 75% strength:

```bash
ffb_example.exe --effect constant --num_updates 2000 --mutate_forces --strength_percentage 75
```

Run sine without mutation but with updates 5000 times:

```bash
ffb_example.exe --effect sine --num_updates 5000
```

### Emulated Spring

This example reads the X-axis position from a connected DirectInput device and converts
it into a scaled constant-force output. It prints the raw X-axis value and the scaled force
value to the console, and continues updating until Ctrl+C is pressed.

The code is at [`emulated_spring.cpp`](./src/emulated_spring.cpp).

### Force Spikes

This example creates a background sine force that remains active while periodically
injecting short constant-force spikes. The spike timing is controlled by command-line flags,
allowing you to tune the background strength, spike duration, and delay between spikes.

> Be careful with this example as it will spike force and therefore motor current.

The code is at [`force_spikes.cpp`](./src/force_spikes.cpp).

#### Command-line Options

*   `-b, --background_strength <percent>`: Set the strength of the background sine force
    from `0` to `100`. Defaults to `10`.
*   `-s, --spike_duration_ms <ms>`: Set the duration of each constant-force spike from `1`
    to `10000` milliseconds. Defaults to `30`.
*   `-d, --spike_delay_ms <ms>`: Set the delay between spikes in milliseconds. The value must
    be greater than `1`. Defaults to `500`.
*   `-h, --help`: Print the command-line help text.

#### Example Command

```bash
force_spikes.exe --background_strength 25 --spike_duration_ms 50 --spike_delay_ms 750
```
