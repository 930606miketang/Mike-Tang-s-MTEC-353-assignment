#include <iostream>

class MIDIMessage
{
protected:
    unsigned char channel;
    unsigned long timestamp; // Simple timing

public:
    MIDIMessage(unsigned char ch) : channel(ch), timestamp(0) {}

    void send()
    {
        std::cout << "Sending MIDI on channel " << int(channel) << std::endl;
    }

    unsigned char getChannel() { return channel; }
};

int main()
{
    MIDIMessage msg(1);
    msg.send();
    return 0;
}