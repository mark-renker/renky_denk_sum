#pragma once
#include "FaustDsp.h"
#include <map>
#include <string>
#include <vector>

// Walks a Faust DSP's user interface and records a pointer to every control
// zone, keyed by its full path ("/RenkySum/Colour/Drive"). The C++ side binds
// parameters to zones by path, so DSP and plugin stay decoupled: adding a
// control to the .dsp and a row to Parameters.cpp is all it takes.
class FaustZoneMap : public UI
{
public:
    struct Control
    {
        std::string path;
        FAUSTFLOAT* zone = nullptr;
        FAUSTFLOAT init = 0, min = 0, max = 0, step = 0;
    };

    explicit FaustZoneMap (FaustDsp& dsp) { dsp.buildUserInterface (this); }

    FAUSTFLOAT* zoneFor (const std::string& path) const
    {
        auto it = byPath.find (path);
        return it == byPath.end() ? nullptr : it->second.zone;
    }

    const std::vector<Control>& controls() const { return ordered; }

    // UI
    void openTabBox (const char* label) override        { push (label); }
    void openHorizontalBox (const char* label) override { push (label); }
    void openVerticalBox (const char* label) override   { push (label); }
    void closeBox() override                            { if (! stack.empty()) stack.pop_back(); }

    void addButton (const char* label, FAUSTFLOAT* zone) override                       { add (label, zone, 0, 0, 1, 1); }
    void addCheckButton (const char* label, FAUSTFLOAT* zone) override                  { add (label, zone, 0, 0, 1, 1); }
    void addVerticalSlider (const char* l, FAUSTFLOAT* z, FAUSTFLOAT i, FAUSTFLOAT mn, FAUSTFLOAT mx, FAUSTFLOAT s) override   { add (l, z, i, mn, mx, s); }
    void addHorizontalSlider (const char* l, FAUSTFLOAT* z, FAUSTFLOAT i, FAUSTFLOAT mn, FAUSTFLOAT mx, FAUSTFLOAT s) override { add (l, z, i, mn, mx, s); }
    void addNumEntry (const char* l, FAUSTFLOAT* z, FAUSTFLOAT i, FAUSTFLOAT mn, FAUSTFLOAT mx, FAUSTFLOAT s) override         { add (l, z, i, mn, mx, s); }

    void addHorizontalBargraph (const char* l, FAUSTFLOAT* z, FAUSTFLOAT mn, FAUSTFLOAT mx) override { add (l, z, 0, mn, mx, 0); }
    void addVerticalBargraph (const char* l, FAUSTFLOAT* z, FAUSTFLOAT mn, FAUSTFLOAT mx) override   { add (l, z, 0, mn, mx, 0); }
    void addSoundfile (const char*, const char*, Soundfile**) override {}
    void declare (FAUSTFLOAT*, const char*, const char*) override {}

private:
    void push (const char* label) { stack.emplace_back (label); }

    void add (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    {
        std::string path;
        for (auto& s : stack) path += "/" + s;
        path += "/" + std::string (label);

        Control c { path, zone, init, min, max, step };
        byPath[path] = c;
        ordered.push_back (c);
    }

    std::vector<std::string> stack;
    std::map<std::string, Control> byPath;
    std::vector<Control> ordered;
};
