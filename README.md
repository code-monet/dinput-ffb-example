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

## Running the Example

1.  **Ensure Device is Connected:** Make sure your force feedback device is
    plugged in and recognized by Windows.
2.  **Run from Visual Studio:**
    *   You can run the application directly from Visual Studio by pressing `F5`
        (Start Debugging) or `Ctrl+F5` (Start Without Debugging).
3.  **Run Executable Directly:**
    *   Alternatively, navigate to the build output directory (e.g.,
        `x64\Debug` or `x64\Release` relative to your solution file).
    *   Find the compiled executable (e.g., `dinput-ffb-example.exe`) and
        double-click it to run.
