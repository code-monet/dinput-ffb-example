cmake_minimum_required(VERSION 3.10)

project(ffb_example LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

add_library(di_common STATIC
    src/di_common/di_common.cpp
)
target_include_directories(di_common PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/cxxopts"
)
target_link_libraries(di_common PUBLIC dinput8 dxguid)

add_executable(ffb_example src/ffb_example.cpp)
target_link_libraries(ffb_example PRIVATE di_common)

add_executable(emulated_spring src/emulated_spring.cpp)
target_link_libraries(emulated_spring PRIVATE di_common)

add_executable(force_spikes src/force_spikes.cpp)
target_link_libraries(force_spikes PRIVATE di_common)
