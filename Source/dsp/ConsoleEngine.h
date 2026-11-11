#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include <memory>
#include <vector>

#include "FaustDsp.h"
JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wunused-parameter", "-Wzero-as-null-pointer-constant", "-Wfloat-equal")
#include "RenkySumDSP.h"
JUCE_END_IGNORE_WARNINGS_GCC_LIKE
#include "FaustZoneMap.h"

// Owns the Faust-generated DSP and the parameter -> zone bindings.
//
// This is the seam for future processing features that live outside Faust
// (oversampling, lookahead, sidechain, A/B snapshots): wrap or extend
// process() here and the processor/editor stay untouched.
class ConsoleEngine
{
public:
    ConsoleEngine();

    // Resolve every parameter in rs::specs() to its Faust zone. Call once
    // after the APVTS exists. Asserts (debug) on an unresolved path so a
    // renamed Faust control is caught immediately.
    void bind (juce::AudioProcessorValueTreeState& apvts);

    void prepare (double sampleRate, int maxBlockSize);
    void reset();

    // Stereo in-place processing. Pushes current parameter values into the
    // DSP zones, then runs the Faust compute in chunks of maxBlockSize.
    void process (juce::AudioBuffer<float>& buffer);

    struct Meters
    {
        std::atomic<float> peakL { 0 }, peakR { 0 };
        std::atomic<int>   busOverSamples { 0 }; // input samples past the decode knee (MIX BUS only)
    };
    Meters& meters() { return meterState; }

    // Input level above which the MIX BUS decode leaves its exact region.
    // Mirrors `decodeKnee` in dsp/RenkySum.dsp.
    static constexpr float decodeKnee = 0.9f;

private:
    struct Link
    {
        std::atomic<float>* source = nullptr;
        FAUSTFLOAT* zone = nullptr;
    };

    void pushParameters();

    RenkySumDSP dsp;
    std::unique_ptr<FaustZoneMap> zones;
    std::vector<Link> links;
    std::atomic<float>* stageParam = nullptr;

    juce::AudioBuffer<float> scratch;
    int maxBlock = 0;
    Meters meterState;
};
