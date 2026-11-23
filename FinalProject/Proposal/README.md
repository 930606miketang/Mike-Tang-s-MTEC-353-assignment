# Simple Reverb

## 1. Project Overview
* **Title:** Simple Reverb
* **Summary:** A basic real-time stereo algorithmic reverb plugin built with C++ and JUCE.
* **Detailed Description:** Simple Reverb processes stereo audio in real time using delay lines, feedback damping, and dry/wet mixing as a DAW effect plugin with GUI sliders and automation. Audio runs on the real-time audio thread using pre-allocated buffers and parameter smoothing to prevent clicks or feedback runaway. Keeping the design focused makes it realistic to finish in three weeks.
* **Innovation/Interest:** A clean, minimal reverb plugin with optional tail freeze/hold for quick spatial depth without overcomplicating the DSP.

## 2. Technical Specification
* **Core Technologies:** `juce_audio_processors`, `juce_dsp`, `juce_gui_basics`; C++ classes, smart pointers, `AudioProcessorValueTreeState`; delay/filter reverb DSP; no external libraries.
* **Architecture Overview:** `PluginProcessor` + `ReverbDSP` (`prepare`, `processBlock`, `reset`) + `PluginEditor`; audio thread avoids allocations/locks, GUI connects via APVTS; flow is `Audio In` -> `Reverb + Damping` -> `Mix` -> `Audio Out`.
* **Performance Requirements:** 44.1 kHz / 48 kHz, buffer sizes 64–1024, low latency, low CPU load.

## 3. Scope Management
* **GOOD:** Stereo reverb with decay, damping, and mix controls + basic GUI.
* **BETTER:** Smoothed parameters (`SmoothedValue`), pre-delay, freeze toggle, state save/recall.
* **BEST:** Simple presets and basic visual feedback.

## 4. Implementation Plan
* **Week 1 (Nov 20-27):** Project setup, APVTS parameters, basic working reverb audio.
* **Week 2 (Nov 27-Dec 4):** GUI binding, parameter smoothing, stability/sound testing.
* **Week 3 (Dec 4-11):** Freeze/BETTER/BEST polish, buffer/CPU tests, README and demo.

## 5. AI Collaboration Strategy
* **Development Acceleration:** Use AI for JUCE boilerplate and debugging; compile, listen, and log key prompts/decisions.
* **Learning Enhancement:** Write/edit core DSP myself, test edge cases, and ask AI to explain unclear code rather than blind copying.

## 6. Risk Assessment
* **Technical Challenges:** Feedback buildup, metallic ringing, or parameter clicks; fallback is safe feedback limits and a simple delay/filter structure.
* **Knowledge Gaps:** JUCE parameter smoothing and audio-thread safety; use JUCE docs and instructor/office hours help.