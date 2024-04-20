#ifndef __H_VOICE_SDK_TYPE_TRAITS_HPP
#define __H_VOICE_SDK_TYPE_TRAITS_HPP __TIMESTAMP__

#include <type_traits>
#include <vector>
#include <array>
#include <string>
#include <valarray>
#include <iterator>

namespace VoiceSDK
{

#pragma region is_contiguous_iterator

template <class Iter, typename = void>
struct is_contiguous_iterator : std::false_type {};

template <class Ptr>
struct is_contiguous_iterator<Ptr, typename std::enable_if<std::is_pointer<Ptr>::value>::type> : std::true_type {};

#if _CPP_VER >= 202002L
template <class Iter>
struct is_contiguous_iterator<Iter, std::enable_if_t<std::contiguous_iterator<Iter>>> : std::true_type {};
#else

template <class Iter>
struct is_contiguous_iterator<Iter, typename std::enable_if<std::is_same<Iter, typename std::vector<typename std::iterator_traits<Iter>::value_type>::iterator>::value>::type> : std::true_type {};
template <class Iter>
struct is_contiguous_iterator<Iter, typename std::enable_if<std::is_same<Iter, typename std::array<typename std::iterator_traits<Iter>::value_type, 10>::iterator>::value>::type> : std::true_type {};
template <class Iter>
struct is_contiguous_iterator<Iter, typename std::enable_if<std::is_same<Iter, decltype(std::begin(std::declval<std::valarray<typename std::iterator_traits<Iter>::value_type>>()))>::value>::type> : std::true_type {};
template <class Iter>
struct is_contiguous_iterator<Iter, typename std::enable_if<std::is_same<Iter, typename std::basic_string<typename std::iterator_traits<Iter>::value_type>::iterator>::value>::type> : std::true_type {};

#if _CPP_VER >= 201703L
template <class Iter>
struct is_contiguous_iterator<Iter, typename std::enable_if<std::is_same<Iter, typename std::basic_string_view<typename std::iterator_traits<Iter>::value_type>::iterator>::value>::type> : std::true_type {};
#endif

#endif

template <class Iter>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<Iter>::value;

#pragma endregion


#pragma region is_input_iterator


#if _CPP_VER >= 202002L
template <class Iter>
struct is_input_iterator : std::integral_constant<bool, std::input_iterator<Iter>> {};
#else
template <class Iter>
struct is_input_iterator : std::integral_constant<bool, std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<Iter>::iterator_category>::value> {};
#endif

template <class Iter>
inline constexpr bool is_input_iterator_v = is_input_iterator<Iter>::value;

#pragma endregion

} // namespace VoiceSDK

#endif //__H_VOICE_SDK_TYPE_TRAITS_HPP