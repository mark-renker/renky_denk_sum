# RenkySum

A per-channel **vintage console summing** plugin (Faust → AU/VST3) in the spirit of
Sonimus Satson, Waves NLS, and airwindows Console — but with an adjustable
harmonic-distortion amount, a **3D Depth** control, and per-instance
**component tolerances** so every track gets its own subtly different strip.

## The idea

Drop one instance on every track in Ableton, set each instance to a different
**Channel** number, and the plugin behaves like 32 adjacent strips of a real
desk: nominally identical, never actually identical. Gain trims are ±0.05 dB
off, HF poles are ±12 % apart, transformer asymmetry and crosstalk differ per
strip, and the noise floors are decorrelated. The spread is deterministic —
channel 7 always sounds like channel 7, sessions recall exactly.

## Controls

| Control | What it does |
|---|---|
| **Model** | CLASS-A XFMR (transformer, 2nd-harmonic-rich, LF core saturation) / VCA BUS (clean, mostly 3rd harmonic) / DISCRETE OP-AMP (punchy odd+even blend, strongest rail sag) |
| **Stage** | CHANNEL on tracks, MIX BUS on the master. Channel instances *encode* their output with `sin()`; the bus instance *decodes* the DAW's sum with `asin()` (airwindows-Console style). One channel alone nulls perfectly, but multiple channels interact nonlinearly at the sum — the resistive-network behaviour of a real desk that plain per-track saturation can't reproduce. |
| **Channel** | 1–32. The per-instance tolerance seed. Set differently per track. |
| **Drive** | Level pushed into the summing amp (±12 dB). Use Auto Gain to keep loudness constant while adding colour. |
| **Harmonics** | Intrinsic THD of the stage at nominal level — from near-clean to well past vintage spec. |
| **3D Depth** | See below. |
| **Output / Auto Gain / Noise Floor** | Trim, drive-compensation, and switchable console hiss. |

## What "3D Depth" actually is

The depth knob scales five mechanisms that research (null tests, airwindows
Console design notes, transformer measurements) points to as the sources of
the "dimensional" quality attributed to analog summing:

1. **Phase rotation** — transformers and coupling caps rotate phase at the
   spectrum edges. Depth moves the LF and HF all-pass corners *into* the
   audible band (LF up to ~110 Hz, HF down to ~8 kHz), dispersing transients
   the way iron and caps do.
2. **Dynamic (program-dependent) harmonics** — the even-harmonic bias
   breathes with the signal envelope, so the harmonic signature is never
   static. Static waveshaping is what most plugins do; movement is what
   analog does.
3. **Power-rail sag** — a shared (L/R common) envelope dips gain by up to
   ~0.3 dB on sustained loud material: micro-glue, front-to-back movement.
4. **Phase-blurred crosstalk** — inter-channel bleed around −52 dB whose
   path goes through the HF phase rotator, slightly decorrelating the image.
5. **Noise floor** — decorrelated hiss around −86 dBFS (switchable), which
   listeners reliably read as "air" and spatial continuity.

## Honest footnote on the research

Null and blind tests (Dan Worrall's "Summing Shmumming", Paul Frindle,
Sound on Sound) consistently show modern digital summing is transparent, and
much of the reported "depth" of hardware summing boxes traces to the extra
D/A→A/D pass and small loudness offsets (one famous SSL-vs-Neve shootout came
down to a 0.2 dB level difference). What *is* real and measurable is the
colour of driven console electronics: level-dependent harmonics, per-strip
tolerance spread, phase rotation, crosstalk and noise. This plugin models
those real mechanisms and lets you scale them from authentic (subtle) to
exaggerated — and Auto Gain keeps A/B comparisons loudness-honest.

## Measured behaviour (48 kHz, 1 kHz sine)

- Default settings, −12 dBFS: THD 0.59 % (H2 −50 dB, H3 −46 dB) — "driven
  vintage" territory; Harmonics = 0 measures ≈ −70 dB residuals.
- Encode→decode null: channel-stage sin() adds −39 dB H3, which the bus-stage
  asin() cancels to −80 dB. Interaction survives only *between* channels.
- Channel 1 vs channel 2 (same audio): −40 dB decorrelation. Channel 1 vs
  channel 1: bit-identical (deterministic recall).
- Rail sag at full Depth on hot material: ≈ 0.2–0.35 dB program-dependent dip.
- Noise floor at full Depth: −88 dBFS RMS, decorrelated per side/channel.

## Workflow in Ableton

1. AU: **RenkySum** on every track — Stage = CHANNEL, a different Channel
   number per track, Drive/Harmonics to taste.
2. One instance on the Master — Stage = **MIX BUS** (this decodes the sum;
   without it the sin() encode acts as a mild extra saturator, which is fine
   but skips the interaction trick).
3. A/B with Auto Gain on so loudness bias doesn't fool you.

## Build

Requires [Faust](https://faust.grame.fr) (`brew install faust`) and JUCE.

```sh
faust2juce -jucemodulesdir ~/JUCE/modules RenkySum.dsp
~/JUCE/Projucer.app/Contents/MacOS/Projucer --resave RenkySum/RenkySum.jucer
cd RenkySum/Builds/MacOSX
xcodebuild -project RenkySum.xcodeproj -alltargets -configuration Release build
```

JUCE installs the built AU/VST3 into `~/Library/Audio/Plug-Ins/` automatically;
in Live: Settings → Plug-Ins → Rescan. For instant browser prototyping, paste
`RenkySum.dsp` into the [Faust Web IDE](https://faustide.grame.fr).
