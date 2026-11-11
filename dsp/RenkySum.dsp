declare name        "RenkySum";
declare version     "1.0";
declare author      "Mark Renker";
declare license     "MIT";
declare description "Vintage console summing-amp channel colour. Per-instance component tolerances, dynamic harmonics, and a 3D Depth control.";

import("stdfaust.lib");

//=============================================================================
// RenkySum — per-channel vintage summing-bus colour
//
// Put one instance on every track (like Satson / NLS / Console7). Each
// instance is made unique by the CHANNEL number: it deterministically
// detunes component tolerances (gain, HF pole, asymmetry, crosstalk,
// noise) the way two adjacent strips of a real desk never measure
// identically. Set a different channel number per track.
//
// Signal path per side:
//   [bus: decode] -> input trim -> channel-tolerance HF pole
//   -> transformer LF emphasis -> dynamic-bias saturator
//   -> LF/HF phase rotation (allpasses) -> rail sag -> crosstalk -> noise
//   -> output trim -> DC block -> [channel: encode]
//
// Conventions
//   * Parameter paths are bound by name in Source/Parameters.cpp. If you
//     rename a control here, update the table there.
//   * Anything derived only from controls is computed at control rate;
//     smoothing (si.smoo) is applied AFTER the expensive maths so the
//     per-sample loop stays to multiplies, adds and the saturators.
//=============================================================================

//---------------------------------- UI --------------------------------------

consoleSel = nentry("h:[0]Console/[0]Model[style:menu{'CLASS-A XFMR':0;'VCA BUS':1;'DISCRETE OP-AMP':2}]", 0, 0, 2, 1);
stageSel   = nentry("h:[0]Console/[1]Stage[style:menu{'CHANNEL':0;'MIX BUS':1}]", 0, 0, 1, 1);
chan       = nentry("h:[0]Console/[2]Channel[style:knob]", 1, 1, 32, 1);

driveDb    = hslider("h:[1]Colour/[0]Drive[unit:dB][style:knob]", 0, -12, 12, 0.1);
harmAmt    = hslider("h:[1]Colour/[1]Harmonics[style:knob]", 0.35, 0, 1, 0.01) : si.smoo;
depthRaw   = hslider("h:[1]Colour/[2]3D Depth[style:knob]", 0.35, 0, 1, 0.01);
depthAmt   = depthRaw : si.smoo;

outDb      = hslider("h:[2]Output/[0]Output[unit:dB][style:knob]", 0, -12, 12, 0.1);
autoComp   = checkbox("h:[2]Output/[1]Auto Gain");
noiseOn    = checkbox("h:[2]Output/[2]Noise Floor") : si.smoo;

// dB -> linear happens once per block; only the linear gain is smoothed.
drv        = driveDb : ba.db2linear : si.smoo;
outGain    = outDb   : ba.db2linear : si.smoo;
makeup     = ba.if(autoComp > 0.5, (driveDb : ba.db2linear) ^ (-0.85), 1) : si.smoo;

//------------------------- per-instance tolerances ---------------------------
// Deterministic "component spread" per channel number and per side (L and R
// of a stereo pair are two different strips on a real desk).
//
// Pure 32-bit integer arithmetic, no overflow, so every compiler and CPU
// produces the same spread: channel 7 is channel 7 on any machine.
// Three rounds of a Lehmer generator mod 65521 (16807 * 65521 < 2^31).

stripIdx(side) = (int(chan) - 1) * 2 + side;              // 0..63
hashU(side, k) = r3 with {                                 // 0..65520
    x  = stripIdx(side) * 16 + k + 1;
    r1 = (x  * 16807 + 12345) % 65521;
    r2 = (r1 * 16807 + 12345) % 65521;
    r3 = (r2 * 16807 + 12345) % 65521;
};
tol(side, k) = hashU(side, k) / 32760.0 - 1.0;            // -1..1

gainTol(s)  = 1 + 0.006 * tol(s, 0);   // +/- 0.05 dB strip gain spread
hfTol(s)    = 1 + 0.12  * tol(s, 1);   // +/- 12% HF pole spread
biasTol(s)  = 1 + 0.25  * tol(s, 2);   // asymmetry (even-harmonic) spread
lfTol(s)    = 1 + 0.15  * tol(s, 3);   // transformer LF corner spread
ctTol       = 1 + 0.30  * tol(0, 4);   // crosstalk spread
noiseTol(s) = ba.db2linear(4 * tol(s, 5)); // +/- 4 dB noise-floor spread

//----------------------------- console models --------------------------------
// evenH  : 2nd-harmonic bias (transformer / single-ended flavour)
// oddK   : 3rd-harmonic drive factor (VCA / opamp flavour)
// lfSat  : extra LF saturation (core hysteresis at low freq)
// hfFc   : open-loop HF pole (Hz)
// lfApFc : transformer/coupling LF phase-rotation corner (Hz)
// sag    : power-rail sag depth (program-dependent gain dip)
//
// Gains are smoothed so switching Model doesn't click; corner frequencies
// feed filter coefficients that are smoothed further down.

evenH  = select3(consoleSel, 0.90, 0.22, 0.55) : si.smoo;
oddK   = select3(consoleSel, 1.00, 1.35, 1.20) : si.smoo;
lfSat  = select3(consoleSel, 1.00, 0.15, 0.50) : si.smoo;
sag    = select3(consoleSel, 0.40, 0.20, 0.55) : si.smoo;
hfFc   = select3(consoleSel, 24000, 30000, 27000);
lfApFc = select3(consoleSel, 28.0, 8.0, 18.0);

//------------------------------- helpers -------------------------------------

capFc(fc) = min(fc, 0.47 * ma.SR);

// 1st-order allpass. Coefficient is computed from the corner at control rate
// and smoothed, so tan() never runs per sample.
apCoef(fc) = (t - 1) / (t + 1) with { t = tan(ma.PI * capFc(fc) / ma.SR); };
ap1(a) = fi.tf1(a, 1, a);

// Saturator with unity small-signal gain. k sets odd-harmonic drive,
// b sets asymmetry (even harmonics). DC from asymmetry removed later.
shaper(k, b, x) = (ma.tanh(kk * x + b) - ma.tanh(b)) / kk with {
    kk = max(0.05, k);
};

//------------------- complementary nonlinear summing -------------------------
// airwindows-Console-style: CHANNEL instances encode with sin(), the DAW
// sums linearly, one MIX BUS instance decodes with asin(). A single channel
// through encode+decode nulls exactly; multiple channels create the
// "shared power" interaction of a resistive summing network.
//
// asin() is only defined on [-1, 1]. A hot DAW sum (> 0 dBFS at the bus
// instance) used to be hard-clipped there. Above the knee the remaining
// headroom is compressed smoothly instead, so overs land as a soft limit.
// Below the knee the decode is still exact.

encode(x) = sin(max(-halfPi, min(halfPi, x))) with { halfPi = ma.PI * 0.5; };

decodeKnee = 0.9;
softDomain(x) = ba.if(ax < decodeKnee, x, ma.signum(x) * (decodeKnee + room * ma.tanh((ax - decodeKnee) / room)))
with {
    ax   = abs(x);
    room = 1.0 - decodeKnee;
};
decode(x) = asin(max(-0.9999, min(0.9999, softDomain(x))));

isBus = stageSel > 0.5;

//------------------------------ depth engine ---------------------------------
// The "3D" percept, per research: (1) LF/HF phase rotation moving into the
// audible band, (2) program-dependent (dynamic) harmonics, (3) rail sag,
// (4) phase-shifted inter-channel crosstalk, (5) low-level noise floor.

// shared rail-sag envelope (common power supply for both sides), measured
// on the DRIVEN signal so Drive pushes the rails harder
sagEnv = an.amp_follower_ar(0.008, 0.35);
sagGain(e) = 1 - sag * 0.12 * depthAmt * min(e, 1.5);

// phase rotation corners move into the band as Depth rises
lfRotA = apCoef(lfApFc * (1 + 3.0 * depthRaw)) : si.smoo;
hfRotA = apCoef(26000 / (1 + 2.3 * depthRaw))  : si.smoo;

// crosstalk: bled signal goes through the HF rotator (phase-blurred bleed)
ctGain = ba.db2linear(-52) * (0.25 + depthAmt) * ctTol;

//------------------------------ noise floor ----------------------------------
// Console hiss, decorrelated per strip. Each strip owns an LCG whose state
// is seeded from the channel number, so 32 instances produce 32 unrelated
// noise sequences (they used to share one seed and sum coherently). The
// generator reseeds whenever Channel changes, and on the first sample.

noiseSeed(s) = hashU(s, 6) * 32768 + hashU(s, 7);
seededNoise(s) = state / 2147483647.0 with {
    reset = chan != chan';
    state = (\(prev).(select2(reset, prev * 1103515245 + 12345, noiseSeed(s)))) ~ _;
};

hiss(s) = seededNoise(s) : fi.lowpass(1, capFc(9000))
          * ba.db2linear(-82) * noiseTol(s) * (0.3 + 0.7 * depthAmt) * noiseOn;

//------------------------------ channel strip --------------------------------

strip(side, env, x) = x * drv * gainTol(side)
    : fi.lowpass(1, capFc(hfFc * hfTol(side)))
    : lfEmphSat
    : ap1(lfRotA) : ap1(hfRotA)
    : *(sagGain(env))
with {
    // dynamic bias: even harmonics breathe with the programme (depth-scaled)
    kOdd  = 0.05 + 1.85 * harmAmt * oddK;
    bias  = 0.10 * evenH * harmAmt * biasTol(side) * (1 + 0.6 * depthAmt * min(env, 1.5));

    // transformer core works harder at LF: split, saturate lows more
    lfXover = capFc(140 * lfTol(side));
    lfEmphSat(sig) = shaper(kOdd, bias, hi) + shaper(kOdd * (1 + 1.2 * lfSat * harmAmt), bias * 1.5, lo)
    with {
        lo = sig : fi.lowpass(1, lfXover);
        hi = sig - lo;
    };
};

//------------------------------- top level -----------------------------------

xtalk(l, r) = l + ctGain * ap1(hfRotA, r),
              r + ctGain * ap1(hfRotA, l);

// MIX BUS instances decode the DAW's linear sum back to signal domain first
pre(x)  = ba.if(isBus, decode(x), x);
// CHANNEL instances encode as the very last operation before the DAW sum
stageOut(x) = ba.if(isBus, x, encode(x));
post(s) = _ + hiss(s) : *(makeup * outGain) : fi.dcblocker : stageOut;

process = _,_ : pre, pre <: _,_, ((_ + _) * 0.5 * drv : sagEnv) : procs
with {
    procs(l, r, e) = strip(0, e, l), strip(1, e, r) : xtalk : post(0), post(1);
};
