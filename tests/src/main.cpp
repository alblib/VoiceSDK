#include <VoiceSDK/ringbuffer.hpp>
#include <VoiceSDK/polynomial.hpp>
#include <VoiceSDK/type_traits.hpp>
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

    rb2 = static_cast<decltype(rb2)>(rb);
    for (size_t i = 0; i < 3; ++i)
        a[i] *= 2;
    rb2.amplify(2);
    vec2 = rb2.dequeue();
    if (vec2 != a) {
        std::cerr << "VoiceSDK::RingBuffer content amplify test failed.";
        return -1;
    }

    return 0;
}


template<class It>
//std::enable_if_t<VoiceSDK::is_input_iterator_v<It> && VoiceSDK::is_iterator_of_v<It, float>, bool>
VoiceSDK::enable_if_input_iterator_of_ignore_cv_t<It, float, bool>
test(It a)
{
    return true;
}

int test_type_traits() {
    std::vector<float> a;

    std::cout << VoiceSDK::is_input_iterator_v<decltype(a.cbegin())>;
    
    std::cout << VoiceSDK::is_iterator_of_ignore_cv_v<decltype(a.cbegin()), float>;

    std::cout << VoiceSDK::is_input_iterator_v<float*>;
    std::cout << VoiceSDK::is_iterator_of_v<float*, float>;

    // input iterator test
    if (!std::is_same_v<VoiceSDK::enable_if_input_iterator<std::vector<float>::iterator>::type, void>)
    {
        std::cerr << "VoiceSDK::enable_if_input_iterator does not capture iterator.";
        return -2;
    }

    if (!VoiceSDK::is_input_iterator_v<std::vector<float>::iterator>)
    {
        std::cerr << "VoiceSDK::is_input_iterator gives wrong value.";
        return -2;
    }
    if (VoiceSDK::is_input_iterator_v<int>)
    {
        std::cerr << "VoiceSDK::is_input_iterator gives wrong value.";
        return -2;
    }

    if (!VoiceSDK::is_iterator_of_v<std::vector<float>::iterator, float>)
    {
        std::cerr << "VoiceSDK::is_iterator_value_type_v cannot compare its value type.";
        return -2;
    }

    if (VoiceSDK::is_iterator_of_v<std::vector<float>::iterator, int>)
    {
        std::cerr << "VoiceSDK::is_iterator_value_type_v cannot compare its value type.";
        return -2;
    }

    if (VoiceSDK::is_iterator_of_v<int, float>) // should be compilable even if it is not iterator
    {
        std::cerr << "VoiceSDK::is_iterator_value_type_v gives true even for a non iterator.";
        return -2;
    }
    return 0;

}



int main(){
    if (test_RingBuffer() != 0) return -1;
    if (test_type_traits() != 0) return -2;
    return 0;
}