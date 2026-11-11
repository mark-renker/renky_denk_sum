#include "Parameters.h"

namespace rs
{

const std::vector<ParamSpec>& specs()
{
    static const std::vector<ParamSpec> table
    {
        { id::model,     "Model",       "/RenkySum/Console/Model",    ParamKind::Choice, 0, 2, 1, 0, Display::Plain,
          { "CLASS-A XFMR", "VCA BUS", "DISCRETE OP-AMP" } },
        { id::stage,     "Stage",       "/RenkySum/Console/Stage",    ParamKind::Choice, 0, 1, 1, 0, Display::Plain,
          { "CHANNEL", "MIX BUS" } },
        { id::channel,   "Channel",     "/RenkySum/Console/Channel",  ParamKind::Int,    1, 32, 1, 1, Display::Integer },

        { id::drive,     "Drive",       "/RenkySum/Colour/Drive",     ParamKind::Float, -12, 12, 0.1f, 0,     Display::Decibels, {}, true },
        { id::harmonics, "Harmonics",   "/RenkySum/Colour/Harmonics", ParamKind::Float,   0,  1, 0.01f, 0.35f, Display::Percent },
        { id::depth,     "3D Depth",    "/RenkySum/Colour/3D Depth",  ParamKind::Float,   0,  1, 0.01f, 0.35f, Display::Percent },

        { id::output,    "Output",      "/RenkySum/Output/Output",    ParamKind::Float, -12, 12, 0.1f, 0,     Display::Decibels, {}, true },
        { id::autoGain,  "Auto Gain",   "/RenkySum/Output/Auto Gain",   ParamKind::Bool, 0, 1, 1, 0 },
        { id::noise,     "Noise Floor", "/RenkySum/Output/Noise Floor", ParamKind::Bool, 0, 1, 1, 0 },

        // Engine-side (no Faust zone): the saturators run at 2x/4x the host rate.
        { id::oversampling, "Oversampling", nullptr, ParamKind::Choice, 0, 2, 1, 1, Display::Plain,
          { "OFF", "2X", "4X" } },
    };
    return table;
}

const ParamSpec& spec (const juce::String& id)
{
    for (auto& s : specs())
        if (id == s.id)
            return s;

    jassertfalse; // unknown parameter id
    return specs().front();
}

static juce::String formatValue (const ParamSpec& s, float v)
{
    switch (s.display)
    {
        case Display::Decibels: return juce::String (v, 1) + " dB";
        case Display::Percent:  return juce::String (juce::roundToInt (v * 100.0f)) + " %";
        case Display::Integer:  return juce::String (juce::roundToInt (v));
        case Display::Plain:    break;
    }
    return juce::String (v, 2);
}

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (const auto& s : specs())
    {
        const juce::ParameterID pid { s.id, 1 };

        switch (s.kind)
        {
            case ParamKind::Float:
            {
                auto attrs = juce::AudioParameterFloatAttributes()
                                 .withStringFromValueFunction ([&s] (float v, int) { return formatValue (s, v); });
                layout.add (std::make_unique<juce::AudioParameterFloat> (
                    pid, s.name, juce::NormalisableRange<float> (s.min, s.max, s.step), s.defaultValue, attrs));
                break;
            }
            case ParamKind::Int:
                layout.add (std::make_unique<juce::AudioParameterInt> (
                    pid, s.name, (int) s.min, (int) s.max, (int) s.defaultValue));
                break;

            case ParamKind::Choice:
                layout.add (std::make_unique<juce::AudioParameterChoice> (
                    pid, s.name, s.choices, (int) s.defaultValue));
                break;

            case ParamKind::Bool:
                layout.add (std::make_unique<juce::AudioParameterBool> (
                    pid, s.name, s.defaultValue > 0.5f));
                break;
        }
    }

    return layout;
}

} // namespace rs
