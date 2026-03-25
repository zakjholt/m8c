cmake_minimum_required(VERSION 3.20)

project(m8c LANGUAGES C VERSION 2.2.4)

set(APP_NAME m8c)

option(USE_LIBSERIALPORT "Use libserialport as a backend" OFF)
option(USE_LIBUSB "Use libusb as a backend" OFF)
option(USE_RTMIDI "Use RtMidi as a backend" OFF)
option(M8C_USE_SDL2 "Link SDL2 (e.g. TrimUI /usr/trimui/lib) instead of SDL3" OFF)

# Enable USE_LIBSERIALPORT by default if neither USE_LIBUSB nor USE_RTMIDI are defined
if (NOT USE_LIBUSB AND NOT USE_RTMIDI)
    message(STATUS "Neither USE_LIBUSB nor USE_RTMIDI are enabled. Enabling USE_LIBSERIALPORT by default.")
    set(USE_LIBSERIALPORT ON)
endif ()

file(GLOB m8c_SRC "src/*.h" "src/*.c" "src/backends/*.h" "src/backends/*.c" "src/fonts/*.h" "src/fonts/*.c")

if (M8C_USE_SDL2)
    list(FILTER m8c_SRC EXCLUDE REGEX ".*/audio_sdl\\.c$")
    list(APPEND m8c_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/backends/audio_sdl2.c")
    list(APPEND m8c_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/m8c_sdl_compat.c")
endif ()

if (NOT USE_LIBUSB)
    list(FILTER m8c_SRC EXCLUDE REGEX ".*/m8_libusb\\.c$")
    list(FILTER m8c_SRC EXCLUDE REGEX ".*/audio_libusb\\.c$")
endif ()

if (NOT USE_RTMIDI)
    list(FILTER m8c_SRC EXCLUDE REGEX ".*/m8_rtmidi\\.c$")
endif ()

set(MACOS_CONTENTS "${CMAKE_CURRENT_SOURCE_DIR}/package/macos/m8c.app/Contents")
set(MACOS_ENTITLEMENTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/package/macos/Entitlements.plist")

set(APP_ICON ${MACOS_CONTENTS}/Resources/m8c.icns)
set_source_files_properties(${APP_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")

if (APPLE)
    add_executable(${APP_NAME} MACOSX_BUNDLE ${APP_ICON} ${m8c_SRC})
elseif (WIN32)
    add_executable(${APP_NAME} WIN32 ${m8c_SRC})
else ()
    add_executable(${APP_NAME} ${m8c_SRC})
endif ()

target_include_directories(${APP_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

find_package(PkgConfig REQUIRED)

if (M8C_USE_SDL2)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    target_compile_definitions(${APP_NAME} PRIVATE M8C_USE_SDL2)
    target_include_directories(${APP_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${APP_NAME} PRIVATE ${SDL2_LIBRARIES})
    target_compile_options(${APP_NAME} PRIVATE ${SDL2_CFLAGS_OTHER})
else ()
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)
    pkg_check_modules(SDL3 REQUIRED sdl3)
    target_link_options(${APP_NAME} PRIVATE ${SDL3_LDFLAGS})
    target_include_directories(${APP_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})
    target_compile_options(${APP_NAME} PRIVATE ${SDL3_CFLAGS_OTHER})
endif ()

if (USE_LIBSERIALPORT)
    pkg_check_modules(LIBSERIALPORT REQUIRED libserialport)
    target_link_options(${APP_NAME} PRIVATE ${LIBSERIALPORT_LDFLAGS})
    target_include_directories(${APP_NAME} PRIVATE ${LIBSERIALPORT_INCLUDE_DIRS})
    target_compile_options(${APP_NAME} PRIVATE ${LIBSERIALPORT_CFLAGS_OTHER})
    target_compile_definitions(${APP_NAME} PRIVATE USE_LIBSERIALPORT)
    target_link_libraries(${APP_NAME} PRIVATE ${LIBSERIALPORT_LIBRARIES})
endif ()

if (USE_LIBUSB)
    pkg_check_modules(LIBUSB REQUIRED libusb-1.0)
    target_link_options(${APP_NAME} PRIVATE ${LIBUSB_LDFLAGS})
    target_include_directories(${APP_NAME} PRIVATE ${LIBUSB_INCLUDE_DIRS})
    target_compile_options(${APP_NAME} PRIVATE ${LIBUSB_CFLAGS_OTHER})
    target_compile_definitions(${APP_NAME} PRIVATE USE_LIBUSB)
endif ()

if (USE_RTMIDI)
    pkg_check_modules(RTMIDI REQUIRED rtmidi)
    target_link_options(${APP_NAME} PRIVATE ${RTMIDI_LDFLAGS})
    target_include_directories(${APP_NAME} PRIVATE ${RTMIDI_INCLUDE_DIRS})
    target_compile_options(${APP_NAME} PRIVATE ${RTMIDI_CFLAGS_OTHER})
    target_compile_definitions(${APP_NAME} PRIVATE USE_RTMIDI)
endif ()

if (WIN32)
    if (M8C_USE_SDL2)
        target_link_libraries(${APP_NAME} PRIVATE ${SDL2_LIBRARIES})
    else ()
        target_link_libraries(${APP_NAME} ${SDL3_LIBRARIES})
    endif ()
endif ()

if (APPLE)
    # Destination paths below are relative to ${CMAKE_INSTALL_PREFIX}
    install(TARGETS ${APP_NAME}
            BUNDLE DESTINATION . COMPONENT Runtime
            RUNTIME DESTINATION bin COMPONENT Runtime
    )

    set(CODESIGN_CERT_NAME "" CACHE STRING "The developer ID cert name for codesign")

    set_target_properties(${APP_NAME} PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/package/macos/m8c.app/Contents/Info.plist"
            MACOSX_BUNDLE_BUNDLE_NAME "m8c"
            MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025 laamaa. All rights reserved."
            MACOSX_BUNDLE_GUI_IDENTIFIER "fi.laamaa.m8c"
            MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
            MACOSX_BUNDLE_ICON_FILE "m8c.icns"
    )

    set(APPS "\${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app")

    if(CODESIGN_CERT_NAME)
        # Use the specified certificate
        install(CODE "include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\" \"\")
        execute_process(COMMAND codesign --force --options runtime --deep --entitlements ../package/macos/Entitlements.plist --sign \"${CODESIGN_CERT_NAME}\" --timestamp  \${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app)")
    else()
        # Use ad-hoc signing (self-signed) when no certificate is specified
        install(CODE "include(BundleUtilities)
        fixup_bundle(\"${APPS}\" \"\" \"\")
        execute_process(COMMAND codesign --force --options runtime --deep --entitlements ../package/macos/Entitlements.plist --sign - --timestamp  \${CMAKE_INSTALL_PREFIX}/${APP_NAME}.app)")
    endif()

    set(CPACK_PACKAGE_NAME "m8c")
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_PACKAGE_DIRECTORY "package-output")
    set(CPACK_GENERATOR "DragNDrop")
    include(CPack)
elseif (UNIX)
    install(TARGETS ${APP_NAME})
    install(DIRECTORY package/share/applications DESTINATION share/)
    install(DIRECTORY package/share/icons DESTINATION share/)
    install(FILES AUDIOGUIDE.md README.md LICENSE DESTINATION share/doc/${APP_NAME})
    install(FILES gamecontrollerdb.txt DESTINATION bin/)
endif ()
