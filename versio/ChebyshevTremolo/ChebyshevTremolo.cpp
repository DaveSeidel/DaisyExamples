#include "time.h"
#include "daisy_versio.h"
#include "daisysp.h"
#include "chebytrem.h"

using namespace daisy;
using namespace daisysp;
using namespace ds_versio;

DaisyVersio hw;
Switch3 bank_sw, kind_sw;
DcBlock block_l, block_r;
Tone lp_l, lp_r;
ChebyTrem ct_l, ct_r;

Parameter mix_param;
Parameter speed_l_param, speed_r_param;
Parameter lp_param;
Parameter tanh_k1_param, tanh_k2_param;

float sr;
float mix, last_mix = -1.0f;

// round to first two decimal places
float round2(float var) {
    float value = (int)(var * 100 + 0.5f);
    return (float)value / 100;
}

// borrowed from moogladder
float fast_tanh(float x)
{
    int sign = 1;
    if(x < 0) {
        sign = -1;
        x    = -x;
        return x * sign;
    }
    else if(x >= 4.0f) {
        return sign;
    }
    else if(x < 0.5f) {
        return x * sign;
    }
    return sign * tanhf(x);
}

// https://github.com/johannesmenzel/SRPlugins/wiki/DSP-ALGORITHMS-Saturation
float saturate(float x, float k1, float k2) {
    float out;
    if (x >= 0) {
        out = fast_tanh(k1 * x) / fast_tanh(k1);
    }
    else {
        out = fast_tanh(k2 * x) / fast_tanh(k2);
    }
    return out;
}

void setKind() {
    int kind = kind_sw.Read();
    ct_l.setKind(kind);
    ct_r.setKind(kind);
}

void setBanks() {
    int banks = bank_sw.Read();
    switch (banks) {
        case 0:
        default:
            // odd vs. even
            ct_l.setBank(0);
            ct_r.setBank(1);
            break;
        case 1:
            // prime vs. non-prime
            ct_l.setBank(2);
            ct_r.setBank(3);
            break;
        case 2:
            // both prime
            ct_l.setBank(2);
            ct_r.setBank(2);
            break;
    }
}

void setSpeed() {
    float speed_l = speed_l_param.Process();
    float speed_r = speed_r_param.Process();
    ct_l.setSpeed(speed_l);
    ct_r.setSpeed(speed_r);
}

void setMix() {
    float _mix = round2(mix_param.Process());
    if (_mix != last_mix) {
        last_mix = mix;
        mix = _mix;
    }
}

void resetChebyVib(bool init=false) {
    ct_l.Reset(init);
    ct_r.Reset(init);
}

void callback(float *in, float *out, size_t size) {
    setKind();
    setBanks();
    setSpeed();
    resetChebyVib();
    setMix();

    float lpf = lp_param.Process();
    lp_l.SetFreq(lpf);
    lp_r.SetFreq(lpf);

    float k1 = tanh_k1_param.Process();
    float k2 = tanh_k2_param.Process();
    
    for (int i = 0; i < int(size); i += 2) {
        float left  = in[i];
        float right = in[i+1];

        float left_  = lp_l.Process(left);
        float right_ = lp_r.Process(right);

        left_  = saturate(left_, k1, k2);
        right_ = saturate(right_, k1, k2);
 
        left_  = ct_l.Process(left_);
        right_ = ct_r.Process(right_);

        left_  = block_l.Process(left_);
        right_ = block_r.Process(right_);

        left_  = (mix * left_)  + ((1.0f - mix) * left);
        right_ = (mix * right_) + ((1.0f - mix) * right);

        out[i]   = left_;
        out[1+i] = right_;
    }
}

int main(void) {
    hw.Init();
    sr = hw.AudioSampleRate();

    // overall speed of lef/right channel LFOs
    speed_l_param.Init(hw.knobs[DaisyVersio::KNOB_0], 0.01f, 0.8f, Parameter::LINEAR);
    speed_r_param.Init(hw.knobs[DaisyVersio::KNOB_1], 0.01f, 0.8f, Parameter::LINEAR);

    // dry/wet mix
    mix_param.Init(hw.knobs[DaisyVersio::KNOB_2], 0.f, 1.0f, Parameter::LINEAR);
    
    // LPF cutoff frequency
    lp_param.Init(hw.knobs[DaisyVersio::KNOB_3], 60, 4000, Parameter::LINEAR);

    // separate saturation factors for positive and negative input samples    
    tanh_k1_param.Init(hw.knobs[DaisyVersio::KNOB_4], 0.f, 2.0, Parameter::LINEAR);
    tanh_k2_param.Init(hw.knobs[DaisyVersio::KNOB_5], 0.f, 2.0, Parameter::LINEAR);

    // Chebyshev kind (first/second/third) 
    kind_sw = hw.sw[DaisyVersio::SW_0];

    // LFO rate type
    bank_sw = hw.sw[DaisyVersio::SW_1];

    // Chebyshev tremolo instances, one for each channel
    ct_l.Init(sr, 2, 2, true, ChebyTrem::PROPORTIONAL);
    ct_r.Init(sr, 2, 3, true, ChebyTrem::PROPORTIONAL);

    // LPF instances
    lp_l.Init(sr);
    lp_r.Init(sr);

    // DcBlock instances 
    block_l.Init(sr);
    block_r.Init(sr);

    hw.StartAudio(callback);
    hw.StartAdc();

    while(1) {
        hw.ProcessAnalogControls();
        hw.UpdateExample();
        hw.UpdateLeds();
    }
}
