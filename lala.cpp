#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>

#include <cmath>
#include <iostream>
#include <memory>

void generateSquareWave()
{
    constexpr double sampleRate = 44100.0;
    constexpr double frequency = 220.0;
    constexpr int durationSeconds = 1;

    const int numSamples =
        static_cast<int>(sampleRate * durationSeconds);

    juce::AudioBuffer<float> buffer(1, numSamples);
    float* channelData = buffer.getWritePointer(0);

    for (int i = 0; i < numSamples; ++i)
    {
        const double time = static_cast<double>(i) / sampleRate;
        double sample = 0.0;

        // 使用奇數諧波合成方波
        for (int harmonic = 1; harmonic <= 15; harmonic += 2)
        {
            sample += (1.0 / harmonic) *
                      std::sin(2.0 * juce::MathConstants<double>::pi *
                               harmonic * frequency * time);
        }

        sample *= (4.0 / juce::MathConstants<double>::pi) * 0.5;
        channelData[i] = static_cast<float>(sample);
    }

    juce::File file =
        juce::File::getCurrentWorkingDirectory()
            .getChildFile("square_wave_220hz.wav");

    file.deleteFile();

    juce::WavAudioFormat wavFormat;
    auto outputStream = file.createOutputStream();

    if (outputStream == nullptr)
    {
        std::cerr << "無法建立音訊檔案\n";
        return;
    }

    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(
            outputStream.release(),
            sampleRate,
            1,
            24,
            {},
            0));

    if (writer != nullptr &&
        writer->writeFromAudioSampleBuffer(
            buffer, 0, buffer.getNumSamples()))
    {
        std::cout << "已建立 square_wave_220hz.wav\n";
    }
    else
    {
        std::cerr << "寫入 WAV 檔案失敗\n";
    }
}

int main()
{
    generateSquareWave();
    return 0;
}