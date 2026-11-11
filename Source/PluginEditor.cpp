#include "PluginEditor.h"

using namespace rs;

RenkySumEditor::RenkySumEditor (RenkySumProcessor& p)
    : AudioProcessorEditor (p), renkyProcessor (p),
      stage     (p.parameters(), id::stage),
      model     (p.parameters(), id::model, true),
      channel   (p.parameters(), id::channel),
      drive     (p.parameters(), id::drive),
      harmonics (p.parameters(), id::harmonics),
      depth     (p.parameters(), id::depth),
      output    (p.parameters(), id::output),
      autoGain  (p.parameters(), id::autoGain),
      noise     (p.parameters(), id::noise)
{
    setLookAndFeel (&lookAndFeel);

    for (auto* panel : { &consolePanel, &colourPanel, &outputPanel })
        addAndMakeVisible (panel);

    for (auto* c : std::initializer_list<juce::Component*> { &stageCaption, &stage, &modelCaption, &model, &channel })
        consolePanel.addAndMakeVisible (c);
    for (auto* c : std::initializer_list<juce::Component*> { &drive, &harmonics, &depth })
        colourPanel.addAndMakeVisible (c);
    for (auto* c : std::initializer_list<juce::Component*> { &output, &autoGain, &noise, &meter, &overLamp })
        outputPanel.addAndMakeVisible (c);

    for (auto* l : { &stageCaption, &modelCaption })
    {
        l->setColour (juce::Label::textColourId, theme::textDim);
        l->setJustificationType (juce::Justification::centredLeft);
    }
    stageCaption.setText ("STAGE", juce::dontSendNotification);
    modelCaption.setText ("MODEL", juce::dontSendNotification);

    channel.getSlider().setTooltip ("Per-instance component tolerance seed. Use a different number on every track.");
    depth.getSlider().setTooltip ("Scales phase rotation, dynamic harmonics, rail sag, crosstalk and noise floor.");

    setResizable (true, true);
    setResizeLimits (baseWidth * 3 / 4, baseHeight * 3 / 4, baseWidth * 2, baseHeight * 2);
    getConstrainer()->setFixedAspectRatio ((double) baseWidth / baseHeight);
    setSize (baseWidth, baseHeight);

    startTimerHz (30);
}

RenkySumEditor::~RenkySumEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void RenkySumEditor::paint (juce::Graphics& g)
{
    const auto r = getLocalBounds().toFloat();
    g.setGradientFill (juce::ColourGradient (theme::background, 0, 0, theme::backgroundLo, 0, r.getBottom(), false));
    g.fillAll();

    const float s = scale();
    auto header = getLocalBounds().toFloat().removeFromTop (54.0f * s).reduced (18.0f * s, 0);

    g.setColour (theme::text);
    g.setFont (theme::caps (22.0f * s, 0.22f));
    const auto titleW = juce::GlyphArrangement::getStringWidth (g.getCurrentFont(), "RENKYSUM");
    g.drawText ("RENKYSUM", header, juce::Justification::centredLeft, false);

    g.setColour (theme::accent);
    g.setFont (theme::caps (10.0f * s, 0.2f));
    g.drawText ("VINTAGE CONSOLE SUMMING", header.withTrimmedLeft (titleW + 16.0f * s), juce::Justification::centredLeft, false);

    g.setColour (theme::textMuted);
    g.setFont (theme::caps (10.0f * s, 0.1f));
    g.drawText ("v" JucePlugin_VersionString, header, juce::Justification::centredRight, false);

    g.setColour (theme::panelEdge);
    g.fillRect (header.getX(), header.getBottom() - 1.0f, header.getWidth(), 1.0f);
}

void RenkySumEditor::resized()
{
    const float s = scale();
    auto r = getLocalBounds();
    r.removeFromTop (juce::roundToInt (54 * s));
    r = r.reduced (juce::roundToInt (18 * s), juce::roundToInt (14 * s));

    const int gutter = juce::roundToInt (12 * s);
    const int usable = r.getWidth() - 2 * gutter;
    const int consoleW = juce::roundToInt ((float) usable * 0.335f);
    const int outputW  = juce::roundToInt ((float) usable * 0.27f);

    consolePanel.setBounds (r.removeFromLeft (consoleW));
    r.removeFromLeft (gutter);
    outputPanel.setBounds (r.removeFromRight (outputW));
    r.removeFromRight (gutter);
    colourPanel.setBounds (r);

    // ---- console: [stage + model] | channel knob
    {
        auto c = consolePanel.content().reduced (0, juce::roundToInt (6 * s));
        const int capH = juce::roundToInt (16 * s);
        const int rowH = juce::roundToInt (30 * s);
        auto left = c.removeFromLeft (juce::roundToInt ((float) c.getWidth() * 0.56f));
        c.removeFromLeft (juce::roundToInt (10 * s));

        stageCaption.setBounds (left.removeFromTop (capH));
        stageCaption.setFont (theme::caps ((float) capH * 0.7f, 0.12f));
        stage.setBounds (left.removeFromTop (rowH));
        left.removeFromTop (juce::roundToInt (10 * s));
        modelCaption.setBounds (left.removeFromTop (capH));
        modelCaption.setFont (theme::caps ((float) capH * 0.7f, 0.12f));
        model.setBounds (left.removeFromTop (rowH * 3 + juce::roundToInt (6 * s)));

        channel.setBounds (c.withSizeKeepingCentre (c.getWidth(), juce::jmin (c.getHeight(), juce::roundToInt (190 * s))));
    }

    // ---- colour: three knobs
    {
        auto c = colourPanel.content();
        const int knobH = juce::jmin (c.getHeight(), juce::roundToInt (220 * s));
        auto row = c.withSizeKeepingCentre (c.getWidth(), knobH);
        const int w = row.getWidth() / 3;
        drive.setBounds (row.removeFromLeft (w));
        harmonics.setBounds (row.removeFromLeft (w));
        depth.setBounds (row);
    }

    // ---- output: knob + meter on top, toggles below
    {
        auto c = outputPanel.content().reduced (0, juce::roundToInt (4 * s));
        const int rowH = juce::roundToInt (26 * s);
        const int gap  = juce::roundToInt (6 * s);
        auto rows = c.removeFromBottom (rowH * 3 + gap * 2);
        overLamp.setBounds (rows.removeFromTop (rowH));
        rows.removeFromTop (gap);
        autoGain.setBounds (rows.removeFromTop (rowH));
        rows.removeFromTop (gap);
        noise.setBounds (rows.removeFromTop (rowH));

        c.removeFromBottom (juce::roundToInt (10 * s));
        auto meterArea = c.removeFromRight (juce::roundToInt ((float) c.getWidth() * 0.34f));
        meter.setBounds (meterArea.reduced (juce::roundToInt (6 * s), juce::roundToInt (4 * s)));
        output.setBounds (c.withSizeKeepingCentre (c.getWidth(), juce::jmin (c.getHeight(), juce::roundToInt (175 * s))));
    }
}

void RenkySumEditor::timerCallback()
{
    auto& m = renkyProcessor.meters();
    const float l = m.peakL.exchange (0.0f, std::memory_order_relaxed);
    const float r = m.peakR.exchange (0.0f, std::memory_order_relaxed);
    const bool over = m.busOverSamples.exchange (0, std::memory_order_relaxed) > 0;
    meter.push (l, r);
    overLamp.push (over);
}
