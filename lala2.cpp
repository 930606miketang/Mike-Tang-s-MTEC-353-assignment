#include <juce_audio_basics/juce_audio_basics.juce_header>
#include <juce_core/juce_core.juce_header>
#include <juce_audio_formats/juce_audio_formats.juce_header>


void generateWhiteNoise() {
    double sampleRate = 44100.0;
    int numSeconds = 1;
    int numSamples = static_cast<int>(sampleRate * numSeconds);

    juce::AudioBuffer<float> buffer(1, numSamples);
    float* channelData = buffer.getWritePointer(0);

    juce::Random rng; 

    for (int i = 0; i < numSamples; ++i) {
        
        channelData[i] = rng.nextFloat() * 2.0f - 1.0f;
    }

    
    juce::File file("white_noise.wav");
    file.deleteFile();
    juce::WavAudioFormat wavFormat;
    
    
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(new juce::FileOutputStream(file), sampleRate, 1, 16, {}, 0)
    );

    if (writer != nullptr) {
        writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }
}