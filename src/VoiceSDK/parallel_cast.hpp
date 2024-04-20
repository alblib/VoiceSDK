#ifndef __H_VOICE_SDK_PARALLEL_CAST_HPP
#define __H_VOICE_SDK_PARALLEL_CAST_HPP __TIMESTAMP__

#include "definitions.hpp"
#include "type_traits.hpp"

#include <future>
#include <vector>
#include <Eigen/Dense>
#include <valarray>
#include <array>

namespace VoiceSDK
{

#pragma region Generic array cast

#ifndef VoiceSDK_DISABLE_THREADS
template <class To, class InputIt>
enable_if_input_iterator_of_convertible_to_t<InputIt, To, std::vector<To>>
    parallel_cast(InputIt from_begin, InputIt from_end)
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

template <class To, class From>
typename std::enable_if<std::is_convertible<From, To>::value, std::vector<To>>::type
    parallel_cast(const std::vector<From>& from_array)
{
    return parallel_cast<To>(from_array.cbegin(), from_array.cend());
}

template <class To, size_t N, class From>
typename std::enable_if<std::is_convertible<From, To>::value, std::array<To, N>>::type
    parallel_cast(const std::array<From, N>& from_array)
{
    std::vector<std::future<To>> futures;
    futures.reserve(from_array.size());
    for (const From& from: from_array)
    {
        futures.push_back(std::async(std::launch:async, [&from]() {
            return static_cast<To>(from);
        }));
    }

    // Wait for all tasks to finish and collect results
    std::array<To, N> results;
    for (size_t i = 0; i < N; ++i)
        results[i] = futures[i].get();

    return results;
}

template <class To, size_t N, class From>
std::enable_if<std::is_convertible<From, To>::value, std::valarray<To>>::type
    parallel_cast(const std::valarray<From>& from_array)
{
    std::vector<std::future<To>> futures;
    futures.reserve(from_array.size());
    for (const From& from: from_array)
    {
        futures.push_back(std::async(std::launch:async, [&from]() {
            return static_cast<To>(from);
        }));
    }

    // Wait for all tasks to finish and collect results
    std::valarray<To> results(from_array.size());
    for (size_t i = 0; i < N; ++i)
        results[i] = futures[i].get();

    return results;
}

#else
template <class To, class InputIt>
enable_if_input_iterator_of_convertible_to_t<InputIt, To, std::vector<To>>
    parallel_cast(InputIt from_begin, InputIt from_end)
{
    std::vector<To> results;
    results.reserve(from_array.size());

    std::transform(from_begin, from_end, std::back_inserter(results),
        [](const From& from) { return static_cast<To>(from); }
    );

    return results;
}

template <class To, class From>
std::enable_if<std::is_convertible<From, To>::value, std::vector<To>>::type
    parallel_cast(const std::vector<From>& from_array)
{
    return parallel_cast<To>(from_array.cbegin(), from_array.cend());
}

template <class To, size_t N, class From>
std::enable_if<std::is_convertible<From, To>::value, std::array<To, N>>::type
    parallel_cast(const std::array<From, N>& from_array)
{
    std::array<To, N> results;
    for (size_t i = 0; i < N; ++i)
        results[i] = static_cast<To>(from_array[i]);

    return results;
}

template <class To, size_t N, class From>
std::enable_if<std::is_convertible<From, To>::value, std::valarray<To>>::type
    parallel_cast(const std::valarray<From>& from_array)
{
    std::valarray<To> results(from_array.size());
    for (size_t i = 0; i < N; ++i)
        results[i] = static_cast<To>(from_array[i]);

    return results;
}

#endif

#pragma endregion

template<typename To, typename From>
typename std::enable_if<is_eigen_data_type_v<To> && is_eigen_data_type_v<From>, std::vector<To>>::type
    parallel_cast<To>(const std::vector<From>& from_array)
{
    auto from_array_ref
        = Eigen::Map<Eigen::Array<std::add_const<From>::type, Eigen::Dynamic, 1>>(from_array.data(), from_array.size());
    Eigen::Array<To, Eigen::Dynamic, 1> to_array = from_array_ref.cast<To>();
    return std::vector<To>(to_array.data(), to_array.data() + from_array.size());
}



} // namespace VoiceSDK

#endif // __H_VOICE_SDK_PARALLEL_CAST_HPP