#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Palette and typography for the editor. Everything visual reads from here.
namespace rs::theme
{
    inline const juce::Colour background   { 0xff1a1917 };
    inline const juce::Colour backgroundLo { 0xff141311 };
    inline const juce::Colour panel        { 0xff23221f };
    inline const juce::Colour panelEdge    { 0xff34322d };
    inline const juce::Colour inset        { 0xff121110 };

    inline const juce::Colour text         { 0xffe2dccd };
    inline const juce::Colour textDim      { 0xff8d877a };
    inline const juce::Colour textMuted    { 0xff5d584f };

    inline const juce::Colour accent       { 0xffe8a73f };   // amber
    inline const juce::Colour accentDim    { 0xff6e5124 };
    inline const juce::Colour accentGlow   { 0x55e8a73f };

    inline const juce::Colour knobFace     { 0xff2c2a26 };
    inline const juce::Colour knobFaceHi   { 0xff3a3832 };
    inline const juce::Colour knobRim      { 0xff4a4740 };
    inline const juce::Colour track        { 0xff2a2823 };

    inline const juce::Colour meterLow     { 0xff6fae5a };
    inline const juce::Colour meterMid     { 0xffe8a73f };
    inline const juce::Colour meterHot     { 0xffd9533b };
    inline const juce::Colour ledOff       { 0xff3a3028 };

    inline juce::Font font (float height, bool bold = false)
    {
        return juce::Font (juce::FontOptions (height, bold ? juce::Font::bold : juce::Font::plain));
    }

    inline juce::Font caps (float height, float kerning = 0.10f)
    {
        return font (height, true).withExtraKerningFactor (kerning);
    }
}
