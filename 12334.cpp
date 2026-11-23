#define JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED 1
#include <juce_audio_basics/juce_audio_basics.h>

#include <algorithm>
#include <iostream>

namespace
{
constexpr float gain = 1.2f;

void printInputSamples(const juce::AudioBuffer<float>& inputBuffer)
{
    const int samplesToPrint = std::min(10, inputBuffer.getNumSamples());

    for (int sampleIndex = 0; sampleIndex < samplesToPrint; ++sampleIndex)
    {
        std::cout << "Input sample " << sampleIndex << ": "
                  << inputBuffer.getSample(0, sampleIndex) << '\n';
    }
}

void printOutputSamples(const juce::AudioBuffer<float>& outputBuffer)
{
    const int samplesToPrint = std::min(10, outputBuffer.getNumSamples());

    for (int sampleIndex = 0; sampleIndex < samplesToPrint; ++sampleIndex)
    {
        std::cout << "Output sample " << sampleIndex << ": "
                  << outputBuffer.getSample(0, sampleIndex) << '\n';
    }
}

bool processAudio(const juce::AudioBuffer<float>& inputBuffer,
                  juce::AudioBuffer<float>& outputBuffer)
{
    const int channelCount =
        std::min(inputBuffer.getNumChannels(), outputBuffer.getNumChannels());
    const int sampleCount =
        std::min(inputBuffer.getNumSamples(), outputBuffer.getNumSamples());
    bool clippingOccurred = false;

    for (int channel = 0; channel < channelCount; ++channel)
    {
        const float* inputData = inputBuffer.getReadPointer(channel);
        float* outputData = outputBuffer.getWritePointer(channel);

        for (int sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex)
        {
            const float unprocessedSample = inputData[sampleIndex] * gain;
            const float processedSample =
                std::clamp(unprocessedSample, -1.0f, 1.0f);

            clippingOccurred |= unprocessedSample != processedSample;
            outputData[sampleIndex] = processedSample;
        }
    }

    return clippingOccurred;
}
} // namespace

void processAudioFile(const juce::AudioBuffer<float>& inputBuffer,
                      juce::AudioBuffer<float>& outputBuffer,
                      double sampleRate)
{
    if (inputBuffer.getNumChannels() == 0 ||
        inputBuffer.getNumSamples() == 0 ||
        outputBuffer.getNumChannels() == 0 ||
        outputBuffer.getNumSamples() == 0)
    {
        std::cerr << "Error: input and output buffers must not be empty.\n";
        return;
    }

    std::cout << "Channels: " << inputBuffer.getNumChannels()
              << ", Samples: " << inputBuffer.getNumSamples()
              << ", Sample Rate: " << sampleRate << '\n';

    printInputSamples(inputBuffer);

    // Direct-copy experiment:
    outputBuffer.makeCopyOf(inputBuffer, true);

    // Gain experiment with clipping protection:
    const bool clippingOccurred = processAudio(inputBuffer, outputBuffer);

    printOutputSamples(outputBuffer);

    if (clippingOccurred)
    {
        std::cout << "Warning: Clipping detected in the audio file!\n";
    }
}
