# Learning Journal — Lab Exercise 4

## 1. DSP Mathematical Intuition
Mapping a bipolar sine wave into `[0, 1]` and blending it with `(1 - depth)` was the moment the tremolo formula clicked for me because I could see directly how `depth` controls the modulation range. I noticed how closely mathematical precision ties to perceptual quality: even a tiny discontinuity in gain shows up immediately as a click or harsh edge when listening. Writing the DSP code also showed me that a formula that looks clean on paper can behave unexpectedly on real audio signals with dynamics and quiet tails. Implementing the noise gate with one-pole smoothing was the trickiest part because balancing `attackTime`, `releaseTime`, and `threshold` requires tuning both the math and the ear together.

## 2. Effect Chain Insights
Changing the order and parameters of effects in a chain noticeably alters how dynamics and modulation interact, especially when shaping amplitude before or after gating. Hearing the chain process an entire WAV file cleanly for the first time made the modular `EffectChain` design feel much more concrete than isolated unit formulas. Pushing the tremolo rate and depth higher alongside faster envelope and gate settings produced much more rhythmic, gated movement than subtle settings.

## 3. AI as DSP Learning Partner
Working with AI helped me separate signal flow (the audio samples moving through `process()`) from control/parameter flow (rates, depths, thresholds, and sample rate state). It also made time-to-sample conversions and one-pole smoothing equations easier to follow before I wrote the C++ code. At the same time, listening tests were essential because a mathematically valid formula can still sound unnatural or over-gated on a specific audio source.

## 4. Professional Audio Architecture
Using a common `AudioEffect` interface with a sequential chain mirrors how modular effect racks and DAW insert chains route audio through multiple processors. Turning this command-line processor into a VST plugin would require wiring host parameter automation, sample-accurate parameter smoothing, `prepareToPlay`/`releaseResources` lifecycle hooks, and real-time audio thread safety.