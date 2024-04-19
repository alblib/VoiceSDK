#include <VoiceSDK/ringbuffer.hpp>
#include <stdexcept>

void test_RingBuffer() {
    VoiceSDK::RingBuffer<float, 128> rb;
    VoiceSDK::RingBuffer<float, 64> rb2(rb);
    VoiceSDK::RingBuffer<float, 256> rb3(rb);

    std::vector<float> a = { 1,2,3 };
    rb.enqueue(a.begin(), a.end());
    rb2 = static_cast<decltype(rb2)>(rb);
    rb3 = static_cast<decltype(rb3)>(rb);

    auto vec2 = rb2.dequeue();
    auto vec3 = rb3.dequeue();

    if (vec2 != a)
        throw std::exception("VoiceSDK::RingBuffer test failed.");
    if (vec3 != a)
        throw std::exception("VoiceSDK::RingBuffer test failed.");
}

int main(){
    test_RingBuffer();
    return 0;
}