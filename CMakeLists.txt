cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SimpleDeviceManagementExample)
file(GLOB_RECURSE SOURCE "Source/*.cpp")

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    add_executable( ${PROJECT_NAME} ${SOURCE} )

    find_library(COREAUDIO_LIBRARY NAMES CoreAudio)
    find_library(COREMIDI_LIBRARY NAMES CoreMIDI)
    find_library(COREFOUNDATION_LIBRARY NAMES CoreFoundation)
    find_library(AUDIOUNIT_LIBRARY NAMES AudioUnit)

    set_target_properties( ${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE OFF
        MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}")

    target_link_libraries( ${PROJECT_NAME}
        ${COREAUDIO_LIBRARY}
        ${COREMIDI_LIBRARY}
        ${COREFOUNDATION_LIBRARY}
        ${AUDIOUNIT_LIBRARY})
endif ()