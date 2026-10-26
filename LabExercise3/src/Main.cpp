/**
 * MTEC-353: Lab Exercise 3 - AI-Assisted Polymorphic MIDI Event System with JUCE
 * 
 * Design Overview:
 * 1. Polymorphism (Runtime Dispatch): Used for the MidiEvent hierarchy so heterogeneous
 *    MIDI messages (NoteOn, NoteOff, CC, ProgramChange) can be stored in a single collection
 *    and processed uniformly via base class pointers and virtual function tables (vtables).
 * 2. Templates (Compile-time Dispatch): Used for TimedEventQueue and frequency calculation
 *    to achieve zero-overhead type-generic containers and mathematical operations.
 */

#include <juce_core/juce_core.h>
#include <iostream>
#include <memory>
#include <cmath>
#include <algorithm>

// ==============================================================================
// Helper Function: Convert MIDI Note Number to Human-Readable Pitch Name (e.g., 60 -> C4)
// ==============================================================================
juce::String midiNoteToString(int noteNumber) {
    const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int octave = (noteNumber / 12) - 1;
    int noteIndex = noteNumber % 12;
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

// ==============================================================================
// Part B: Polymorphic MIDI Event Hierarchy
// ==============================================================================

/**
 * Abstract Base Class representing a generic MIDI Event.
 * Contains shared attributes (timestamp, channel) and defines a strict interface
 * via pure virtual functions for derived classes to implement.
 */
class MidiEvent {
protected:
    int timestamp;
    int channel;

public:
    MidiEvent(int time, int ch) : timestamp(time), channel(ch) {}
    
    // CRITICAL: Virtual destructor ensures that deleting a derived object through
    // a MidiEvent* base pointer correctly invokes the derived class destructor,
    // preventing memory leaks and undefined behavior.
    virtual ~MidiEvent() = default;

    // Pure virtual functions enforcing the polymorphic contract
    virtual juce::String toString() const = 0;
    virtual int getDataSize() const = 0;
    virtual void execute() const = 0;

    int getTimestamp() const { return timestamp; }
    int getChannel() const { return channel; }
};

/**
 * Concrete Derived Class: NoteOnEvent
 * Represents a MIDI Note-On message containing pitch and velocity (2 data bytes).
 */
class NoteOnEvent : public MidiEvent {
private:
    int note;
    int velocity;

public:
    NoteOnEvent(int time, int ch, int n, int vel)
        : MidiEvent(time, ch), note(n), velocity(vel) {}

    juce::String toString() const override {
        return "NoteOn: " + midiNoteToString(note) + 
               " vel:" + juce::String(velocity) + 
               " ch:" + juce::String(channel);
    }

    int getDataSize() const override { return 2; } // Note + Velocity bytes

    void execute() const override {
        std::cout << "[" << timestamp << "ms] " << toString() 
                  << " | Action: Playing note " << note << " with velocity " << velocity << std::endl;
    }

    int getNote() const { return note; }
    int getVelocity() const { return velocity; }
};

/**
 * Concrete Derived Class: NoteOffEvent
 * Represents a MIDI Note-Off message releasing an active pitch (2 data bytes).
 */
class NoteOffEvent : public MidiEvent {
private:
    int note;
    int velocity;

public:
    NoteOffEvent(int time, int ch, int n, int vel = 0)
        : MidiEvent(time, ch), note(n), velocity(vel) {}

    juce::String toString() const override {
        return "NoteOff: " + midiNoteToString(note) + 
               " vel:" + juce::String(velocity) + 
               " ch:" + juce::String(channel);
    }

    int getDataSize() const override { return 2; }

    void execute() const override {
        std::cout << "[" << timestamp << "ms] " << toString() 
                  << " | Action: Releasing note " << note << " with release velocity " << velocity << std::endl;
    }
};

/**
 * Concrete Derived Class: ControlChangeEvent
 * Represents a MIDI CC message for continuous parameter modulation (2 data bytes).
 */
class ControlChangeEvent : public MidiEvent {
private:
    int controllerNumber;
    int controllerValue;

public:
    ControlChangeEvent(int time, int ch, int cc, int val)
        : MidiEvent(time, ch), controllerNumber(cc), controllerValue(val) {}

    juce::String toString() const override {
        return "ControlChange: cc:" + juce::String(controllerNumber) + 
               " val:" + juce::String(controllerValue) + 
               " ch:" + juce::String(channel);
    }

    int getDataSize() const override { return 2; }

    void execute() const override {
        std::cout << "[" << timestamp << "ms] " << toString() 
                  << " | Action: Setting CC#" << controllerNumber << " to value " << controllerValue << std::endl;
    }
};

/**
 * Concrete Derived Class: ProgramChangeEvent
 * Represents a MIDI patch/preset selection message (1 data byte).
 */
class ProgramChangeEvent : public MidiEvent {
private:
    int programNumber;

public:
    ProgramChangeEvent(int time, int ch, int prog)
        : MidiEvent(time, ch), programNumber(prog) {}

    juce::String toString() const override {
        return "ProgramChange: prog:" + juce::String(programNumber) + 
               " ch:" + juce::String(channel);
    }

    int getDataSize() const override { return 1; } // Only 1 data byte in MIDI spec

    void execute() const override {
        std::cout << "[" << timestamp << "ms] " << toString() 
                  << " | Action: Switching patch to Program #" << programNumber << std::endl;
    }
};


// ==============================================================================
// Part C: Template-Based Event Container & Generic Utilities
// ==============================================================================

/**
 * Additional Template Work 1: Generic MIDI Note-to-Frequency Conversion
 * Uses compile-time polymorphism to calculate frequency in float, double, or long double.
 * Formula: f = 440 * 2^((n - 69) / 12)
 */
template <typename FloatType>
FloatType midiNoteToFrequency(int midiNote) {
    return static_cast<FloatType>(440.0 * std::pow(2.0, (midiNote - 69.0) / 12.0));
}

/**
 * Additional Template Work 2: Template Specialization for Formatting Timed Data
 * Provides type-specific diagnostic formatting without runtime overhead.
 */
template <typename T>
struct EventFormatter {
    static juce::String format(int timestamp, const T& data) {
        return "[" + juce::String(timestamp) + "ms] Generic Value: " + juce::String(data);
    }
};

// Explicit template specialization for std::shared_ptr<MidiEvent>
template <>
struct EventFormatter<std::shared_ptr<MidiEvent>> {
    static juce::String format(int timestamp, const std::shared_ptr<MidiEvent>& event) {
        if (event != nullptr)
            return "[" + juce::String(timestamp) + "ms] " + event->toString();
        return "[" + juce::String(timestamp) + "ms] Null Event";
    }
};

// Explicit template specialization for juce::String
template <>
struct EventFormatter<juce::String> {
    static juce::String format(int timestamp, const juce::String& text) {
        return "[" + juce::String(timestamp) + "ms] System Log: \"" + text + "\"";
    }
};

/**
 * Generic TimedEventQueue Container
 * Wraps juce::Array to store, sort, retrieve, and filter time-stamped events of any type T.
 */
template <typename T>
class TimedEventQueue {
private:
    struct TimedItem {
        int timestamp;
        T data;

        // Operators required by juce::Array::sort()
        bool operator<(const TimedItem& other) const { return timestamp < other.timestamp; }
        bool operator>(const TimedItem& other) const { return timestamp > other.timestamp; }
        bool operator==(const TimedItem& other) const { return timestamp == other.timestamp; }
    };

    juce::Array<TimedItem> events;

public:
    void addEvent(int time, T event) {
        events.add({ time, event });
    }

    T getNextEvent() {
        if (!events.isEmpty()) {
            // removeAndReturn(0) pops the front element using juce::Array's API
            TimedItem first = events.removeAndReturn(0);
            return first.data;
        }
        return T{}; // Return default-constructed T if empty
    }

    void sortByTime() {
        // Utilize std::sort on juce::Array's iterators for chronological ordering
        std::sort(events.begin(), events.end(), [](const TimedItem& a, const TimedItem& b) {
            return a.timestamp < b.timestamp;
        });
    }

    // Template member function accepting any callable Predicate (lambda, functor, function ptr)
    template <typename Predicate>
    juce::Array<T> filterEvents(Predicate pred) const {
        juce::Array<T> filteredResults;
        for (const auto& item : events) {
            if (pred(item.data)) {
                filteredResults.add(item.data);
            }
        }
        return filteredResults;
    }

    int size() const { return events.size(); }
    bool isEmpty() const { return events.isEmpty(); }
};


// ==============================================================================
// Main Application Entry Point
// ==============================================================================
int main() {
    std::cout << "=== MIDI Event System ===" << std::endl;

    // 1. Create a polymorphic collection of 5 mixed MIDI events
    juce::Array<std::shared_ptr<MidiEvent>> polyEvents;
    polyEvents.add(std::make_shared<NoteOnEvent>(0, 1, 60, 64));          // C4
    polyEvents.add(std::make_shared<NoteOnEvent>(100, 1, 64, 80));        // E4
    polyEvents.add(std::make_shared<ControlChangeEvent>(200, 1, 7, 100)); // Volume CC7
    polyEvents.add(std::make_shared<NoteOffEvent>(300, 1, 60, 0));        // C4 Off
    polyEvents.add(std::make_shared<ProgramChangeEvent>(400, 1, 5));      // Patch 5 (Electric Piano)

    std::cout << "Created " << polyEvents.size() << " polymorphic events\n" << std::endl;

    // 2. Process polymorphic sequence matching required submission output format
    std::cout << "Processing event sequence:" << std::endl;
    for (const auto& ev : polyEvents) {
        std::cout << "[" << ev->getTimestamp() << "ms] " << ev->toString() << std::endl;
    }

    std::cout << "\nExecuting polymorphic behaviors (execute() & getDataSize()):" << std::endl;
    for (const auto& ev : polyEvents) {
        ev->execute();
    }

    // 3. Demonstrate Template Queue with 3 distinct data types
    TimedEventQueue<std::shared_ptr<MidiEvent>> midiQueue;
    TimedEventQueue<juce::String> messageQueue;
    TimedEventQueue<float> automationQueue;

    // Populate midiQueue out of chronological order to test sorting
    midiQueue.addEvent(300, polyEvents[3]);
    midiQueue.addEvent(0,   polyEvents[0]);
    midiQueue.addEvent(200, polyEvents[2]);
    midiQueue.addEvent(100, polyEvents[1]);
    midiQueue.addEvent(400, polyEvents[4]);
    midiQueue.sortByTime();

    // Populate messageQueue out of order
    messageQueue.addEvent(150, "Audio Engine Initialized");
    messageQueue.addEvent(10,  "Loading Synth Presets");
    messageQueue.addEvent(500, "Buffer Underrun Check Passed");
    messageQueue.sortByTime();

    // Populate automationQueue (e.g., Filter Cutoff normalized 0.0f - 1.0f)
    automationQueue.addEvent(250, 0.85f);
    automationQueue.addEvent(50,  0.20f);
    automationQueue.addEvent(120, 0.55f);
    automationQueue.sortByTime();

    std::cout << "\nTemplate queue test: Sorted 3 queues of different types" << std::endl;

    // 4. Test filterEvents() template member function using a C++ lambda predicate
    auto channelOneNotes = midiQueue.filterEvents([](const std::shared_ptr<MidiEvent>& e) {
        // Filter only events with 2 data bytes that occur at or before 200ms
        return e->getDataSize() == 2 && e->getTimestamp() <= 200;
    });
    std::cout << "Filtered MIDI queue: Found " << channelOneNotes.size() 
              << " two-byte events occurring <= 200ms" << std::endl;

    // 5. Test Template Specialization & Frequency Conversion Template Function
    double a4FreqDouble = midiNoteToFrequency<double>(69);
    float c4FreqFloat   = midiNoteToFrequency<float>(60);
    std::cout << "Template Frequency Calculation -> A4 (Note 69): " << a4FreqDouble 
              << " Hz | C4 (Note 60): " << c4FreqFloat << " Hz" << std::endl;

    auto nextMsg = messageQueue.getNextEvent();
    std::cout << "Template Specialization Output -> " 
              << EventFormatter<juce::String>::format(10, nextMsg) << std::endl;

    return 0;
}
