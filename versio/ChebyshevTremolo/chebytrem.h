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
#define PA(part)    (2.0f/(float)part)

class ChebyTrem {
  public:
    ChebyTrem() {}
    ~ChebyTrem() {}

    enum KIND {
        FIRST_KIND  = 0,
        SECOND_KIND = 1,
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
    static const int NUM_BANKS = 2;
    static const int MAX_PART = 14;

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
        {  14, PA(1), 0.f,   PA(3),  0.f,   PA(5), 0.f,   PA(7), 0.f,   PA(9),  0.f,    PA(11), 0.f,    PA(13), 0.f    },
        // even partials
        {  14, 0.f,   PA(2), 0.f,    PA(4), 0.f,   PA(6), 0.f,   PA(8), 0.f,    PA(10), 0.f,    PA(12), 0.f,    PA(15) },
    };

    void KN(float x, int n);
    void setLFOs(bool init=false);
};

} // namespace ds_versio

#endif
#endif
