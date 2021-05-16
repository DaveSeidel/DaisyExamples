# ChebyshevTremolo

## Author

Dave Seidel

## Description

NOTE: This is alpha-quality firmware at best! I am open to suggestions for improvements and/or modifications. I don't necessarily see this as worthy of a complete version firmware, but it might5 be usable as an effect as a larger design.

Stereo tremolo effect based on the Chebyshev polynomial. Left channel uses coefficient based on the first six odd-numbered partials, right channel does the same but with even-numbered partials. In both cases, the coefficient are modulated by LFOs whose frequency is proportional to the partial number, meaning that each of the twelve LFOs (six per channel) are operating at a different speed, and the left channels has different modulators than the right channel.

The processing chain for each channel is: input-> LPF -> tanh saturation -> modulated Chebyshev transform -> dcblock -> output.

Controls:
* Knob 0: overall tremolo speed for left channel
* Knob 1: overall tremolo speed for right channel
* Knob 2: dry/wet balance
* Knob 3: LPF cutoff
* Knob 4: tanh factor for positive values
* Knob 5: tanh factor for negative values
* Knob 6: unused
* Sw 1: LFO unipolar vs. bipolar operation, where center is unipolar and the other two positions (identical) are bipolar
* Sw 2: LFO frequency control, where center means frequency is proportional to partial number, left means all LFO on each side are the same speed, right means LFOs are not used
* Button: not used

The LEDS just use the demo behavior, nothing specific to the effect yet.

