/* ------------------------------------------------------------
author: "Mark Renker"
license: "MIT"
name: "RenkySum"
version: "1.0"
Code generated with Faust 2.85.9 (https://faust.grame.fr)
Compilation options: -lang cpp -fpga-mem-th 4 -ct 1 -cn RenkySumDSP -scn FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __RenkySumDSP_H__
#define  __RenkySumDSP_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

/* link with : "" */
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS RenkySumDSP
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif


class RenkySumDSP : public FaustDsp {
	
 private:
	
	FAUSTFLOAT fEntry0;
	int fSampleRate;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fHslider0;
	float fConst2;
	float fRec1[2];
	FAUSTFLOAT fHslider1;
	float fRec2[2];
	float fConst3;
	FAUSTFLOAT fEntry1;
	float fVec0[2];
	FAUSTFLOAT fEntry2;
	float fConst4;
	float fRec6[2];
	float fVec1[2];
	float fRec5[2];
	float fRec7[2];
	FAUSTFLOAT fHslider2;
	float fRec8[2];
	float fRec9[2];
	float fRec10[2];
	FAUSTFLOAT fHslider3;
	float fRec11[2];
	float fConst5;
	float fConst6;
	float fRec12[2];
	float fRec13[2];
	float fVec2[2];
	float fRec14[2];
	float fRec4[2];
	float fRec15[2];
	float fRec3[2];
	float fRec16[2];
	float fVec3[2];
	float fVec4[2];
	float fRec20[2];
	float fRec21[2];
	float fVec5[2];
	float fRec19[2];
	float fRec18[2];
	float fVec6[2];
	float fRec17[2];
	float fConst7;
	float fConst8;
	int iRec23[2];
	float fVec7[2];
	float fConst9;
	float fRec22[2];
	FAUSTFLOAT fCheckbox1;
	float fRec24[2];
	float fVec8[2];
	float fRec0[2];
	float fRec26[2];
	int iRec28[2];
	float fVec9[2];
	float fRec27[2];
	float fVec10[2];
	float fRec25[2];
	
 public:
	RenkySumDSP() {
	}
	
	RenkySumDSP(const RenkySumDSP&) = default;
	
	virtual ~RenkySumDSP() = default;
	
	RenkySumDSP& operator=(const RenkySumDSP&) = default;
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/amp_follower_ar:author", "Jonatan Liljedahl, revised by Romain Michon");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "1.3.0");
		m->declare("author", "Mark Renker");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-lang cpp -fpga-mem-th 4 -ct 1 -cn RenkySumDSP -scn FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("description", "Vintage console summing-amp channel colour. Per-instance component tolerances, dynamic harmonics, and a 3D Depth control.");
		m->declare("filename", "RenkySum.dsp");
		m->declare("filters.lib/dcblocker:author", "Julius O. Smith III");
		m->declare("filters.lib/dcblocker:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/dcblocker:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("filters.lib/zero:author", "Julius O. Smith III");
		m->declare("filters.lib/zero:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/zero:license", "MIT-style STK-4.3 license");
		m->declare("license", "MIT");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("name", "RenkySum");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("signals.lib/name", "Faust Routing Library");
		m->declare("signals.lib/onePoleSwitching:author", "Jonatan Liljedahl, revised by Dario Sanfilippo");
		m->declare("signals.lib/onePoleSwitching:licence", "STK-4.3");
		m->declare("signals.lib/version", "1.6.0");
		m->declare("version", "1.0");
	}

	virtual int getNumInputs() {
		return 2;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = 44.1f / fConst0;
		fConst2 = 1.0f - fConst1;
		fConst3 = 3.1415927f / fConst0;
		fConst4 = 0.47f * fConst0;
		fConst5 = std::exp(-(2.857143f / fConst0));
		fConst6 = std::exp(-(125.0f / fConst0));
		fConst7 = 1.0f / std::tan(3.1415927f * (std::min<float>(9e+03f, fConst4) / fConst0));
		fConst8 = 1.0f / (fConst7 + 1.0f);
		fConst9 = 1.0f - fConst7;
	}
	
	virtual void instanceResetUserInterface() {
		fEntry0 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider1 = static_cast<FAUSTFLOAT>(0.0f);
		fEntry1 = static_cast<FAUSTFLOAT>(1.0f);
		fEntry2 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider2 = static_cast<FAUSTFLOAT>(0.35f);
		fHslider3 = static_cast<FAUSTFLOAT>(0.35f);
		fCheckbox1 = static_cast<FAUSTFLOAT>(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			fRec1[l0] = 0.0f;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			fRec2[l1] = 0.0f;
		}
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			fVec0[l2] = 0.0f;
		}
		for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
			fRec6[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
			fVec1[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
			fRec5[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			fRec7[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec8[l7] = 0.0f;
		}
		for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
			fRec9[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
			fRec10[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
			fRec11[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
			fRec12[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec13[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 2; l13 = l13 + 1) {
			fVec2[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
			fRec14[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 2; l15 = l15 + 1) {
			fRec4[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 2; l16 = l16 + 1) {
			fRec15[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 2; l17 = l17 + 1) {
			fRec3[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 2; l18 = l18 + 1) {
			fRec16[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 2; l19 = l19 + 1) {
			fVec3[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
			fVec4[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 2; l21 = l21 + 1) {
			fRec20[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 2; l22 = l22 + 1) {
			fRec21[l22] = 0.0f;
		}
		for (int l23 = 0; l23 < 2; l23 = l23 + 1) {
			fVec5[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 2; l24 = l24 + 1) {
			fRec19[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
			fRec18[l25] = 0.0f;
		}
		for (int l26 = 0; l26 < 2; l26 = l26 + 1) {
			fVec6[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 2; l27 = l27 + 1) {
			fRec17[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 2; l28 = l28 + 1) {
			iRec23[l28] = 0;
		}
		for (int l29 = 0; l29 < 2; l29 = l29 + 1) {
			fVec7[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 2; l30 = l30 + 1) {
			fRec22[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 2; l31 = l31 + 1) {
			fRec24[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 2; l32 = l32 + 1) {
			fVec8[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 2; l33 = l33 + 1) {
			fRec0[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 2; l34 = l34 + 1) {
			fRec26[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 2; l35 = l35 + 1) {
			iRec28[l35] = 0;
		}
		for (int l36 = 0; l36 < 2; l36 = l36 + 1) {
			fVec9[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 2; l37 = l37 + 1) {
			fRec27[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 2; l38 = l38 + 1) {
			fVec10[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 2; l39 = l39 + 1) {
			fRec25[l39] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual RenkySumDSP* clone() {
		return new RenkySumDSP(*this);
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("RenkySum");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("Console");
		ui_interface->declare(&fEntry2, "0", "");
		ui_interface->declare(&fEntry2, "style", "menu{'CLASS-A XFMR':0;'VCA BUS':1;'DISCRETE OP-AMP':2}");
		ui_interface->addNumEntry("Model", &fEntry2, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(2.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fEntry0, "1", "");
		ui_interface->declare(&fEntry0, "style", "menu{'CHANNEL':0;'MIX BUS':1}");
		ui_interface->addNumEntry("Stage", &fEntry0, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fEntry1, "2", "");
		ui_interface->declare(&fEntry1, "style", "knob");
		ui_interface->addNumEntry("Channel", &fEntry1, FAUSTFLOAT(1.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(32.0f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Colour");
		ui_interface->declare(&fHslider0, "0", "");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->declare(&fHslider0, "unit", "dB");
		ui_interface->addHorizontalSlider("Drive", &fHslider0, FAUSTFLOAT(0.0f), FAUSTFLOAT(-12.0f), FAUSTFLOAT(12.0f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fHslider2, "1", "");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Harmonics", &fHslider2, FAUSTFLOAT(0.35f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fHslider3, "2", "");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->addHorizontalSlider("3D Depth", &fHslider3, FAUSTFLOAT(0.35f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("Output");
		ui_interface->declare(&fHslider1, "0", "");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->declare(&fHslider1, "unit", "dB");
		ui_interface->addHorizontalSlider("Output", &fHslider1, FAUSTFLOAT(0.0f), FAUSTFLOAT(-12.0f), FAUSTFLOAT(12.0f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fCheckbox0, "1", "");
		ui_interface->addCheckButton("Auto Gain", &fCheckbox0);
		ui_interface->declare(&fCheckbox1, "2", "");
		ui_interface->addCheckButton("Noise Floor", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		int iSlow0 = static_cast<float>(fEntry0) > 0.5f;
		float fSlow1 = std::pow(1e+01f, 0.05f * static_cast<float>(fHslider0));
		float fSlow2 = fConst1 * ((static_cast<float>(fCheckbox0) > 0.5f) ? std::pow(fSlow1, -0.85f) : 1.0f);
		float fSlow3 = fConst1 * std::pow(1e+01f, 0.05f * static_cast<float>(fHslider1));
		float fSlow4 = static_cast<float>(fEntry1);
		int iSlow5 = static_cast<int>(fSlow4) + -1;
		int iSlow6 = 32 * iSlow5;
		float fSlow7 = static_cast<float>(fEntry2);
		int iSlow8 = fSlow7 == 0.0f;
		int iSlow9 = fSlow7 == 1.0f;
		float fSlow10 = static_cast<float>(((iSlow8) ? 24000 : ((iSlow9) ? 30000 : 27000)));
		float fSlow11 = 1.0f / std::tan(fConst3 * std::min<float>((0.12f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow6 + 2) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f) * fSlow10, fConst4));
		float fSlow12 = 1.0f - fSlow11;
		float fSlow13 = 0.006f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow6 + 1) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f;
		float fSlow14 = fConst1 * fSlow1;
		float fSlow15 = fSlow11 + 1.0f;
		float fSlow16 = 1.0f / std::tan(fConst3 * std::min<float>(1.4e+02f * (0.15f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow6 + 4) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f), fConst4));
		float fSlow17 = 1.0f / (fSlow16 + 1.0f);
		float fSlow18 = 1.0f - fSlow16;
		float fSlow19 = fConst1 * static_cast<float>(fHslider2);
		float fSlow20 = fConst1 * ((iSlow8) ? 1.0f : ((iSlow9) ? 1.35f : 1.2f));
		float fSlow21 = 0.25f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow6 + 3) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f;
		float fSlow22 = 0.1f * fSlow21;
		float fSlow23 = fConst1 * ((iSlow8) ? 0.9f : ((iSlow9) ? 0.22f : 0.55f));
		float fSlow24 = static_cast<float>(fHslider3);
		float fSlow25 = fConst1 * fSlow24;
		float fSlow26 = fConst1 * ((iSlow8) ? 1.0f : ((iSlow9) ? 0.15f : 0.5f));
		float fSlow27 = 0.15f * fSlow21;
		float fSlow28 = std::tan(fConst3 * std::min<float>((3.0f * fSlow24 + 1.0f) * ((iSlow8) ? 28.0f : ((iSlow9) ? 8.0f : 18.0f)), fConst4));
		float fSlow29 = fConst1 * ((fSlow28 + -1.0f) / (fSlow28 + 1.0f));
		float fSlow30 = std::tan(fConst3 * std::min<float>(2.6e+04f / (2.3f * fSlow24 + 1.0f), fConst4));
		float fSlow31 = fConst1 * ((fSlow30 + -1.0f) / (fSlow30 + 1.0f));
		float fSlow32 = fConst1 * ((iSlow8) ? 0.4f : ((iSlow9) ? 0.2f : 0.55f));
		float fSlow33 = 0.0025118864f * (0.3f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow6 + 5) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f);
		int iSlow34 = 16 * (2 * iSlow5 + 1);
		float fSlow35 = 1.0f / std::tan(fConst3 * std::min<float>((0.12f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow34 + 2) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f) * fSlow10, fConst4));
		float fSlow36 = 1.0f - fSlow35;
		float fSlow37 = 0.006f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow34 + 1) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f;
		float fSlow38 = fSlow35 + 1.0f;
		float fSlow39 = 1.0f / std::tan(fConst3 * std::min<float>(1.4e+02f * (0.15f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow34 + 4) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f), fConst4));
		float fSlow40 = 1.0f / (fSlow39 + 1.0f);
		float fSlow41 = 1.0f - fSlow39;
		float fSlow42 = 0.25f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow34 + 3) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f) + 1.0f;
		float fSlow43 = 0.1f * fSlow42;
		float fSlow44 = 0.15f * fSlow42;
		float fSlow45 = 7.943282e-05f * std::pow(1e+01f, 0.2f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow6 + 6) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f));
		int iSlow46 = 32768 * ((16807 * ((16807 * ((16807 * (iSlow6 + 7) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + (16807 * ((16807 * ((16807 * (iSlow6 + 8) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521;
		float fSlow47 = fConst1 * static_cast<float>(fCheckbox1);
		float fSlow48 = 7.943282e-05f * std::pow(1e+01f, 0.2f * (3.0525032e-05f * static_cast<float>((16807 * ((16807 * ((16807 * (iSlow34 + 6) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + -1.0f));
		int iSlow49 = 32768 * ((16807 * ((16807 * ((16807 * (iSlow34 + 7) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521) + (16807 * ((16807 * ((16807 * (iSlow34 + 8) + 12345) % 65521) + 12345) % 65521) + 12345) % 65521;
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			fRec1[0] = fSlow2 + fConst2 * fRec1[1];
			fRec2[0] = fSlow3 + fConst2 * fRec2[1];
			float fTemp0 = fRec1[0] * fRec2[0];
			fVec0[0] = fSlow4;
			fRec6[0] = fSlow14 + fConst2 * fRec6[1];
			float fTemp1 = static_cast<float>(input0[i0]);
			float fTemp2 = std::fabs(fTemp1);
			float fTemp3 = ((iSlow0) ? std::asin(std::max<float>(-0.9999f, std::min<float>(0.9999f, ((fTemp2 < 0.9f) ? fTemp1 : static_cast<float>((fTemp1 > 0.0f) - (fTemp1 < 0.0f)) * (0.1f * tanhf(1e+01f * (fTemp2 + -0.9f)) + 0.9f))))) : fTemp1);
			float fTemp4 = fSlow13 * fRec6[0] * fTemp3;
			fVec1[0] = fTemp4;
			fRec5[0] = -((fRec5[1] * fSlow12 - (fTemp4 + fVec1[1])) / fSlow15);
			fRec7[0] = -(fSlow17 * (fSlow18 * fRec7[1] - (fRec5[0] + fRec5[1])));
			fRec8[0] = fSlow19 + fConst2 * fRec8[1];
			fRec9[0] = fSlow20 + fConst2 * fRec9[1];
			float fTemp5 = 1.85f * fRec8[0] * fRec9[0] + 0.05f;
			float fTemp6 = std::max<float>(0.05f, fTemp5);
			fRec10[0] = fSlow23 + fConst2 * fRec10[1];
			fRec11[0] = fSlow25 + fConst2 * fRec11[1];
			float fTemp7 = static_cast<float>(input1[i0]);
			float fTemp8 = std::fabs(fTemp7);
			float fTemp9 = ((iSlow0) ? std::asin(std::max<float>(-0.9999f, std::min<float>(0.9999f, ((fTemp8 < 0.9f) ? fTemp7 : static_cast<float>((fTemp7 > 0.0f) - (fTemp7 < 0.0f)) * (0.1f * tanhf(1e+01f * (fTemp8 + -0.9f)) + 0.9f))))) : fTemp7);
			float fTemp10 = std::fabs(0.5f * fRec6[0] * (fTemp3 + fTemp9));
			float fTemp11 = ((fTemp10 > fRec12[1]) ? fConst6 : fConst5);
			fRec12[0] = fTemp10 * (1.0f - fTemp11) + fRec12[1] * fTemp11;
			float fTemp12 = fRec11[0] * std::min<float>(fRec12[0], 1.5f);
			float fTemp13 = fRec8[0] * fRec10[0] * (0.6f * fTemp12 + 1.0f);
			float fTemp14 = fSlow22 * fTemp13;
			fRec13[0] = fSlow26 + fConst2 * fRec13[1];
			float fTemp15 = std::max<float>(0.05f, fTemp5 * (1.2f * fRec8[0] * fRec13[0] + 1.0f));
			float fTemp16 = fSlow27 * fTemp13;
			float fTemp17 = (tanhf((fRec5[0] - fRec7[0]) * fTemp6 + fTemp14) - tanhf(fTemp14)) / fTemp6 + (tanhf(fRec7[0] * fTemp15 + fTemp16) - tanhf(fTemp16)) / fTemp15;
			fVec2[0] = fTemp17;
			fRec14[0] = fSlow29 + fConst2 * fRec14[1];
			fRec4[0] = fVec2[1] - fRec14[0] * (fRec4[1] - fTemp17);
			fRec15[0] = fSlow31 + fConst2 * fRec15[1];
			fRec3[0] = fRec4[1] - fRec15[0] * (fRec3[1] - fRec4[0]);
			fRec16[0] = fSlow32 + fConst2 * fRec16[1];
			float fTemp18 = 1.0f - 0.12f * fTemp12 * fRec16[0];
			float fTemp19 = fRec3[0] * fTemp18;
			fVec3[0] = fTemp19;
			float fTemp20 = fSlow37 * fRec6[0] * fTemp9;
			fVec4[0] = fTemp20;
			fRec20[0] = -((fRec20[1] * fSlow36 - (fTemp20 + fVec4[1])) / fSlow38);
			fRec21[0] = -(fSlow40 * (fSlow41 * fRec21[1] - (fRec20[0] + fRec20[1])));
			float fTemp21 = fSlow43 * fTemp13;
			float fTemp22 = fSlow44 * fTemp13;
			float fTemp23 = (tanhf((fRec20[0] - fRec21[0]) * fTemp6 + fTemp21) - tanhf(fTemp21)) / fTemp6 + (tanhf(fRec21[0] * fTemp15 + fTemp22) - tanhf(fTemp22)) / fTemp15;
			fVec5[0] = fTemp23;
			fRec19[0] = fVec5[1] - fRec14[0] * (fRec19[1] - fTemp23);
			fRec18[0] = fRec19[1] - fRec15[0] * (fRec18[1] - fRec19[0]);
			float fTemp24 = fRec18[0] * fTemp18;
			fVec6[0] = fTemp24;
			fRec17[0] = fVec6[1] - fRec15[0] * (fRec17[1] - fTemp24);
			float fTemp25 = fRec11[0] + 0.25f;
			int iTemp26 = fSlow4 != fVec0[1];
			iRec23[0] = ((iTemp26) ? iSlow46 : 1103515245 * iRec23[1] + 12345);
			float fTemp27 = static_cast<float>(iRec23[0]);
			fVec7[0] = fTemp27;
			fRec22[0] = fConst8 * (4.656613e-10f * (fTemp27 + fVec7[1]) - fConst9 * fRec22[1]);
			fRec24[0] = fSlow47 + fConst2 * fRec24[1];
			float fTemp28 = 0.7f * fRec11[0] + 0.3f;
			float fTemp29 = fTemp0 * (fTemp19 + fSlow33 * fRec17[0] * fTemp25 + fSlow45 * fRec22[0] * fRec24[0] * fTemp28);
			fVec8[0] = fTemp29;
			fRec0[0] = 0.995f * fRec0[1] + fTemp29 - fVec8[1];
			output0[i0] = static_cast<FAUSTFLOAT>(((iSlow0) ? fRec0[0] : std::sin(std::max<float>(-1.5707964f, std::min<float>(1.5707964f, fRec0[0])))));
			fRec26[0] = fVec3[1] - fRec15[0] * (fRec26[1] - fTemp19);
			iRec28[0] = ((iTemp26) ? iSlow49 : 1103515245 * iRec28[1] + 12345);
			float fTemp30 = static_cast<float>(iRec28[0]);
			fVec9[0] = fTemp30;
			fRec27[0] = fConst8 * (4.656613e-10f * (fTemp30 + fVec9[1]) - fConst9 * fRec27[1]);
			float fTemp31 = fTemp0 * (fTemp24 + fSlow33 * fRec26[0] * fTemp25 + fSlow48 * fRec24[0] * fRec27[0] * fTemp28);
			fVec10[0] = fTemp31;
			fRec25[0] = 0.995f * fRec25[1] + fTemp31 - fVec10[1];
			output1[i0] = static_cast<FAUSTFLOAT>(((iSlow0) ? fRec25[0] : std::sin(std::max<float>(-1.5707964f, std::min<float>(1.5707964f, fRec25[0])))));
			fRec1[1] = fRec1[0];
			fRec2[1] = fRec2[0];
			fVec0[1] = fVec0[0];
			fRec6[1] = fRec6[0];
			fVec1[1] = fVec1[0];
			fRec5[1] = fRec5[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			fVec2[1] = fVec2[0];
			fRec14[1] = fRec14[0];
			fRec4[1] = fRec4[0];
			fRec15[1] = fRec15[0];
			fRec3[1] = fRec3[0];
			fRec16[1] = fRec16[0];
			fVec3[1] = fVec3[0];
			fVec4[1] = fVec4[0];
			fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			fVec5[1] = fVec5[0];
			fRec19[1] = fRec19[0];
			fRec18[1] = fRec18[0];
			fVec6[1] = fVec6[0];
			fRec17[1] = fRec17[0];
			iRec23[1] = iRec23[0];
			fVec7[1] = fVec7[0];
			fRec22[1] = fRec22[0];
			fRec24[1] = fRec24[0];
			fVec8[1] = fVec8[0];
			fRec0[1] = fRec0[0];
			fRec26[1] = fRec26[0];
			iRec28[1] = iRec28[0];
			fVec9[1] = fVec9[0];
			fRec27[1] = fRec27[0];
			fVec10[1] = fVec10[0];
			fRec25[1] = fRec25[0];
		}
	}

};

#endif
