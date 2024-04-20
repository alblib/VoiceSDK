#ifndef __H_VOICE_SDK_PARALLEL_CAST_HPP
#define __H_VOICE_SDK_PARALLEL_CAST_HPP __TIMESTAMP__

#include "definitions.hpp"
#include "type_traits.hpp"

#include <future>
#include <vector>
#include <Eigen/Dense>

namespace VoiceSDK
{

#ifndef VoiceSDK_DISABLE_THREADS
template <class To, class From>
std::enable_if<std::is_convertible<From, To>::value, std::vector<To>> parallel_cast(const std::vector<From>& from_array)
{
    std::vector<std::future<To>> futures;
    futures.reserve(from_array.size());
    for (const From& from: from_array)
    {
        futures.push_back(std::async(std::launch:async, [&from_array]() {
            return static_cast<To>(from);
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
template <class To, class From>
std::enable_if<std::is_convertible<From, To>::value, std::vector<To>> parallel_cast(const std::vector<From>& from_array)
{
    std::vector<To> results;
    results.reserve(from_array.size());

    std::transform(from_array.cbegin(), from_array.cend(), std::back_inserter(results),
        [](const From& from) { return static_cast<To>(from); }
    );

    return results;
}
#endif

} // namespace VoiceSDK

#endif // __H_VOICE_SDK_PARALLEL_CAST_HPP