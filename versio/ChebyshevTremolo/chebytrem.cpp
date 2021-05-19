#include "ChebyTrem.h"

using namespace ds_versio;

// public

void ChebyTrem::Init(float sr, int kind, int bank, bool uni, int rateType) {
    sr_ = sr;
    setKind(kind);
    setBank(bank);
    setUni(uni);
    setRateType(rateType);
    Reset(true);
}

float ChebyTrem::Process(float input) {
    float output = 0.f;
    for (int i = 0; i < plen_; i++) {
        KN(input, i);
    }

    for (int j = 1; j <= plen_; j++) {
        float f = banks_[bank_][j];
        if (rate_type_ != NONE) {
            if (f != 0.f) {
                f = lfos_[j-1].Process();
            }
            else {
                continue;
            }
        }
        output += f * kn_data_[j];
    }
    return output;
}

void ChebyTrem::Reset(bool init) {
    if (init == true) {
        setLFOs(init);
    }
    else if (dirty_ > 0) {
        setLFOs();
        dirty_ = 0;
    }
}

void ChebyTrem::setKind(int kind) {
    kind_ = kind;
}

void ChebyTrem::setBank(int bank) {
    if (bank != last_bank_) {
        last_bank_ = bank_;
        bank_ = bank;
        plen_ = int(banks_[bank_][0]);
        dirty_ += 1;
    }
}

void ChebyTrem::setUni(bool uni) {
    if (uni != last_uni_) {
        last_uni_ = uni_;
        uni_ = uni;
        dirty_ += 1;
    }
}

void ChebyTrem::setRateType(int rate_type) {
    if (rate_type != last_rate_type_) {
        last_rate_type_ = rate_type_;
        rate_type_ = rate_type;
        dirty_ += 1;
    }
}

void ChebyTrem::setSpeed(float speed) {
    if (speed != last_speed_) {
        last_speed_ = speed_;
        speed_ = speed;
        dirty_ += 1;
    }
}

// private

// set up an LFO for each non-zero coefficient
void ChebyTrem::setLFOs(bool init) {
    for (int j = 0; j < plen_; j++) {
        LFO *lfo = &lfos_[j];
        float _j = (float)(j+1);
        float _freq; 
        float p = banks_[bank_][j+1];

        if (p == 0.f) {
            _freq = 0.f;
        }
        else {
            switch (rate_type_) {
                case PROPORTIONAL:
                default:
                    // each partial has different LFO speed
                    _freq = (speed_ * _j) * 0.9f;
                    break;
                case SAME:
                    // all partials have same LFO speed
                    _freq = speed_;
                    break;
                case NONE:
                    // no LFOs
                    _freq = 0.f;
            }
        }
        
        if (_freq > 0.f) { 
            if (init) {
                lfo->Init(sr_, p, _freq);
                lfo->SetUnipolar(uni_);
            }
            else {
                lfo->SetFreq(_freq);
                lfo->SetAmp(p);
                lfo->SetUnipolar(uni_);
            }
        }
        else {
            if (init) {
                lfo->Init(sr_, 0.f, 0.f);
            }
            else {
                lfo->SetFreq(0.f);
                lfo->SetAmp(0.f);
            }
        }
    }
}

// Chebyshev recurrence relation, using a buffer to avoid recursion
// handles first three kinds
void ChebyTrem::KN(float x, int n) {
    if (n == 0) {
        kn_data_[n] = 1.0f;
    }
    else if (n == 1) {
        switch (kind_) {
            case FIRST_KIND:
            default:
                kn_data_[n] = x;
                break;
            case SECOND_KIND:
                kn_data_[n] = 2*x;
                break;
            case THIRD_KIND:
                kn_data_[n] = 2*x - 1;
                break;
        }
    }
    else {
        kn_data_[n] = (2*x) * kn_data_[n-1] - kn_data_[n-2];
    }
}

