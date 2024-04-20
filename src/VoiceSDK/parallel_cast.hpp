#ifndef __H_VOICE_SDK_PARALLEL_CAST_HPP
#define __H_VOICE_SDK_PARALLEL_CAST_HPP __TIMESTAMP__

#include "definitions.hpp"
#include "type_traits.hpp"

#include <future>
#include <vector>
#include <Eigen/Dense>

namespace VoiceSDK
{

/*
template <class To, class InputIt>
using vector_To_if_input_iterator_value_type_convertible_to_To
    = typename std::enable_if<
        is_input_iterator_v<InputIt> 
        && 
    , std::vector<To>>::type;

    */

#ifndef VoiceSDK_DISABLE_THREADS
template <class To, class InputIt>
enable_if_input_iterator_t<InputIt, std::vector<To>> parallel_cast(InputIt from_begin, InputIt from_end)
{
    std::vector<std::future<To>> futures;
    futures.reserve(from_array.size());
    for (Iter it = from_begin; it != from_end; ++it)
    {
        futures.push_back(std::async(std::launch:async, [it]() {
            return static_cast<To>(*it);
        }));
    }

    // Wait for all tasks to finish and collect results
    std::vector<To> results;
    results.reserve(from_array.size());
    for (auto& future: futures)
        results.push_back(future.get());

    return results;
}

#else
template <class To, class InputIt>
enable_if_input_iterator_t<InputIt, std::vector<To>> parallel_cast(InputIt from_begin, InputIt from_end)
{
    std::vector<To> results;
    results.reserve(from_array.size());

    std::transform(from_begin, from_end, std::back_inserter(results),
        [](const From& from) { return static_cast<To>(from); }
    );

    return results;
}
#endif

template <class To, class From>
std::enable_if<std::is_convertible<From, To>::value, std::vector<To>> parallel_cast(const std::vector<From>& from_array)
{
    
}


} // namespace VoiceSDK

#endif // __H_VOICE_SDK_PARALLEL_CAST_HPP