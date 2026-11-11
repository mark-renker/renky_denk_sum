#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Theme.h"
#include "RenkyLookAndFeel.h"
#include "../Parameters.h"

namespace rs::ui
{

// Rounded panel with a small-caps title. Children are laid out by the owner
// inside content().
class SectionPanel : public juce::Component
{
public:
    explicit SectionPanel (juce::String titleText) : title (std::move (titleText)) {}

    juce::Rectangle<int> content() const
    {
        return getLocalBounds().reduced (pad()).withTrimmedTop (headerHeight());
    }

    void paint (juce::Graphics& g) override
    {
        const auto r = getLocalBounds().toFloat().reduced (0.5f);
        g.setColour (theme::panel);
        g.fillRoundedRectangle (r, 8.0f);
        g.setColour (theme::panelEdge);
        g.drawRoundedRectangle (r, 8.0f, 1.0f);

        const auto head = getLocalBounds().reduced (pad()).removeFromTop (headerHeight());
        g.setColour (theme::textDim);
        g.setFont (theme::caps ((float) headerHeight() * 0.62f, 0.18f));
        g.drawText (title, head, juce::Justification::centredLeft, false);

        g.setColour (theme::panelEdge);
        g.fillRect (head.getX(), head.getBottom() - 1, head.getWidth(), 1);
    }

private:
    int pad() const          { return juce::jmax (6, getHeight() / 26); }
    int headerHeight() const { return juce::jmax (14, getHeight() / 13); }
    juce::String title;
};

// Rotary parameter control: caption above, value box below.
class Knob : public juce::Component
{
public:
    Knob (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId)
    {
        const auto& s = rs::spec (paramId);
        caption.setText (juce::String (s.name).toUpperCase(), juce::dontSendNotification);
        caption.setJustificationType (juce::Justification::centred);
        caption.setColour (juce::Label::textColourId, theme::textDim);
        addAndMakeVisible (caption);

        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                                    juce::MathConstants<float>::pi * 2.75f, true);
        slider.getProperties().set (RenkyLookAndFeel::bipolarProperty, s.bipolar);
        slider.setDoubleClickReturnValue (true, s.defaultValue);
        addAndMakeVisible (slider);

        attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (apvts, paramId, slider);
    }

    void resized() override
    {
        auto r = getLocalBounds();
        const int capH = juce::jmax (12, r.getHeight() / 12);
        const int boxH = juce::jmax (14, r.getHeight() / 9);

        caption.setBounds (r.removeFromTop (capH));
        caption.setFont (theme::caps ((float) capH * 0.7f, 0.14f));
        r.removeFromTop (capH / 3);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, juce::jmin (r.getWidth(), boxH * 4), boxH);
        slider.setBounds (r);
    }

    juce::Slider& getSlider() { return slider; }

private:
    juce::Label caption;
    juce::Slider slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

// A row (or column) of mutually exclusive buttons bound to a choice parameter.
class SegmentSelector : public juce::Component
{
public:
    SegmentSelector (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId, bool vertical = false)
        : isVertical (vertical)
    {
        auto* param = apvts.getParameter (paramId);
        jassert (param != nullptr);

        for (const auto& label : rs::spec (paramId).choices)
        {
            auto* b = buttons.add (new juce::TextButton (label));
            b->setClickingTogglesState (false);
            b->setRadioGroupId (0);
            const int index = buttons.size() - 1;
            b->onClick = [this, index] { if (attachment) attachment->setValueAsCompleteGesture ((float) index); };
            addAndMakeVisible (b);
        }

        attachment = std::make_unique<juce::ParameterAttachment> (*param, [this] (float v)
        {
            const int sel = juce::roundToInt (v);
            for (int i = 0; i < buttons.size(); ++i)
                buttons[i]->setToggleState (i == sel, juce::dontSendNotification);
        });
        attachment->sendInitialUpdate();
    }

    void resized() override
    {
        auto r = getLocalBounds();
        const int n = buttons.size();
        if (n == 0) return;
        const int gap = 3;
        const int cellW = (r.getWidth()  - gap * (n - 1)) / n;
        const int cellH = (r.getHeight() - gap * (n - 1)) / n;

        for (int i = 0; i < n; ++i)
        {
            juce::Rectangle<int> cell;
            if (isVertical)
            {
                cell = r.removeFromTop (cellH);
                r.removeFromTop (gap);
            }
            else
            {
                cell = r.removeFromLeft (cellW);
                r.removeFromLeft (gap);
            }
            buttons[i]->setBounds (cell);
        }
    }

private:
    bool isVertical;
    juce::OwnedArray<juce::TextButton> buttons;
    std::unique_ptr<juce::ParameterAttachment> attachment;
};

// LED-style toggle bound to a bool parameter.
class LedToggle : public juce::Component
{
public:
    LedToggle (juce::AudioProcessorValueTreeState& apvts, const juce::String& paramId)
    {
        button.setButtonText (juce::String (rs::spec (paramId).name).toUpperCase());
        addAndMakeVisible (button);
        attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (apvts, paramId, button);
    }

    void resized() override { button.setBounds (getLocalBounds()); }

private:
    juce::ToggleButton button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
};

// Held warning lamp: lights when the MIX BUS decode left its exact region.
class OverLamp : public juce::Component
{
public:
    void push (bool over)
    {
        const bool was = frames > 0;
        if (over) frames = holdFrames;
        else if (frames > 0) --frames;
        if (was != (frames > 0)) repaint();
    }

    void paint (juce::Graphics& g) override
    {
        const auto r = getLocalBounds().toFloat();
        const bool lit = frames > 0;
        const float d = r.getHeight() * 0.42f;
        auto led = juce::Rectangle<float> (d, d).withCentre ({ r.getX() + r.getHeight() * 0.5f, r.getCentreY() });

        g.setColour (theme::inset);
        g.fillRoundedRectangle (r.reduced (1.0f), r.getHeight() * 0.3f);
        g.setColour (theme::panelEdge);
        g.drawRoundedRectangle (r.reduced (1.0f), r.getHeight() * 0.3f, 1.0f);

        if (lit) { g.setColour (theme::meterHot.withAlpha (0.35f)); g.fillEllipse (led.expanded (d * 0.5f)); }
        g.setColour (lit ? theme::meterHot : theme::ledOff);
        g.fillEllipse (led);
        g.setColour (lit ? theme::text : theme::textDim);
        g.setFont (theme::caps (r.getHeight() * 0.42f, 0.06f));
        g.drawText ("BUS OVER", r.withTrimmedLeft (r.getHeight() * 0.95f).toNearestInt(), juce::Justification::centredLeft, false);
    }

private:
    static constexpr int holdFrames = 45;
    int frames = 0;
};

// Two-channel peak meter with falling ballistics and peak hold.
class StereoMeter : public juce::Component
{
public:
    void push (float peakL, float peakR)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            const float v = ch == 0 ? peakL : peakR;
            shown[ch] = juce::jmax (v, shown[ch] * decay);
            if (v > hold[ch]) { hold[ch] = v; holdAge[ch] = 0; }
            else if (++holdAge[ch] > holdFrames) hold[ch] = juce::jmax (v, hold[ch] * 0.92f);
        }
        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        auto r = getLocalBounds().toFloat();

        // bars
        const float gap = juce::jmax (3.0f, r.getWidth() * 0.12f);
        const float barW = (r.getWidth() - gap) * 0.5f;
        for (int ch = 0; ch < 2; ++ch)
        {
            auto bar = juce::Rectangle<float> (r.getX() + (float) ch * (barW + gap), r.getY(), barW, r.getHeight());
            g.setColour (theme::inset);
            g.fillRoundedRectangle (bar, 3.0f);

            const float level = toPos (shown[ch]);
            if (level > 0.0f)
            {
                auto fill = bar.withTrimmedTop (bar.getHeight() * (1.0f - level));
                juce::ColourGradient grad (theme::meterHot, bar.getX(), bar.getY(),
                                           theme::meterLow, bar.getX(), bar.getBottom(), false);
                grad.addColour (0.25, theme::meterMid);
                g.setGradientFill (grad);
                g.fillRoundedRectangle (fill, 3.0f);
            }

            const float hp = toPos (hold[ch]);
            if (hp > 0.0f)
            {
                g.setColour (theme::text.withAlpha (0.8f));
                g.fillRect (bar.getX(), bar.getBottom() - bar.getHeight() * hp - 1.0f, bar.getWidth(), 2.0f);
            }

            g.setColour (theme::panelEdge);
            g.drawRoundedRectangle (bar, 3.0f, 1.0f);
        }
    }

private:
    static float toPos (float lin)
    {
        const float db = juce::Decibels::gainToDecibels (lin, -60.0f);
        return juce::jlimit (0.0f, 1.0f, (db + 60.0f) / 60.0f);
    }

    static constexpr float decay = 0.86f;
    static constexpr int holdFrames = 30;
    float shown[2] { 0, 0 }, hold[2] { 0, 0 };
    int holdAge[2] { 0, 0 };
};

} // namespace rs::ui
