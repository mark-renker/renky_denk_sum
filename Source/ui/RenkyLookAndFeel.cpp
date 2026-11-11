#include "RenkyLookAndFeel.h"
#include "Theme.h"

using namespace rs;

RenkyLookAndFeel::RenkyLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, theme::text);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, theme::accentDim);
    setColour (juce::Label::textColourId, theme::text);
    setColour (juce::Label::textWhenEditingColourId, theme::text);
    setColour (juce::Label::backgroundWhenEditingColourId, theme::inset);
    setColour (juce::Label::outlineWhenEditingColourId, theme::accent);
    setColour (juce::TextEditor::highlightColourId, theme::accentDim);
    setColour (juce::TextEditor::textColourId, theme::text);
    setColour (juce::CaretComponent::caretColourId, theme::accent);
    setColour (juce::TextButton::textColourOnId, theme::text);
    setColour (juce::TextButton::textColourOffId, theme::textDim);
    setColour (juce::ToggleButton::textColourId, theme::text);
    setColour (juce::ToggleButton::tickColourId, theme::accent);
    setColour (juce::PopupMenu::backgroundColourId, theme::panel);
    setColour (juce::PopupMenu::textColourId, theme::text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, theme::accentDim);
    setColour (juce::PopupMenu::highlightedTextColourId, theme::text);
    setColour (juce::TooltipWindow::backgroundColourId, theme::panel);
    setColour (juce::TooltipWindow::textColourId, theme::text);
    setColour (juce::TooltipWindow::outlineColourId, theme::panelEdge);
}

void RenkyLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                         float startAngle, float endAngle, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (2.0f);
    const float size  = juce::jmin (bounds.getWidth(), bounds.getHeight());
    const auto area   = bounds.withSizeKeepingCentre (size, size);
    const auto centre = area.getCentre();

    const float arcWidth  = juce::jmax (2.0f, size * 0.075f);
    const float arcRadius = size * 0.5f - arcWidth * 0.5f;
    const float angle     = startAngle + pos * (endAngle - startAngle);
    const bool bipolar    = slider.getProperties()[bipolarProperty];
    const bool enabled    = slider.isEnabled();

    // track
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, startAngle, endAngle, true);
    g.setColour (theme::track);
    g.strokePath (track, juce::PathStrokeType (arcWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // value arc
    const float from = bipolar ? (startAngle + endAngle) * 0.5f : startAngle;
    if (std::abs (angle - from) > 0.01f)
    {
        juce::Path value;
        value.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f,
                             juce::jmin (from, angle), juce::jmax (from, angle), true);
        g.setColour (enabled ? theme::accent : theme::accentDim);
        g.strokePath (value, juce::PathStrokeType (arcWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // knob body
    const float bodyRadius = arcRadius - arcWidth * 1.6f;
    const auto body = juce::Rectangle<float> (bodyRadius * 2.0f, bodyRadius * 2.0f).withCentre (centre);

    g.setColour (theme::backgroundLo.withAlpha (0.6f));
    g.fillEllipse (body.translated (0, bodyRadius * 0.08f).expanded (bodyRadius * 0.04f));

    juce::ColourGradient grad (theme::knobFaceHi, centre.x, body.getY(),
                               theme::knobFace,   centre.x, body.getBottom(), false);
    g.setGradientFill (grad);
    g.fillEllipse (body);
    g.setColour (theme::knobRim);
    g.drawEllipse (body, juce::jmax (1.0f, size * 0.012f));

    // pointer
    const float pointerLen = bodyRadius * 0.55f;
    const float pointerW   = juce::jmax (2.0f, bodyRadius * 0.12f);
    juce::Path pointer;
    pointer.addRoundedRectangle (-pointerW * 0.5f, -bodyRadius + pointerW, pointerW, pointerLen, pointerW * 0.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
    g.setColour (enabled ? theme::accent : theme::textMuted);
    g.fillPath (pointer);
}

juce::Label* RenkyLookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (slider);
    l->setJustificationType (juce::Justification::centred);
    l->setColour (juce::Label::textColourId, theme::text);
    l->setColour (juce::Label::outlineColourId, juce::Colours::transparentBlack);
    l->setColour (juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    l->setColour (juce::Label::outlineWhenEditingColourId, theme::accent);
    l->setColour (juce::Label::backgroundWhenEditingColourId, theme::inset);
    return l;
}

juce::Font RenkyLookAndFeel::getLabelFont (juce::Label& label)
{
    if (dynamic_cast<juce::Slider*> (label.getParentComponent()) != nullptr)
        return theme::font (juce::jmax (9.0f, (float) label.getHeight() * 0.6f));

    return label.getFont();
}

void RenkyLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool down)
{
    const auto bounds = b.getLocalBounds().toFloat().reduced (1.0f);
    const float corner = bounds.getHeight() * 0.3f;
    const bool on = b.getToggleState();

    g.setColour (on ? theme::panelEdge.brighter (0.08f) : theme::inset);
    g.fillRoundedRectangle (bounds, corner);
    g.setColour (highlighted || down ? theme::accentDim : theme::panelEdge);
    g.drawRoundedRectangle (bounds, corner, 1.0f);

    // LED
    const float ledD = bounds.getHeight() * 0.38f;
    auto led = juce::Rectangle<float> (ledD, ledD).withCentre ({ bounds.getX() + bounds.getHeight() * 0.5f, bounds.getCentreY() });
    if (on)
    {
        g.setColour (theme::accentGlow);
        g.fillEllipse (led.expanded (ledD * 0.45f));
    }
    g.setColour (on ? theme::accent : theme::ledOff);
    g.fillEllipse (led);

    g.setColour (on ? theme::text : theme::textDim);
    g.setFont (theme::caps (bounds.getHeight() * 0.42f, 0.06f));
    g.drawText (b.getButtonText(), bounds.withTrimmedLeft (bounds.getHeight() * 0.95f).toNearestInt(),
                juce::Justification::centredLeft, false);
}

void RenkyLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&,
                                             bool highlighted, bool down)
{
    const auto bounds = b.getLocalBounds().toFloat().reduced (1.0f);
    const float corner = juce::jmin (6.0f, bounds.getHeight() * 0.25f);
    const bool on = b.getToggleState();

    g.setColour (on ? theme::accent.withAlpha (0.18f) : (down ? theme::inset.brighter (0.05f) : theme::inset));
    g.fillRoundedRectangle (bounds, corner);
    g.setColour (on ? theme::accent : (highlighted ? theme::accentDim : theme::panelEdge));
    g.drawRoundedRectangle (bounds, corner, on ? 1.5f : 1.0f);
}

juce::Font RenkyLookAndFeel::getTextButtonFont (juce::TextButton&, int buttonHeight)
{
    return theme::caps ((float) buttonHeight * 0.42f, 0.05f);
}

void RenkyLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& b, bool, bool)
{
    g.setFont (getTextButtonFont (b, b.getHeight()));
    g.setColour (b.getToggleState() ? theme::text : theme::textDim);
    g.drawFittedText (b.getButtonText(), b.getLocalBounds().reduced (4, 0), juce::Justification::centred, 1, 0.9f);
}
