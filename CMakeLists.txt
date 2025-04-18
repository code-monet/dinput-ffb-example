cmake_minimum_required(VERSION 3.10)

project(ffb_example)

if(CMAKE_GENERATOR_PLATFORM MATCHES "x64")
    set(ARCH_DIR "x64")
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "Win32")
    set(ARCH_DIR "Win32")
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH_DIR "x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_DIR "Win32")
    else()
        set(ARCH_DIR "unknown_arch")
        message(WARNING "Could not determine target architecture (x64/Win32). Using '${ARCH_DIR}'.")
    endif()
endif()

set(OUTPUT_BASE_DIR "${CMAKE_BINARY_DIR}/bin/${ARCH_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_BASE_DIR}")

add_executable(ffb_example ffb_example.cpp)
target_link_libraries(ffb_example dinput8 dxguid)
