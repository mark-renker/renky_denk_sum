#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class RenkyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RenkyLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float sliderPos,
                           float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;

    juce::Label* createSliderTextBox (juce::Slider&) override;
    juce::Font getLabelFont (juce::Label&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;

    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& bg,
                               bool highlighted, bool down) override;
    void drawButtonText (juce::Graphics&, juce::TextButton&, bool highlighted, bool down) override;
    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override;

    // Set on a Slider's properties to draw the value arc from 12 o'clock.
    static inline const juce::Identifier bipolarProperty { "bipolar" };
};
