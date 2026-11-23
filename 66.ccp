std::cout << "Channels: " << inBuffer.getNumChannels() 
          << ", Samples: " << inBuffer.getNumSamples() 
          << ", Sample Rate: " << sampleRate << std::endl;

for (int i = 0; i < 10; ++i) {
    std::cout << "Input sample " << i << ": " << inBuffer.getSample(0, i) << std::endl;
}