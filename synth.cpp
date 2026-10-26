#include <AudioToolbox/AudioToolbox.h>

#include <atomic>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <termios.h>
#include <thread>
#include <unistd.h>

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kSampleRate = 44100.0;

struct SynthState {
    std::atomic<int> note{-1};
    std::atomic<bool> running{true};
    double phase = 0.0;
};

double noteFrequency(int midiNote) {
    return 440.0 * std::pow(2.0, (midiNote - 69) / 12.0);
}

OSStatus renderAudio(
    void* userData,
    AudioUnitRenderActionFlags*,
    const AudioTimeStamp*,
    UInt32,
    UInt32 frameCount,
    AudioBufferList* buffers) {
    auto* state = static_cast<SynthState*>(userData);
    float* output = static_cast<float*>(buffers->mBuffers[0].mData);
    const int currentNote = state->note.load();

    for (UInt32 frame = 0; frame < frameCount; ++frame) {
        float sample = 0.0f;
        if (currentNote >= 0) {
            const double frequency = noteFrequency(currentNote);
            sample = static_cast<float>(0.2 * std::sin(state->phase));
            state->phase += 2.0 * kPi * frequency / kSampleRate;
            if (state->phase >= 2.0 * kPi) {
                state->phase -= 2.0 * kPi;
            }
        } else {
            state->phase = 0.0;
        }

        for (UInt32 channel = 0; channel < buffers->mNumberBuffers; ++channel) {
            static_cast<float*>(buffers->mBuffers[channel].mData)[frame] = sample;
        }
    }

    return noErr;
}

bool configureAudioUnit(AudioUnit* audioUnit, SynthState* state) {
    AudioComponentDescription description{
        kAudioUnitType_Output,
        kAudioUnitSubType_DefaultOutput,
        kAudioUnitManufacturer_Apple,
        0,
        0};

    AudioComponent component = AudioComponentFindNext(nullptr, &description);
    if (component == nullptr ||
        AudioComponentInstanceNew(component, audioUnit) != noErr) {
        return false;
    }

    AURenderCallbackStruct callback{renderAudio, state};
    if (AudioUnitSetProperty(
            *audioUnit,
            kAudioUnitProperty_SetRenderCallback,
            kAudioUnitScope_Input,
            0,
            &callback,
            sizeof(callback)) != noErr) {
        return false;
    }

    AudioStreamBasicDescription format{
        kSampleRate,
        kAudioFormatLinearPCM,
        kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved,
        sizeof(float),
        1,
        2,
        32,
        0};

    if (AudioUnitSetProperty(
            *audioUnit,
            kAudioUnitProperty_StreamFormat,
            kAudioUnitScope_Input,
            0,
            &format,
            sizeof(format)) != noErr) {
        return false;
    }

    return AudioUnitInitialize(*audioUnit) == noErr &&
           AudioOutputUnitStart(*audioUnit) == noErr;
}

class TerminalMode {
public:
    TerminalMode() {
        tcgetattr(STDIN_FILENO, &original_);
        termios raw = original_;
        raw.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    ~TerminalMode() {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_);
    }

private:
    termios original_{};
};

int noteForKey(char key) {
    constexpr char keys[] = "awsedftgyhujkolp;'";
    for (int index = 0; keys[index] != '\0'; ++index) {
        if (keys[index] == key) {
            return 60 + index;
        }
    }
    return -1;
}
}  // namespace

int main() {
    SynthState state;
    AudioUnit audioUnit = nullptr;

    if (!configureAudioUnit(&audioUnit, &state)) {
        std::cerr << "Unable to start macOS audio output.\n";
        if (audioUnit != nullptr) {
            AudioComponentInstanceDispose(audioUnit);
        }
        return EXIT_FAILURE;
    }

    std::cout << "Simple Synthesizer\n"
              << "Keys A-W play notes. Press Q to quit.\n";

    TerminalMode terminalMode;
    while (state.running.load()) {
        char key = '\0';
        if (read(STDIN_FILENO, &key, 1) != 1) {
            continue;
        }

        if (key == 'q' || key == 'Q') {
            state.running.store(false);
        } else if (const int note = noteForKey(key); note >= 0) {
            state.note.store(note);
            std::cout << "Playing MIDI note " << note << '\n';
        } else if (key == ' ') {
            state.note.store(-1);
        }
    }

    AudioOutputUnitStop(audioUnit);
    AudioUnitUninitialize(audioUnit);
    AudioComponentInstanceDispose(audioUnit);
    return EXIT_SUCCESS;
}
