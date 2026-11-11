#pragma once
// Minimal Faust runtime interfaces (clean-room, MIT).
//
// The Faust compiler emits a class that derives from a base DSP class and
// talks to UI / Meta visitors. We generate with `-scn FaustDsp` and provide
// these three interfaces ourselves so the build has no dependency on the
// Faust architecture headers. Only the methods the generated code calls are
// declared here. See scripts/regen_dsp.sh.

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct Soundfile;

struct Meta
{
    virtual ~Meta() = default;
    virtual void declare (const char* key, const char* value) = 0;
};

class UI
{
public:
    virtual ~UI() = default;

    // layout
    virtual void openTabBox (const char* label) = 0;
    virtual void openHorizontalBox (const char* label) = 0;
    virtual void openVerticalBox (const char* label) = 0;
    virtual void closeBox() = 0;

    // active widgets
    virtual void addButton (const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addCheckButton (const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addVerticalSlider (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addHorizontalSlider (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addNumEntry (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

    // passive widgets
    virtual void addHorizontalBargraph (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
    virtual void addVerticalBargraph (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

    virtual void addSoundfile (const char* label, const char* filename, Soundfile** sfZone) = 0;

    virtual void declare (FAUSTFLOAT* zone, const char* key, const char* value) = 0;
};

class FaustDsp
{
public:
    FaustDsp() = default;
    FaustDsp (const FaustDsp&) = default;
    FaustDsp& operator= (const FaustDsp&) = default;
    virtual ~FaustDsp() = default;

    virtual int getNumInputs() = 0;
    virtual int getNumOutputs() = 0;
    virtual void buildUserInterface (UI* ui) = 0;
    virtual int getSampleRate() = 0;
    virtual void init (int sampleRate) = 0;
    virtual void instanceInit (int sampleRate) = 0;
    virtual void instanceConstants (int sampleRate) = 0;
    virtual void instanceResetUserInterface() = 0;
    virtual void instanceClear() = 0;
    virtual FaustDsp* clone() = 0;
    virtual void metadata (Meta* m) = 0;
    virtual void compute (int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
};
