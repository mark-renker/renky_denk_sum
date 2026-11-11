#include "ConsoleEngine.h"
#include "../Parameters.h"
#include <cmath>

ConsoleEngine::ConsoleEngine()
{
    dsp.init (48000);
    zones = std::make_unique<FaustZoneMap> (dsp);
}

void ConsoleEngine::bind (juce::AudioProcessorValueTreeState& apvts)
{
    links.clear();

    for (const auto& s : rs::specs())
    {
        if (s.faustPath == nullptr)
            continue; // engine-side parameter

        auto* zone = zones->zoneFor (s.faustPath);
        auto* src  = apvts.getRawParameterValue (s.id);

        jassert (zone != nullptr); // Faust control path not found: see Parameters.cpp vs dsp/RenkySum.dsp
        jassert (src  != nullptr);

        if (zone != nullptr && src != nullptr)
            links.push_back ({ src, zone });
    }

    stageParam        = apvts.getRawParameterValue (rs::id::stage);
    oversamplingParam = apvts.getRawParameterValue (rs::id::oversampling);
}

void ConsoleEngine::prepare (double sampleRate, int maxBlockSize)
{
    hostRate = sampleRate;
    maxBlock = juce::jmax (16, maxBlockSize);
    scratch.setSize (2, maxBlock);
    osOut.setSize (2, maxBlock * 4);

    using OS = juce::dsp::Oversampling<float>;
    for (int i = 0; i < 2; ++i)
    {
        oversamplers[i] = std::make_unique<OS> (2, (size_t) i + 1, OS::filterHalfBandFIREquiripple, true, true);
        oversamplers[i]->initProcessing ((size_t) maxBlock);
    }

    osIndex = -1; // force (re)initialisation at the right rate
    selectOversampling (oversamplingParam != nullptr ? juce::roundToInt (oversamplingParam->load()) : 0);
    pushParameters();
}

void ConsoleEngine::selectOversampling (int factorIndex)
{
    factorIndex = juce::jlimit (0, 2, factorIndex);
    if (factorIndex == osIndex)
        return;

    osIndex = factorIndex;
    const int factor = 1 << osIndex;

    // The whole DSP runs at the oversampled rate: filter corners and
    // envelope times are specified in Hz / seconds, so they stay put, and
    // the saturators + sin/asin gain the extra headroom above Nyquist.
    dsp.init ((int) (hostRate * factor));

    if (osIndex > 0)
    {
        oversamplers[osIndex - 1]->reset();
        currentLatency = juce::roundToInt (oversamplers[osIndex - 1]->getLatencyInSamples());
    }
    else
    {
        currentLatency = 0;
    }
}

void ConsoleEngine::reset()
{
    dsp.instanceClear();
    for (auto& os : oversamplers)
        if (os != nullptr)
            os->reset();
}

void ConsoleEngine::computeChunk (int n, float* const* in, float* const* out)
{
    if (osIndex == 0)
    {
        dsp.compute (n, const_cast<FAUSTFLOAT**> (in), const_cast<FAUSTFLOAT**> (out));
        return;
    }

    auto& os = *oversamplers[osIndex - 1];
    juce::dsp::AudioBlock<float> inBlock (const_cast<float* const*> (in), 2, (size_t) n);
    auto up = os.processSamplesUp (inBlock);

    const int upN = (int) up.getNumSamples();
    FAUSTFLOAT* upIn[2]  = { up.getChannelPointer (0), up.getChannelPointer (1) };
    FAUSTFLOAT* upOut[2] = { osOut.getWritePointer (0), osOut.getWritePointer (1) };
    dsp.compute (upN, upIn, upOut);

    for (int ch = 0; ch < 2; ++ch)
        juce::FloatVectorOperations::copy (upIn[ch], upOut[ch], upN);

    juce::dsp::AudioBlock<float> outBlock (const_cast<float* const*> (out), 2, (size_t) n);
    os.processSamplesDown (outBlock);
}

void ConsoleEngine::pushParameters()
{
    for (auto& l : links)
        *l.zone = l.source->load (std::memory_order_relaxed);
}

void ConsoleEngine::process (juce::AudioBuffer<float>& buffer)
{
    jassert (buffer.getNumChannels() >= 2);
    pushParameters();
    if (oversamplingParam != nullptr)
        selectOversampling (juce::roundToInt (oversamplingParam->load (std::memory_order_relaxed)));

    const bool isBus = stageParam != nullptr && stageParam->load (std::memory_order_relaxed) > 0.5f;
    const int numSamples = buffer.getNumSamples();

    float peakL = 0, peakR = 0;
    int overs = 0;

    for (int start = 0; start < numSamples; start += maxBlock)
    {
        const int n = juce::jmin (maxBlock, numSamples - start);

        scratch.copyFrom (0, 0, buffer, 0, start, n);
        scratch.copyFrom (1, 0, buffer, 1, start, n);

        if (isBus)
        {
            for (int ch = 0; ch < 2; ++ch)
            {
                const float* in = scratch.getReadPointer (ch);
                for (int i = 0; i < n; ++i)
                    overs += std::fabs (in[i]) > decodeKnee ? 1 : 0;
            }
        }

        float* ins[2]  = { scratch.getWritePointer (0), scratch.getWritePointer (1) };
        float* outs[2] = { buffer.getWritePointer (0, start), buffer.getWritePointer (1, start) };
        computeChunk (n, ins, outs);

        peakL = juce::jmax (peakL, buffer.getMagnitude (0, start, n));
        peakR = juce::jmax (peakR, buffer.getMagnitude (1, start, n));
    }

    // Meters are "max since last read": the editor swaps them back to zero.
    auto raise = [] (std::atomic<float>& a, float v)
    {
        float cur = a.load (std::memory_order_relaxed);
        while (v > cur && ! a.compare_exchange_weak (cur, v, std::memory_order_relaxed)) {}
    };
    raise (meterState.peakL, peakL);
    raise (meterState.peakR, peakR);
    if (overs > 0)
        meterState.busOverSamples.fetch_add (overs, std::memory_order_relaxed);
}
