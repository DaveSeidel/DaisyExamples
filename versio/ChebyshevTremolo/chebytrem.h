#pragma once
#ifndef CHEBY_TREM_H
#define CHEBY_TREM_H
#ifdef __cplusplus

#include "daisysp.h"

using namespace daisysp;

namespace ds_versio {

// based on LFO sample
struct LFO {
    Oscillator osc;
    float      amp;
    float      freq;
    int        waveform;
    bool       unipolar;

    void Init(float samplerate, float _amp, float _freq, float _phase = 0.f) {
        osc.Init(samplerate);

        amp = abs(_amp);
        osc.SetAmp(amp);

        freq = _freq;
        osc.SetFreq(freq);

        waveform = Oscillator::WAVE_TRI;
        osc.SetWaveform(waveform);

        if (_phase != 0.f) {
            osc.PhaseAdd(_phase);
        }
    }

    void SetAmp(float _amp) {
        amp = abs(_amp);
        osc.SetAmp(amp);
    }

    void SetFreq(float _freq) {
        freq = _freq;
        osc.SetFreq(freq);
    }

    void SetUnipolar(bool _unipolar) {
        unipolar = _unipolar;
    }

    float Process(void) {
        if (unipolar) {
            return (osc.Process() + amp) * 0.5f;
        }
        return osc.Process();
    }
};

// amplitude/weight for a coeeficient based on which partial it represents
#define PA(part)    (part != 0 ? (2.0f/(float)part) : 0.f)

class ChebyTrem {
  public:
    ChebyTrem() {}
    ~ChebyTrem() {}

    enum KIND {
        FIRST_KIND  = 1,
        SECOND_KIND = 0,
        THIRD_KIND  = 2
    };

    enum RATE_TYPE {
        PROPORTIONAL = 0,
        SAME         = 1,
        NONE         = 2
    };

    void Init(float sr, int kind, int bank, bool uni, int rateType);
    float Process(float in);
    void Reset(bool init=false);

    inline static int numBanks() { return NUM_BANKS; }

    void setKind(int kind);
    void setBank(int bank);
    void setUni(bool uni);
    void setSpeed(float speed);
    void setRateType(int rateType);

  private:
    static const int NUM_BANKS = 4;
    static const int MAX_PART = 15;

    float sr_;
    
    int kind_ = SECOND_KIND;
    int last_bank_ = -1, bank_ = 0;
    bool last_uni_ = false, uni_ = false;
    float last_speed_ = -1.0f, speed_ = 0.5f;
    int last_rate_type_ = -1, rate_type_ = PROPORTIONAL;

    int plen_ = 0;
    int dirty_ = 0;

    LFO lfos_[MAX_PART];

    float kn_data_[MAX_PART];

    const float banks_[NUM_BANKS][MAX_PART+1] = {
        // odd partials
        {  MAX_PART, PA(1), PA(0), PA(3), PA(0), PA(5), PA(0), PA(7), PA(0), PA(9), PA(0),  PA(11), PA(0),  PA(13), PA(0),  PA(15) },
        // even partials
        {  MAX_PART, PA(0), PA(2), PA(0), PA(4), PA(0), PA(6), PA(0), PA(8), PA(0), PA(10), PA(0),  PA(12), PA(0),  PA(14), PA(0)  },

        // prime partials
        {  MAX_PART, PA(1), PA(2), PA(3), PA(0), PA(5), PA(0), PA(7), PA(0), PA(0), PA(0),  PA(11), PA(0),  PA(13), PA(0),  PA(0)  },
        // non-prime partials
        {  MAX_PART, PA(0), PA(0), PA(0), PA(4), PA(0), PA(6), PA(0), PA(8), PA(9), PA(10), PA(0),  PA(12), PA(0),  PA(14), PA(15) },
    };

    void KN(float x, int n);
    void setLFOs(bool init=false);
};

} // namespace ds_versio

#endif
#endif