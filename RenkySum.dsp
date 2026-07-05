declare name        "RenkySum";
declare version     "0.9";
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
//   input trim -> channel-tolerance HF pole -> transformer LF emphasis
//   -> dynamic-bias saturator (env-modulated even harmonics)
//   -> LF/HF phase rotation (allpasses) -> crosstalk -> noise -> output
//=============================================================================

//---------------------------------- UI --------------------------------------

consoleSel = nentry("h:[0]Console/[0]Model[style:menu{'CLASS-A XFMR':0;'VCA BUS':1;'DISCRETE OP-AMP':2}]", 0, 0, 2, 1);
stageSel   = nentry("h:[0]Console/[1]Stage[style:menu{'CHANNEL':0;'MIX BUS':1}]", 0, 0, 1, 1);
chan       = nentry("h:[0]Console/[2]Channel[style:knob]", 1, 1, 32, 1);

driveDb    = hslider("h:[1]Colour/[0]Drive[unit:dB][style:knob]", 0, -12, 12, 0.1) : si.smoo;
harmAmt    = hslider("h:[1]Colour/[1]Harmonics[style:knob]", 0.35, 0, 1, 0.01) : si.smoo;
depthAmt   = hslider("h:[1]Colour/[2]3D Depth[style:knob]", 0.35, 0, 1, 0.01) : si.smoo;

outDb      = hslider("h:[2]Output/[0]Output[unit:dB][style:knob]", 0, -12, 12, 0.1) : si.smoo;
autoComp   = checkbox("h:[2]Output/[1]Auto Gain");
noiseOn    = checkbox("h:[2]Output/[2]Noise Floor");

drv        = driveDb : ba.db2linear;
outGain    = outDb   : ba.db2linear;

//------------------------- per-instance tolerances ---------------------------
// Deterministic "component spread" per channel number and per side
// (L and R of a stereo pair are two different strips on a real desk).
// tol() returns -1..1, stable across sessions.

tol(side, k) = ma.frac(sin((chan + side*0.5) * 12.9898 + k * 78.233) * 43758.5453) * 2 - 1;

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

evenH  = select3(consoleSel, 0.90, 0.22, 0.55);
oddK   = select3(consoleSel, 1.00, 1.35, 1.20);
lfSat  = select3(consoleSel, 1.00, 0.15, 0.50);
hfFc   = select3(consoleSel, 24000, 30000, 27000);
lfApFc = select3(consoleSel, 28.0, 8.0, 18.0);
sag    = select3(consoleSel, 0.40, 0.20, 0.55);

//------------------------------- helpers -------------------------------------

capFc(fc) = min(fc, 0.47 * ma.SR);

// 1st-order allpass, corner fc
ap1(fc) = fi.tf1(a, 1, a) with {
    t = tan(ma.PI * capFc(fc) / ma.SR);
    a = (t - 1) / (t + 1);
};

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

encode(x) = sin(max(-halfPi, min(halfPi, x))) with { halfPi = ma.PI * 0.5; };
decode(x) = asin(max(-1.0, min(1.0, x)));

isBus = stageSel > 0.5;

//------------------------------ depth engine ---------------------------------
// The "3D" percept, per research: (1) LF/HF phase rotation moving into the
// audible band, (2) program-dependent (dynamic) harmonics, (3) rail sag,
// (4) phase-shifted inter-channel crosstalk, (5) low-level noise floor.

// shared rail-sag envelope (common power supply for both sides)
sagEnv = an.amp_follower_ar(0.008, 0.35);
sagGain(e) = 1 - sag * 0.12 * depthAmt * min(e, 1.5);

// phase rotation corners move into the band as Depth rises
lfRotFc = lfApFc * (1 + 3.0 * depthAmt);
hfRotFc = 26000 / (1 + 2.3 * depthAmt);

// crosstalk: bled signal goes through the HF rotator (phase-blurred bleed)
ctGain = ba.db2linear(-52) * (0.25 + depthAmt) * ctTol;

// console hiss, decorrelated, gently rolled off
hiss(s) = no.noises(2, s) : fi.lowpass(1, capFc(9000))
          * ba.db2linear(-82) * noiseTol(s) * (0.3 + 0.7 * depthAmt) * noiseOn;

//------------------------------ channel strip --------------------------------

strip(side, env, x) = x * drv * gainTol(side)
    : fi.lowpass(1, capFc(hfFc * hfTol(side)))
    : lfEmphSat
    : ap1(lfRotFc) : ap1(hfRotFc)
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

xtalk(l, r) = l + ctGain * ap1(hfRotFc, r),
              r + ctGain * ap1(hfRotFc, l);

makeup = ba.if(autoComp > 0.5, drv ^ (-0.85), 1);

// MIX BUS instances decode the DAW's linear sum back to signal domain first
pre(x)  = ba.if(isBus, decode(x), x);
// CHANNEL instances encode as the very last operation before the DAW sum
stageOut(x) = ba.if(isBus, x, encode(x));
post(s) = _ + hiss(s) : *(makeup * outGain) : fi.dcblocker : stageOut;

process = _,_ : pre, pre <: _,_, ((_ + _) * 0.5 : sagEnv) : procs
with {
    procs(l, r, e) = strip(0, e, l), strip(1, e, r) : xtalk : post(0), post(1);
};
