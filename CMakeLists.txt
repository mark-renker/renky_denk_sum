# RenkySum — vintage console summing plug-in (Faust -> JUCE).
# RenkySum/FaustPluginProcessor.cpp is the committed faust2juce output of RenkySum.dsp, so this builds
# with CMake + JUCE alone (no Faust needed). Regenerate it with:
#   faust2juce -jucemodulesdir ~/JUCE/modules RenkySum.dsp
# Targets: RenkySum_AU, RenkySum_VST3, RenkySum_Standalone (bundles in build*/RenkySum_artefacts/Release).
cmake_minimum_required(VERSION 3.22)
project(RenkySum VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(APPLE)
  set(RENKYSUM_ARCHS "arm64;x86_64" CACHE STRING "macOS architectures (use arm64 for a fast dev build)")
  set(CMAKE_OSX_ARCHITECTURES "${RENKYSUM_ARCHS}" CACHE STRING "" FORCE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
endif()
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# ---- JUCE: local checkout when present, otherwise fetch 8.0.6 --------------
include(FetchContent)
if(DEFINED RENKYSUM_JUCE_PATH AND EXISTS "${RENKYSUM_JUCE_PATH}/CMakeLists.txt")
  add_subdirectory("${RENKYSUM_JUCE_PATH}" JUCE)
elseif(EXISTS "$ENV{HOME}/JUCE/CMakeLists.txt")
  message(STATUS "Using local JUCE at $ENV{HOME}/JUCE")
  add_subdirectory("$ENV{HOME}/JUCE" JUCE)
else()
  FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(JUCE)
endif()

set(RENKYSUM_FORMATS VST3 Standalone)
if(APPLE)
  list(APPEND RENKYSUM_FORMATS AU)
endif()

# Manufacturer/plug-in codes are kept from the original faust2juce build (aufx d8aa Manu) so existing
# sessions keep loading the same plug-in identity.
juce_add_plugin(RenkySum
  PRODUCT_NAME "RenkySum"
  COMPANY_NAME "Renky Denk"
  BUNDLE_ID com.renkydenk.renkysum
  PLUGIN_MANUFACTURER_CODE Manu
  PLUGIN_CODE d8aa
  FORMATS ${RENKYSUM_FORMATS}
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  VST3_CATEGORIES Fx Distortion
  AU_MAIN_TYPE kAudioUnitType_Effect
  COPY_PLUGIN_AFTER_BUILD FALSE)

# The Faust architecture includes both "JuceLibraryCode/JuceHeader.h" and "../JuceLibraryCode/JuceHeader.h".
# CMake generates that header under <binary-dir>/RenkySum_artefacts/JuceLibraryCode, so the source is compiled
# from a copy in <binary-dir>/RenkySum_artefacts/Source: both spellings then resolve to the one generated header
# (a stale Projucer RenkySum/JuceLibraryCode/ next to the original would otherwise be picked up as a second copy).
set(RENKYSUM_GEN "${CMAKE_CURRENT_BINARY_DIR}/RenkySum_artefacts")
configure_file(RenkySum/FaustPluginProcessor.cpp "${RENKYSUM_GEN}/Source/FaustPluginProcessor.cpp" COPYONLY)
target_sources(RenkySum PRIVATE "${RENKYSUM_GEN}/Source/FaustPluginProcessor.cpp")
juce_generate_juce_header(RenkySum)
target_include_directories(RenkySum PRIVATE "${RENKYSUM_GEN}")
target_compile_definitions(RenkySum PUBLIC
  FAUSTFLOAT=float
  JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_DISPLAY_SPLASH_SCREEN=0)
target_link_libraries(RenkySum
  PRIVATE juce::juce_audio_utils
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags)
