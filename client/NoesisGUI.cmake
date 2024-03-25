if (PLATFORM_WIN32)
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/windows_x86)
        set(NOESISGUI_LIBRARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Lib/windows_x86)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/uwp_arm)
        set(NOESISGUI_LIBRARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Lib/uwp_arm)
    else()
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/windows_x86_64)
        set(NOESISGUI_LIBRARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Lib/windows_x86_64)
    endif()
elseif (PLATFORM_LINUX)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL amd64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL i686)
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/linux_x86_64)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL armv7-a OR CMAKE_SYSTEM_PROCESSOR STREQUAL arm71 OR CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/linux_arm)
    else()
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/linux_arm64)
    endif()
    set(NOESISGUI_LIBRARY_DIR ${NOESISGUI_BINARY_DIR})
elseif (PLATFORM_ANDROID)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL amd64)
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/android_x86_64)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL i686)
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/android_x86)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL armv7-a OR CMAKE_SYSTEM_PROCESSOR STREQUAL arm71 OR CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/android_arm)
    else()
        set(NOESISGUI_BINARY_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Bin/android_arm64)
    endif()
    set(NOESISGUI_LIBRARY_DIR ${NOESISGUI_BINARY_DIR})
endif()
set(NOESISGUI_INCLUDE_DIR ${NEXTMU_CLIENT_BASE_DIR}/../dependencies/installed/noesisgui/Include)