#include <JuceHeader.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

namespace {
constexpr float kTwoPi = 6.28318530717958647692f; 
constexpr float kDefaultSampleRate = 44100.0f;    
constexpr float kDefaultDepth = 0.5f;             
constexpr float kDefaultLfoFreq = 5.0f;          
constexpr float kDefaultAttackSec = 0.01f;        
constexpr float kDefaultDecaySec = 0.1f;          
constexpr float kMinDecaySec = 0.001f;            
constexpr float kDefaultGateThreshold = 0.02f;    
constexpr float kDefaultGateAttackSec = 0.002f;   
constexpr float kDefaultGateReleaseSec = 0.02f;   
} 


class AudioEffect {
protected:
    float sampleRate; 
public:
    AudioEffect() : sampleRate(kDefaultSampleRate) {}
    virtual ~AudioEffect() = default;

    void setSampleRate(float newSampleRate) {
        if (newSampleRate > 0.0f) {
            sampleRate = newSampleRate;
        }
    }

    float getSampleRate() const {
        return sampleRate;
    }

    
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;

    
    virtual juce::String getEffectName() const = 0;
};


class TremoloEffect : public AudioEffect {
private:
    float lfoFrequency; 
    float depth;        
    float lfoPhase;     

public:
    TremoloEffect(float freq = kDefaultLfoFreq, float dep = kDefaultDepth)
        : lfoFrequency(std::max(0.0f, freq)),
          depth(std::clamp(dep, 0.0f, 1.0f)),
          lfoPhase(0.0f) {}

    void setParameters(float freq, float dep) {
        lfoFrequency = std::max(0.0f, freq);
        depth = std::clamp(dep, 0.0f, 1.0f);
    }

    void process(juce::AudioBuffer<float>& buffer) override {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        if (numChannels == 0 || numSamples == 0 || sampleRate <= 0.0f) {
            return;
        }

        const float phaseIncrement = (kTwoPi * lfoFrequency) / sampleRate;
        std::vector<float> modGain(static_cast<size_t>(numSamples), 1.0f);

        for (int n = 0; n < numSamples; ++n) {
            
            const float unipolarSine = 0.5f * (std::sin(lfoPhase) + 1.0f);
            modGain[static_cast<size_t>(n)] = (1.0f - depth) + depth * unipolarSine;

            lfoPhase += phaseIncrement;
            if (lfoPhase >= kTwoPi) {
                lfoPhase = std::fmod(lfoPhase, kTwoPi);
            }
        }

        for (int ch = 0; ch < numChannels; ++ch) {
            float* channelData = buffer.getWritePointer(ch);
            for (int n = 0; n < numSamples; ++n) {
                channelData[n] *= modGain[static_cast<size_t>(n)];
            }
        }
    }

    juce::String getEffectName() const override {
        return "Tremolo";
    }
};


class EnvelopeEffect : public AudioEffect {
private:
    float attackTime; 
    float decayTime;  

public:
    EnvelopeEffect(float attack = kDefaultAttackSec, float decay = kDefaultDecaySec)
        : attackTime(std::max(0.0f, attack)),
          decayTime(std::max(kMinDecaySec, decay)) {}

    void setParameters(float attack, float decay) {
        attackTime = std::max(0.0f, attack);
        decayTime = std::max(kMinDecaySec, decay);
    }

    void process(juce::AudioBuffer<float>& buffer) override {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        if (numChannels == 0 || numSamples == 0 || sampleRate <= 0.0f) {
            return;
        }

        const int attackSamples = static_cast<int>(std::round(attackTime * sampleRate));
        const int decaySamples = std::max(1, static_cast<int>(std::round(decayTime * sampleRate)));
        const int totalActiveSamples = std::min(numSamples, attackSamples + decaySamples);

        std::vector<float> env(static_cast<size_t>(numSamples), 0.0f);

        for (int n = 0; n < numSamples; ++n) {
            if (n < attackSamples) {
                
                const int denom = std::max(1, attackSamples);
                env[static_cast<size_t>(n)] = static_cast<float>(n) / static_cast<float>(denom);
            } else if (n < totalActiveSamples) {
                
                const int decayIndex = n - attackSamples;
                const int denom = std::max(1, totalActiveSamples - attackSamples);
                const float progress = static_cast<float>(decayIndex) / static_cast<float>(denom);
                env[static_cast<size_t>(n)] = std::clamp(1.0f - progress, 0.0f, 1.0f);
            } else {
                env[static_cast<size_t>(n)] = 0.0f;
            }
        }

        for (int ch = 0; ch < numChannels; ++ch) {
            float* channelData = buffer.getWritePointer(ch);
            for (int n = 0; n < numSamples; ++n) {
                channelData[n] *= env[static_cast<size_t>(n)];
            }
        }
    }

    juce::String getEffectName() const override {
        return "AD Envelope";
    }
};


class NoiseGateEffect : public AudioEffect {
private:
    float threshold;   
    float attackTime;  
    float releaseTime; 
    float currentGain; 
    bool useSmoothing; 

    float computeCoeff(float timeSec) const {
        if (timeSec <= 0.0f || sampleRate <= 0.0f) {
            return 1.0f;
        }
        return 1.0f - std::exp(-1.0f / (timeSec * sampleRate));
    }

public:
    NoiseGateEffect(float thresh = kDefaultGateThreshold,
                    float attack = kDefaultGateAttackSec,
                    float release = kDefaultGateReleaseSec,
                    bool smooth = true)
        : threshold(std::max(0.0f, thresh)),
          attackTime(std::max(0.0f, attack)),
          releaseTime(std::max(0.0f, release)),
          currentGain(0.0f),
          useSmoothing(smooth) {}

    void setParameters(float thresh, float attack, float release, bool smooth = true) {
        threshold = std::max(0.0f, thresh);
        attackTime = std::max(0.0f, attack);
        releaseTime = std::max(0.0f, release);
        useSmoothing = smooth;
    }

    void process(juce::AudioBuffer<float>& buffer) override {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        if (numChannels == 0 || numSamples == 0) {
            return;
        }

        const float attackCoeff = computeCoeff(attackTime);
        const float releaseCoeff = computeCoeff(releaseTime);
        std::vector<float> gainCurve(static_cast<size_t>(numSamples), 0.0f);

        for (int n = 0; n < numSamples; ++n) {
            
            float maxAbsSample = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch) {
                const float sample = buffer.getSample(ch, n);
                maxAbsSample = std::max(maxAbsSample, std::abs(sample));
            }

            const float targetGain = (maxAbsSample > threshold) ? 1.0f : 0.0f;

            if (!useSmoothing) {
                currentGain = targetGain;
            } else {
                const float coeff = (targetGain > currentGain) ? attackCoeff : releaseCoeff;
                currentGain += (targetGain - currentGain) * coeff;
            }

            gainCurve[static_cast<size_t>(n)] = currentGain;
        }

        for (int ch = 0; ch < numChannels; ++ch) {
            float* channelData = buffer.getWritePointer(ch);
            for (int n = 0; n < numSamples; ++n) {
                channelData[n] *= gainCurve[static_cast<size_t>(n)];
            }
        }
    }

    juce::String getEffectName() const override {
        return "Noise Gate";
    }
};


class EffectChain {
private:
    juce::OwnedArray<AudioEffect> effects; 
    float chainSampleRate = kDefaultSampleRate;

public:
    void setSampleRate(float newSampleRate) {
        if (newSampleRate > 0.0f) {
            chainSampleRate = newSampleRate;
            for (auto* effect : effects) {
                if (effect != nullptr) {
                    effect->setSampleRate(chainSampleRate);
                }
            }
        }
    }

    void addEffect(AudioEffect* effect) {
        if (effect != nullptr) {
            effect->setSampleRate(chainSampleRate);
            effects.add(effect);
        }
    }

    void addEffect(std::unique_ptr<AudioEffect> effect) {
        if (effect != nullptr) {
            effect->setSampleRate(chainSampleRate);
            effects.add(effect.release());
        }
    }

    void removeEffect(int index) {
        effects.remove(index, true);
    }

    void clearChain() {
        effects.clear(true);
    }

    int getEffectCount() const {
        return effects.size();
    }

    AudioEffect* getEffect(int index) {
        return effects[index];
    }

    void process(juce::AudioBuffer<float>& buffer) {
        for (auto* effect : effects) {
            if (effect != nullptr) {
                effect->process(buffer);
            }
        }
    }

    void printChain() const {
        std::cout << "=== Effect Chain (" << effects.size() << " effects) ===\n";
        if (effects.isEmpty()) {
            std::cout << "  [Empty]\n";
            return;
        }
        for (int i = 0; i < effects.size(); ++i) {
            std::cout << "  " << (i + 1) << ". " << effects[i]->getEffectName() << "\n";
        }
    }
};


    juce::AudioBuffer<float> buffer;
    double sampleRate = static_cast<double>(kDefaultSampleRate);
    int bitsPerSample = 24;
    bool valid = false;
};

LoadedAudio loadAudioFile(const juce::File& file, juce::AudioFormatManager& formatManager) {
    LoadedAudio result;
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader == nullptr) {
        std::cerr << "Error: Could not open input audio file: " << file.getFullPathName() << "\n";
        return result;
    }

    const int numChannels = static_cast<int>(reader->numChannels);
    const int numSamples = static_cast<int>(reader->lengthInSamples);
    result.sampleRate = reader->sampleRate;
    result.bitsPerSample = static_cast<int>(reader->bitsPerSample);
    result.buffer.setSize(numChannels, numSamples);

    if (!reader->read(&result.buffer, 0, numSamples, 0, true, true)) {
        std::cerr << "Error: Failed reading samples from: " << file.getFullPathName() << "\n";
        return result;
    }

    result.valid = true;
    return result;
}

bool saveAudioFile(const juce::File& outputFile,
                   const juce::AudioBuffer<float>& buffer,
                   double sampleRate,
                   int bitsPerSample,
                   juce::AudioFormatManager& formatManager) {
    auto parentDir = outputFile.getParentDirectory();
    if (!parentDir.exists()) {
        parentDir.createDirectory();
    }

    if (outputFile.exists()) {
        outputFile.deleteFile();
    }

    juce::WavAudioFormat wavFormat;
    juce::ignoreUnused(formatManager);

    auto outStream = std::unique_ptr<juce::FileOutputStream>(outputFile.createOutputStream());
    if (outStream == nullptr || outStream->failedToOpen()) {
        std::cerr << "Error: Failed opening output stream: " << outputFile.getFullPathName() << "\n";
        return false;
    }

    const int writeBits = (bitsPerSample > 0) ? bitsPerSample : 24;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(outStream.get(),
                                  sampleRate,
                                  static_cast<unsigned int>(buffer.getNumChannels()),
                                  writeBits,
                                  {},
                                  0));

    if (writer == nullptr) {
        std::cerr << "Error: Failed creating WAV writer: " << outputFile.getFullPathName() << "\n";
        return false;
    }

    
    outStream.release();

    if (!writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples())) {
        std::cerr << "Error: Failed writing audio samples to: " << outputFile.getFullPathName() << "\n";
        return false;
    }

    return true;
}

void runTestCase(const juce::String& title,
                 const LoadedAudio& source,
                 EffectChain& chain,
                 const juce::File& outFile,
                 juce::AudioFormatManager& formatManager) {
    std::cout << "\n--- " << title << " ---\n";
    chain.setSampleRate(static_cast<float>(source.sampleRate));
    chain.printChain();

    juce::AudioBuffer<float> processedBuffer(source.buffer);
    std::cout << "Processing audio through effect chain...\n";
    chain.process(processedBuffer);

    if (saveAudioFile(outFile, processedBuffer, source.sampleRate, source.bitsPerSample, formatManager)) {
        std::cout << "Done! Processed file saved to: " << outFile.getFullPathName() << "\n";
    }
}


int main() {
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    const juce::File currentDir = juce::File::getCurrentWorkingDirectory();
    juce::File inputFile = currentDir.getChildFile("audio/input.wav");
    if (!inputFile.existsAsFile()) {
        inputFile = currentDir.getParentDirectory().getChildFile("audio/input.wav");
    }

    LoadedAudio loaded = loadAudioFile(inputFile, formatManager);
    if (!loaded.valid) {
        std::cerr << "Please place a valid WAV file at audio/input.wav before running.\n";
        return 1;
    }

    const juce::File outputDir = inputFile.getParentDirectory().getChildFile("output");

    
    {
        EffectChain singleChain;
        singleChain.addEffect(std::make_unique<TremoloEffect>(5.0f, 0.7f));
        runTestCase("Test 1: Single Effect (Tremolo 5.0 Hz, depth 0.7)",
                    loaded,
                    singleChain,
                    outputDir.getChildFile("test1_tremolo_only.wav"),
                    formatManager);
    }

    
    {
        EffectChain orderedChain;
        orderedChain.addEffect(std::make_unique<TremoloEffect>(5.0f, 0.6f));
        orderedChain.addEffect(std::make_unique<EnvelopeEffect>(0.02f, 0.25f));
        orderedChain.addEffect(std::make_unique<NoiseGateEffect>(0.025f, 0.002f, 0.02f, true));
        runTestCase("Test 2: Effect Chain (Tremolo -> AD Envelope -> Noise Gate)",
                    loaded,
                    orderedChain,
                    outputDir.getChildFile("test2_chain_ordered.wav"),
                    formatManager);
    }

    
    {
        EffectChain variedChain;
        variedChain.addEffect(std::make_unique<TremoloEffect>(9.0f, 0.9f));
        variedChain.addEffect(std::make_unique<EnvelopeEffect>(0.005f, 0.12f));
        variedChain.addEffect(std::make_unique<NoiseGateEffect>(0.05f, 0.001f, 0.015f, true));
        runTestCase("Test 3: Parameter Variations (Tremolo 9.0 Hz/0.9, faster AD, Gate 0.05)",
                    loaded,
                    variedChain,
                    outputDir.getChildFile("test3_param_variations.wav"),
                    formatManager);
    }

    std::cout << "\nAll test cases completed.\n";
    return 0;
}