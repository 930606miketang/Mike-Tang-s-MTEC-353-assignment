# Learning Journal

## 1. Framework Overwhelm

Encountering a massive professional framework like JUCE alongside CMake initially felt overwhelming due to the steep learning curve of modern C++ build tooling. Pushing through required starting with the minimal `juce_core` module and a simple console application structure. Clearer architectural diagrams highlighting dependency flows would have significantly eased this initial friction.

## 2. Abstraction Mechanism Insights

Implementing the `MidiEvent` inheritance hierarchy provided a clear "aha!" moment when observing runtime polymorphism dispatching unique behaviors through a uniform interface pointer. Compared to Python's dynamic duck typing, C++ enforces strict type safety and high performance at compile time. While templates eliminate virtual table indirection overhead, balancing these design choices is essential for efficient audio software architecture.

## 3. Professional Development Reality Check

The complexity of this lab closely mirrors real-world audio plugin development environments rather than academic toy problems. Leveraging a robust framework prevents reinventing the wheel, and utilizing AI as a technical sounding board proved invaluable in navigating unfamiliar professional build systems and debugging compiler diagnostics.

## 4. MIDI System Design Reflection

Encapsulating MIDI events into dedicated classes fundamentally transformed my mental model of musical data streams from raw byte arrays into expressive, stateful domain objects. This object-oriented design paradigm naturally extends to real-time audio processing graphs, DSP node chains, and parameter automation pipelines.
