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
template <class InputIt>
struct is_input_iterator : std::integral_constant<bool, std::input_iterator<InputIt>> {};
#else
template <class InputIt, class = void>
struct is_input_iterator : std::false_type {};
template <class InputIt>
struct is_input_iterator<InputIt, typename std::enable_if<
    std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value
>::type> : std::true_type {};
#endif

template <class Iter>
inline constexpr bool is_input_iterator_v = is_input_iterator<Iter>::value;

#pragma endregion


#pragma region enable_if_input_iterator

template <typename Iter, typename Type = void>
struct enable_if_input_iterator : std::enable_if<is_input_iterator_v<Iter>, Type> {};

template <typename Iter, typename Type = void>
using enable_if_input_iterator_t = typename enable_if_input_iterator<Iter, Type>::type;

#pragma endregion


#pragma region is_iterator_of
template <class Iter, class ValueType, class = void>
struct is_iterator_of: std::false_type {};

template <class Iter, class ValueType>
struct is_iterator_of<
    Iter, ValueType,
    typename std::enable_if<
        std::is_same<typename std::iterator_traits<Iter>::value_type, ValueType>::value
    >::type
> : std::true_type {};

template <class Iter, class ValueType>
inline constexpr bool is_iterator_of_v
    = is_iterator_of<Iter, ValueType>::value;
#pragma endregion

#pragma region is_iterator_of_ignore_cv
template <class Iter, class ValueType, class = void>
struct is_iterator_of_ignore_cv : std::false_type {};

template <class Iter, class ValueType>
struct is_iterator_of_ignore_cv<
    Iter, ValueType,
    typename std::enable_if<
        std::is_same<
            typename std::remove_cv<typename std::iterator_traits<Iter>::value_type>::type, 
            typename std::remove_cv<ValueType>::type
        >::value
    >::type
> : std::true_type {};

template <class Iter, class ValueType>
inline constexpr bool is_iterator_of_ignore_cv_v 
    = is_iterator_of_ignore_cv<Iter, ValueType>::value;
#pragma endregion

#pragma region is_iterator_of_derived_from
template <class Iter, class BaseOfValueType, class = void>
struct is_iterator_of_derived_from : std::false_type {};

template <class Iter, class BaseOfValueType>
struct is_iterator_of_derived_from<
    Iter, BaseOfValueType,
    typename std::enable_if<
        std::is_base_of<BaseOfValueType, typename std::iterator_traits<Iter>::value_type>::value
    >::type
> : std::true_type {};

template <class Iter, class BaseOfValueType>
inline constexpr bool is_iterator_of_derived_from_v
    = is_iterator_of_derived_from<Iter, BaseOfValueType>::value;

#pragma endregion

#pragma region is_iterator_of_convertible_to

template <class Iter, class To, class = void>
struct is_iterator_of_convertible_to : std::false_type {};

template <class Iter, class To>
struct is_iterator_of_convertible_to<
    Iter, To,
    typename std::enable_if<
        std::is_convertible<typename std::iterator_traits<Iter>::value_type, To>::value
    >::type
> : std::true_type {};

template <class Iter, class To>
inline constexpr bool is_iterator_of_convertible_to_v
    = is_iterator_of_convertible_to<Iter, To>::value;
#pragma endregion


#pragma region is_iterator_of_convertible_from

template <class Iter, class From, class = void>
struct is_iterator_of_convertible_from : std::false_type {};

template <class Iter, class From>
struct is_iterator_of_convertible_from<
    Iter, From,
    typename std::enable_if<
    std::is_convertible<From, typename std::iterator_traits<Iter>::value_type>::value
    >::type
> : std::true_type {};

template <class Iter, class From>
inline constexpr bool is_iterator_of_convertible_from_v
= is_iterator_of_convertible_from<Iter, From>::value;
#pragma endregion

#pragma region enable_if_input_iterator_of

template <typename InputIt, typename ValueType, typename Type = void>
struct enable_if_input_iterator_of : std::enable_if<is_input_iterator_v<InputIt> && is_iterator_of_v<InputIt, ValueType>, Type> {};

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_input_iterator_of_t = typename enable_if_input_iterator_of<InputIt, Type>::type;

#pragma endregion

#pragma region enable_if_input_iterator_of_ignore_cv

template <typename InputIt, typename ValueType, typename Type = void>
struct enable_if_input_iterator_of_ignore_cv : std::enable_if<is_input_iterator_v<InputIt> && is_iterator_of_ignore_cv_v<InputIt, ValueType>, Type> {};

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_input_iterator_of_ignore_cv_t = typename enable_if_input_iterator_of_ignore_cv<InputIt, Type>::type;

#pragma endregion

} // namespace VoiceSDK

#endif //__H_VOICE_SDK_TYPE_TRAITS_HPP