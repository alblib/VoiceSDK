#include <VoiceSDK/ringbuffer.hpp>

int main(){
    VoiceSDK::RingBuffer<float, 128> rb;
    VoiceSDK::RingBuffer<float, 64> rb2(rb);

    return 0;
}