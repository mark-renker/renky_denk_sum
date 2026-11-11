// Measurement harness for the RenkySum DSP. No JUCE: it drives the Faust-
// generated class directly, prints the figures quoted in the README, and
// returns non-zero if any of them drift outside the expected band.
//
//   cmake --build build --target renkysum_measure && ./build/renkysum_measure

#include "dsp/FaustDsp.h"
#include "dsp/RenkySumDSP.h"
#include "dsp/FaustZoneMap.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <cstdio>
#include <string>
#include <vector>

namespace
{
constexpr int    SR     = 48000;
constexpr double TWO_PI = 6.283185307179586;

struct Instance
{
    RenkySumDSP dsp;
    FaustZoneMap zones { dsp };

    Instance() { dsp.init (SR); }

    void set (const char* path, float v)
    {
        auto* z = zones.zoneFor (path);
        if (z == nullptr) { std::printf ("!! unknown control %s\n", path); std::exit (2); }
        *z = v;
    }

    // Typical defaults, then overrides.
    void defaults()
    {
        set ("/RenkySum/Console/Model", 0);
        set ("/RenkySum/Console/Stage", 0);
        set ("/RenkySum/Console/Channel", 1);
        set ("/RenkySum/Colour/Drive", 0);
        set ("/RenkySum/Colour/Harmonics", 0.35f);
        set ("/RenkySum/Colour/3D Depth", 0.35f);
        set ("/RenkySum/Output/Output", 0);
        set ("/RenkySum/Output/Auto Gain", 0);
        set ("/RenkySum/Output/Noise Floor", 0);
    }

    void run (const std::vector<float>& inL, const std::vector<float>& inR,
              std::vector<float>& outL, std::vector<float>& outR)
    {
        const int n = (int) inL.size();
        outL.assign (n, 0); outR.assign (n, 0);
        std::vector<float> l (inL), r (inR);
        const int block = 256;
        for (int s = 0; s < n; s += block)
        {
            const int c = std::min (block, n - s);
            FAUSTFLOAT* ins[2]  = { l.data() + s, r.data() + s };
            FAUSTFLOAT* outs[2] = { outL.data() + s, outR.data() + s };
            dsp.compute (c, ins, outs);
        }
    }
};

std::vector<float> sine (double freq, double amp, int n)
{
    std::vector<float> v (n);
    for (int i = 0; i < n; ++i) v[i] = (float) (amp * std::sin (TWO_PI * freq * i / SR));
    return v;
}

// Goertzel magnitude (amplitude) of `freq` over [from, from+len) with a Hann window.
double tone (const std::vector<float>& x, double freq, int from, int len)
{
    const double w = TWO_PI * freq / SR;
    const double coeff = 2.0 * std::cos (w);
    double s0 = 0, s1 = 0, s2 = 0, wsum = 0;
    for (int i = 0; i < len; ++i)
    {
        const double win = 0.5 - 0.5 * std::cos (TWO_PI * i / (len - 1));
        wsum += win;
        s0 = x[from + i] * win + coeff * s1 - s2;
        s2 = s1; s1 = s0;
    }
    const double re = s1 - s2 * std::cos (w), im = s2 * std::sin (w);
    return 2.0 * std::sqrt (re * re + im * im) / wsum;
}

double rms (const std::vector<float>& x, int from, int len)
{
    double acc = 0;
    for (int i = 0; i < len; ++i) acc += (double) x[from + i] * x[from + i];
    return std::sqrt (acc / len);
}

double correlation (const std::vector<float>& a, const std::vector<float>& b, int from, int len)
{
    double ma = 0, mb = 0;
    for (int i = 0; i < len; ++i) { ma += a[from + i]; mb += b[from + i]; }
    ma /= len; mb /= len;
    double sab = 0, saa = 0, sbb = 0;
    for (int i = 0; i < len; ++i)
    {
        const double da = a[from + i] - ma, db = b[from + i] - mb;
        sab += da * db; saa += da * da; sbb += db * db;
    }
    return sab / std::sqrt (saa * sbb + 1e-30);
}

double dB (double lin) { return 20.0 * std::log10 (lin + 1e-30); }

int failures = 0;
void check (bool ok, const char* what)
{
    std::printf ("  [%s] %s\n", ok ? "ok" : "FAIL", what);
    if (! ok) ++failures;
}
} // namespace

int main()
{
    const int settle = SR / 2, win = SR, total = settle + win;
    const auto in = sine (1000.0, 0.251189, total); // -12 dBFS
    std::vector<float> l, r, l2, r2;

    std::printf ("RenkySum DSP measurements @ %d Hz, 1 kHz sine\n", SR);

    // ---- 1. THD, default settings, channel stage
    {
        Instance a; a.defaults();
        a.run (in, in, l, r);
        const double h1 = tone (l, 1000, settle, win);
        double thdSq = 0;
        std::printf ("THD at -12 dBFS, defaults:\n");
        for (int k = 2; k <= 5; ++k)
        {
            const double hk = tone (l, 1000.0 * k, settle, win);
            thdSq += hk * hk;
            std::printf ("    H%d %.1f dB\n", k, dB (hk / h1));
        }
        const double thd = std::sqrt (thdSq) / h1;
        std::printf ("    THD %.3f %%\n", thd * 100.0);
        check (thd > 0.001 && thd < 0.03, "THD within vintage band (0.1 %..3 %)");
    }

    // ---- 2. Clean setting residual
    {
        Instance a; a.defaults();
        a.set ("/RenkySum/Colour/Harmonics", 0);
        a.set ("/RenkySum/Colour/3D Depth", 0);
        a.run (in, in, l, r);
        const double h1 = tone (l, 1000, settle, win);
        const double h2 = tone (l, 2000, settle, win), h3 = tone (l, 3000, settle, win);
        std::printf ("Harmonics = 0, Depth = 0 (channel stage, includes sin() encode): H2 %.1f dB, H3 %.1f dB\n",
                     dB (h2 / h1), dB (h3 / h1));
        check (h2 / h1 < 0.001, "H2 residual below -60 dB");
    }

    // ---- 3. Encode / decode null: channel -> bus
    {
        Instance ch; ch.defaults(); ch.set ("/RenkySum/Colour/Harmonics", 0); ch.set ("/RenkySum/Colour/3D Depth", 0);
        Instance bus; bus.defaults(); bus.set ("/RenkySum/Console/Stage", 1);
        bus.set ("/RenkySum/Colour/Harmonics", 0); bus.set ("/RenkySum/Colour/3D Depth", 0);
        const auto hot = sine (1000.0, 0.7, total);
        ch.run (hot, hot, l, r);
        const double h1c = tone (l, 1000, settle, win), h3c = tone (l, 3000, settle, win);
        bus.run (l, r, l2, r2);
        const double h1b = tone (l2, 1000, settle, win), h3b = tone (l2, 3000, settle, win);
        std::printf ("Encode/decode null (-3 dBFS): channel H3 %.1f dB -> after bus decode H3 %.1f dB\n",
                     dB (h3c / h1c), dB (h3b / h1b));
        check (h3b / h1b < h3c / h1c * 0.1, "bus decode removes at least 20 dB of the encode's H3");
        check (h3b / h1b < 0.0003, "post-decode H3 below -70 dB");
    }

    // ---- 4. Channel-to-channel decorrelation and determinism
    {
        Instance a; a.defaults();
        Instance b; b.defaults(); b.set ("/RenkySum/Console/Channel", 2);
        Instance c; c.defaults();
        a.run (in, in, l, r); b.run (in, in, l2, r2);
        std::vector<float> diff (total);
        for (int i = 0; i < total; ++i) diff[i] = l[i] - l2[i];
        const double dec = dB (rms (diff, settle, win) / rms (l, settle, win));
        std::printf ("Channel 1 vs channel 2, same audio: %.1f dB difference\n", dec);
        check (dec < -20 && dec > -70, "channels differ by a subtle but measurable amount");

        std::vector<float> l3, r3; c.run (in, in, l3, r3);
        float maxd = 0; for (int i = 0; i < total; ++i) maxd = std::max (maxd, std::fabs (l[i] - l3[i]));
        check (maxd == 0.0f, "channel 1 vs channel 1 is bit-identical (deterministic recall)");

        const int steps = 32; double spread = 0; std::vector<double> gains;
        for (int chn = 1; chn <= steps; ++chn)
        {
            Instance t; t.defaults(); t.set ("/RenkySum/Console/Channel", (float) chn);
            t.set ("/RenkySum/Colour/Harmonics", 0); t.set ("/RenkySum/Colour/3D Depth", 0);
            std::vector<float> tl, tr; t.run (in, in, tl, tr);
            gains.push_back (dB (tone (tl, 1000, settle, win) / 0.251189));
        }
        double mn = 1e9, mx = -1e9; for (double g : gains) { mn = std::min (mn, g); mx = std::max (mx, g); }
        spread = mx - mn;
        std::printf ("Strip gain spread across 32 channels: %.3f dB\n", spread);
        check (spread > 0.02 && spread < 0.2, "gain tolerance spread is in the +/-0.05 dB range");
    }

    // ---- 5. Noise floor: level and decorrelation
    {
        const std::vector<float> silence (total, 0.0f);
        auto hissInstance = [&] (float chn)
        {
            auto p = std::make_unique<Instance>(); p->defaults();
            p->set ("/RenkySum/Console/Channel", chn);
            p->set ("/RenkySum/Output/Noise Floor", 1);
            p->set ("/RenkySum/Colour/3D Depth", 1);
            return p;
        };
        auto a = hissInstance (1), b = hissInstance (2);
        a->run (silence, silence, l, r); b->run (silence, silence, l2, r2);
        const double level = dB (rms (l, settle, win));
        const double cLR = correlation (l, r, settle, win);
        const double cAB = correlation (l, l2, settle, win);
        std::printf ("Noise floor at full Depth: %.1f dBFS RMS; L/R correlation %.3f; ch1/ch2 correlation %.3f\n",
                     level, cLR, cAB);
        check (level > -100 && level < -75, "noise floor sits around -85 dBFS");
        check (std::fabs (cLR) < 0.05, "L and R hiss are decorrelated");
        check (std::fabs (cAB) < 0.05, "hiss on channel 1 and channel 2 is decorrelated");
    }

    // ---- 6. Hot bus input stays bounded (soft decode region)
    {
        Instance bus; bus.defaults(); bus.set ("/RenkySum/Console/Stage", 1);
        const auto hot = sine (1000.0, 1.4, total);
        bus.run (hot, hot, l, r);
        float peak = 0; bool finite = true;
        for (float v : l) { finite = finite && std::isfinite (v); peak = std::max (peak, std::fabs (v)); }
        std::printf ("MIX BUS fed +2.9 dBFS: output peak %.2f (%.1f dBFS)\n", peak, dB (peak));
        check (finite, "no NaN/inf on over-range bus input");
        check (peak < 1.8, "soft decode keeps overs bounded");

        Instance ch; ch.defaults();
        ch.run (hot, hot, l, r);
        peak = 0; for (float v : l) peak = std::max (peak, std::fabs (v));
        check (peak <= 1.0f, "channel stage encode never exceeds 0 dBFS");
    }

    std::printf ("%d failure(s)\n", failures);
    return failures == 0 ? 0 : 1;
}
