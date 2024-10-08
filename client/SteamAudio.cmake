if (PLATFORM_WIN32)
    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/windows-x86)
    else()
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/windows-x64)
    endif()
elseif (PLATFORM_LINUX)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL amd64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL i686)
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/linux-x64)
    else()
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/linux-x86)
    endif()
elseif (PLATFORM_MACOS)
    set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/osx)
elseif (PLATFORM_ANDROID)
    if (CMAKE_SYSTEM_PROCESSOR STREQUAL x86_64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL amd64)
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/android-x64)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL i686)
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/android-x86)
    elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL armv7-a OR CMAKE_SYSTEM_PROCESSOR STREQUAL arm71 OR CMAKE_SYSTEM_PROCESSOR STREQUAL arm)
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/android-armv7)
    else()
        set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/android-armv8)
    endif()
elseif (PLATFORM_IOS)
    set(STEAMAUDIO_LIBRARY_DIR ${SteamAudio_SOURCE_DIR}/lib/ios)
endif()
set(STEAMAUDIO_INCLUDE_DIR ${SteamAudio_SOURCE_DIR}/include)