# Spectral Modulator (ChebyshevTremolo)

## Author

Dave Seidel

## Description

NOTE: This is alpha-quality firmware at best! I am open to suggestions for improvements and/or modifications. I don't necessarily see this as worthy of a complete Versio firmware, but it might be useful as an effect within a larger multi-effect design.

Stereo distortion/tremolo effect based on the Chebyshev polynomial. I call it a "spectral modulator" because the modulation is in the spectral realm rather than based on amplitude.

There are three configurations that govern the modulation: in one configuration, the left channel uses coefficients based on the first seven odd-numbered partials and the right channel does the same but with the first seven even-numbered partials. There are two other configurations: prime vs. non-prime; and both prime. The coefficients are modulated by LFOs whose frequency is proportional to the partial number, meaning that each of the fourteen LFOs (seven per channel) are operating at a different speed, and the left channel has different modulators than the right channel. Each channel has a separate overall rate control as well. The partial/LFO configurations tend to make the two channels move differently, and this effect is enhanced by using slightly different rates per channel. 

The processing chain for each channel is: input-> LPF -> tanh saturation -> modulated Chebyshev transform -> dcblock -> output.

Controls:
* Knob 0: overall tremolo speed for left channel
* Knob 1: overall tremolo speed for right channel
* Knob 2: dry/wet balance
* Knob 3: LPF cutoff
* Knob 4: tanh factor for positive values
* Knob 5: tanh factor for negative values
* Knob 6: (unused)
* Sw 1: kind of Chebyshev polynomial:
  * left = first kind
  * center = second kind
  * right = third kind
* Sw 2: spectral profile:
  * left = left channel prime partials, right channel non-prime partials
  * center = left channel odd partials, right channel even partials
  * right = both channels prime partials 
* Button: (unused)

The LEDS just use the demo behavior, nothing specific to the effect yet.
