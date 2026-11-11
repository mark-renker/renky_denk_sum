#include "PluginProcessor.h"
#include "PluginEditor.h"

RenkySumProcessor::RenkySumProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "RenkySum", rs::createParameterLayout())
{
    engine.bind (apvts);
}

void RenkySumProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    engine.prepare (sampleRate, samplesPerBlock);
    stereoScratch.setSize (2, samplesPerBlock);
    setLatencySamples (engine.latencySamples());
}

void RenkySumProcessor::reset()
{
    engine.reset();
}

bool RenkySumProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto in  = layouts.getMainInputChannelSet();
    const auto out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono())
        return false;
    if (in != juce::AudioChannelSet::stereo() && in != juce::AudioChannelSet::mono())
        return false;
    if (in == juce::AudioChannelSet::stereo() && out == juce::AudioChannelSet::mono())
        return false;

    return true;
}

void RenkySumProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numIn  = getTotalNumInputChannels();
    const int numOut = getTotalNumOutputChannels();
    const int n      = buffer.getNumSamples();

    if (numIn >= 2 && numOut >= 2)
    {
        engine.process (buffer);
        updateLatency();
        return;
    }

    // Mono paths: run the stereo engine on a duplicated signal, then take
    // the left strip for mono output (strips differ by component tolerance).
    if (stereoScratch.getNumSamples() < n)
        stereoScratch.setSize (2, n, false, false, true);

    stereoScratch.copyFrom (0, 0, buffer, 0, 0, n);
    stereoScratch.copyFrom (1, 0, buffer, 0, 0, n);

    juce::AudioBuffer<float> view (stereoScratch.getArrayOfWritePointers(), 2, n);
    engine.process (view);

    buffer.copyFrom (0, 0, view, 0, 0, n);
    if (numOut >= 2)
        buffer.copyFrom (1, 0, view, 1, 0, n);
    updateLatency();
}

void RenkySumProcessor::updateLatency()
{
    const int latency = engine.latencySamples();
    if (latency != getLatencySamples())
        setLatencySamples (latency);
}

juce::AudioProcessorEditor* RenkySumProcessor::createEditor()
{
    return new RenkySumEditor (*this);
}

void RenkySumProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty ("stateVersion", rs::stateVersion, nullptr);
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void RenkySumProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary (data, sizeInBytes);
    if (xml == nullptr || ! xml->hasTagName (apvts.state.getType()))
        return;

    auto state = juce::ValueTree::fromXml (*xml);
    // const int version = state.getProperty ("stateVersion", 1);
    // Future migrations keyed on `version` go here.
    apvts.replaceState (state);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RenkySumProcessor();
}
