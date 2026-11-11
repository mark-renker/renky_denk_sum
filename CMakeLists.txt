# RenkySum — vintage console summing plug-in (Faust DSP inside a JUCE plug-in).
#
# Layout
#   dsp/RenkySum.dsp            the DSP (Faust)
#   Source/dsp/RenkySumDSP.h    generated from it (committed; scripts/regen_dsp.sh)
#   Source/                     plug-in: parameters table, engine, processor, editor
#   tests/measure.cpp           DSP measurement harness (ctest)
#
# Targets: RenkySum_AU (macOS), RenkySum_VST3, RenkySum_Standalone, renkysum_measure.
# Bundles land in <build>/RenkySum_artefacts/<config>/{AU,VST3,Standalone}.
cmake_minimum_required(VERSION 3.22)

if(APPLE)
    set(RENKYSUM_ARCHS "arm64;x86_64" CACHE STRING "macOS architectures (use arm64 for a fast dev build)")
    set(CMAKE_OSX_ARCHITECTURES "${RENKYSUM_ARCHS}" CACHE STRING "" FORCE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
endif()

project(RenkySum VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(RENKYSUM_COPY_PLUGIN "Copy built plug-ins into the user plug-in folders after each build" OFF)

# ---- JUCE: explicit path, local checkout, or fetch 8.0.6 -------------------
include(FetchContent)
if(DEFINED RENKYSUM_JUCE_PATH AND EXISTS "${RENKYSUM_JUCE_PATH}/CMakeLists.txt")
    add_subdirectory("${RENKYSUM_JUCE_PATH}" JUCE EXCLUDE_FROM_ALL)
elseif(EXISTS "$ENV{HOME}/JUCE/CMakeLists.txt")
    message(STATUS "Using local JUCE at $ENV{HOME}/JUCE")
    add_subdirectory("$ENV{HOME}/JUCE" JUCE EXCLUDE_FROM_ALL)
else()
    FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

set(RENKYSUM_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND RENKYSUM_FORMATS AU)
endif()

# ---- plug-in ---------------------------------------------------------------
# Manufacturer/plug-in codes are kept from the original faust2juce build
# (aufx d8aa Manu) so existing sessions keep loading the same plug-in identity.
juce_add_plugin(RenkySum
    PRODUCT_NAME                "RenkySum"
    COMPANY_NAME                "Renky Denk"
    BUNDLE_ID                   com.renkydenk.renkysum
    PLUGIN_MANUFACTURER_CODE    Manu
    PLUGIN_CODE                 d8aa
    FORMATS                     ${RENKYSUM_FORMATS}
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES             Fx Distortion
    AU_MAIN_TYPE                kAudioUnitType_Effect
    COPY_PLUGIN_AFTER_BUILD     ${RENKYSUM_COPY_PLUGIN})

target_sources(RenkySum PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Parameters.cpp
    Source/dsp/ConsoleEngine.cpp
    Source/ui/RenkyLookAndFeel.cpp)

target_include_directories(RenkySum PRIVATE Source)

target_compile_definitions(RenkySum PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(RenkySum
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# ---- DSP regeneration (only needed after editing dsp/RenkySum.dsp) ---------
find_program(FAUST_EXECUTABLE faust)
if(FAUST_EXECUTABLE AND NOT WIN32)
    add_custom_target(regen_dsp
        COMMAND "${CMAKE_SOURCE_DIR}/scripts/regen_dsp.sh"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Regenerating Source/dsp/RenkySumDSP.h from dsp/RenkySum.dsp")
endif()

# ---- tests -----------------------------------------------------------------
# Pure-C++ measurement harness over the generated DSP (no JUCE). Reproduces
# the figures in the README and fails on regressions:  ctest --test-dir build
enable_testing()
add_executable(renkysum_measure tests/measure.cpp)
target_include_directories(renkysum_measure PRIVATE Source)
target_compile_features(renkysum_measure PRIVATE cxx_std_17)
add_test(NAME dsp_measurements COMMAND renkysum_measure)
