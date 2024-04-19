#include <VoiceSDK/ringbuffer.hpp>
#include <stdexcept>
#include <iostream>
#include <exception>

int test_RingBuffer() {
    VoiceSDK::RingBuffer<float, 128> rb;
    VoiceSDK::RingBuffer<float, 64> rb2(rb);
    VoiceSDK::RingBuffer<float, 256> rb3(rb);

    std::vector<float> a = { 1,2,3 };
    rb.enqueue(a.begin(), a.end());
    rb2 = static_cast<decltype(rb2)>(rb);
    rb3 = static_cast<decltype(rb3)>(rb);

    auto vec2 = rb2.dequeue();
    auto vec3 = rb3.dequeue();

    if (vec2 != a) {
        std::cerr << "VoiceSDK::RingBuffer content copy test failed.";
        return -1;
    }
    if (vec3 != a) {
        std::cerr << "VoiceSDK::RingBuffer content copy test failed.";
        return -1;
    }
    return 0;
}

int main(){
    if (test_RingBuffer() != 0) return -1;
    return 0;
}