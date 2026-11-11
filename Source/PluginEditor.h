#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "ui/RenkyLookAndFeel.h"
#include "ui/Widgets.h"

class RenkySumEditor : public juce::AudioProcessorEditor,
                       private juce::Timer
{
public:
    explicit RenkySumEditor (RenkySumProcessor&);
    ~RenkySumEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    static constexpr int baseWidth  = 760;
    static constexpr int baseHeight = 420;

private:
    void timerCallback() override;
    float scale() const { return (float) getWidth() / (float) baseWidth; }

    RenkySumProcessor& renkyProcessor;
    RenkyLookAndFeel lookAndFeel;

    rs::ui::SectionPanel consolePanel { "CONSOLE" };
    rs::ui::SectionPanel colourPanel  { "COLOUR" };
    rs::ui::SectionPanel outputPanel  { "OUTPUT" };

    rs::ui::SegmentSelector stage;
    rs::ui::SegmentSelector model;
    rs::ui::Knob channel, drive, harmonics, depth, output;
    rs::ui::LedToggle autoGain, noise;
    rs::ui::StereoMeter meter;
    rs::ui::OverLamp overLamp;

    juce::Label stageCaption, modelCaption;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RenkySumEditor)
};
