#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

// Single source of truth for the plugin's parameters.
//
// Each spec drives three things: the APVTS parameter layout (host-visible
// automation + state), the binding to a Faust control zone by path, and the
// editor's labelling/formatting. To add a control: add it to dsp/RenkySum.dsp,
// regenerate (scripts/regen_dsp.sh), add one row to specs() in
// Parameters.cpp, and place it in the editor.
namespace rs
{

namespace id
{
    inline constexpr const char* model     = "model";
    inline constexpr const char* stage     = "stage";
    inline constexpr const char* channel   = "channel";
    inline constexpr const char* drive     = "drive";
    inline constexpr const char* harmonics = "harmonics";
    inline constexpr const char* depth     = "depth";
    inline constexpr const char* output    = "output";
    inline constexpr const char* autoGain  = "autoGain";
    inline constexpr const char* noise     = "noise";
}

enum class ParamKind { Float, Int, Choice, Bool };
enum class Display   { Plain, Decibels, Percent, Integer };

struct ParamSpec
{
    const char* id;
    const char* name;        // host-visible name
    const char* faustPath;   // "/RenkySum/Group/Label" in the generated DSP
    ParamKind kind;
    float min, max, step, defaultValue;
    Display display = Display::Plain;
    juce::StringArray choices {};  // Choice only
    bool bipolar = false;          // editor hint: arc drawn from centre
};

const std::vector<ParamSpec>& specs();
const ParamSpec& spec (const juce::String& id);

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

// Current plugin state schema. Bump when the parameter set changes in a way
// that needs migration in PluginProcessor::setStateInformation.
inline constexpr int stateVersion = 1;

} // namespace rs
