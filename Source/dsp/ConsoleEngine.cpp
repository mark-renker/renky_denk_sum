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
        auto* zone = zones->zoneFor (s.faustPath);
        auto* src  = apvts.getRawParameterValue (s.id);

        jassert (zone != nullptr); // Faust control path not found: see Parameters.cpp vs dsp/RenkySum.dsp
        jassert (src  != nullptr);

        if (zone != nullptr && src != nullptr)
            links.push_back ({ src, zone });
    }

    stageParam = apvts.getRawParameterValue (rs::id::stage);
}

void ConsoleEngine::prepare (double sampleRate, int maxBlockSize)
{
    maxBlock = juce::jmax (16, maxBlockSize);
    scratch.setSize (2, maxBlock);
    dsp.init ((int) sampleRate);
    pushParameters();
}

void ConsoleEngine::reset()
{
    dsp.instanceClear();
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

        FAUSTFLOAT* ins[2]  = { scratch.getWritePointer (0), scratch.getWritePointer (1) };
        FAUSTFLOAT* outs[2] = { buffer.getWritePointer (0, start), buffer.getWritePointer (1, start) };
        dsp.compute (n, ins, outs);

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
