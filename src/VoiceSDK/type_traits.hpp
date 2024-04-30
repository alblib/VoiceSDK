#ifndef __H_VOICE_SDK_TYPE_TRAITS_HPP
#define __H_VOICE_SDK_TYPE_TRAITS_HPP __TIMESTAMP__

#include <type_traits>
#include <vector>
#include <array>
#include <string>
#include <valarray>
#include <iterator>
#include <Eigen/Dense>

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
using enable_if_input_iterator = std::enable_if<is_input_iterator_v<Iter>, Type>;

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

template <class InputIt, class ValueType>
using is_input_iterator_of 
    = std::integral_constant<bool, 
        is_input_iterator_v<InputIt>
        && is_iterator_of_v<InputIt, ValueType>
    >;

template <class InputIt, class ValueType>
inline constexpr bool is_input_iterator_of_v = is_input_iterator_of<InputIt, ValueType>::value;

#pragma endregion

#pragma region enable_if_input_iterator_of

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_input_iterator_of = typename std::enable_if<is_input_iterator_v<InputIt> && is_iterator_of_v<InputIt, ValueType>, Type>;

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_input_iterator_of_t = typename enable_if_input_iterator_of<InputIt, ValueType, Type>::type;

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

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_iterator_of_ignore_cv 
    = typename std::enable_if<is_iterator_of_ignore_cv_v<InputIt, ValueType>, Type>;

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_iterator_of_ignore_cv_t 
    = typename enable_if_iterator_of_ignore_cv<InputIt, ValueType, Type>::type;

template <class InputIt, class ValueType>
using is_input_iterator_of_ignore_cv
    = std::integral_constant<bool, 
        is_input_iterator_v<InputIt>
        && is_iterator_of_ignore_cv_v<InputIt, ValueType>
    >;

template <class InputIt, class ValueType>
inline constexpr bool is_input_iterator_of_ignore_cv_v 
    = is_input_iterator_of_ignore_cv<InputIt, ValueType>::value;

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_input_iterator_of_ignore_cv 
    = typename std::enable_if<is_input_iterator_of_ignore_cv_v<InputIt, ValueType>, Type>;

template <typename InputIt, typename ValueType, typename Type = void>
using enable_if_input_iterator_of_ignore_cv_t 
    = typename enable_if_input_iterator_of_ignore_cv<InputIt, ValueType, Type>::type;

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

template <class InputIt, class BaseOfValueType, class Type = void>
using enable_if_iterator_of_derived_from 
    = typename std::enable_if<is_iterator_of_derived_from_v<InputIt, BaseOfValueType>, Type>;

template <class InputIt, class BaseOfValueType, class Type = void>
using enable_if_iterator_of_derived_from_t 
    = typename enable_if_iterator_of_derived_from<InputIt, BaseOfValueType, Type>::type;

template <class InputIt, class BaseOfValueType>
using is_input_iterator_of_derived_from
    = std::integral_constant<bool, 
        is_input_iterator_v<InputIt>
        && is_iterator_of_derived_from_v<InputIt, BaseOfValueType>
    >;

template <class InputIt, class BaseOfValueType>
inline constexpr bool is_input_iterator_of_derived_from_v 
    = is_input_iterator_of_derived_from<InputIt, BaseOfValueType>::value;
    
template <class InputIt, class BaseOfValueType, class Type = void>
using enable_if_input_iterator_of_derived_from 
    = typename std::enable_if<is_input_iterator_of_derived_from_v<InputIt, BaseOfValueType>, Type>;

template <class InputIt, class BaseOfValueType, class Type = void>
using enable_if_input_iterator_of_derived_from_t 
    = typename enable_if_input_iterator_of_derived_from<InputIt, BaseOfValueType, Type>::type;

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

template <class InputIt, class To, class Type = void>
using enable_if_iterator_of_convertible_to 
    = typename std::enable_if<is_iterator_of_convertible_to_v<InputIt, To>, Type>;

template <class InputIt, class To, class Type = void>
using enable_if_iterator_of_convertible_to_t 
    = typename enable_if_iterator_of_convertible_to<InputIt, To, Type>::type;

template <class InputIt, class To>
using is_input_iterator_of_convertible_to
    = std::integral_constant<bool, 
        is_input_iterator_v<InputIt>
        && is_iterator_of_convertible_to_v<InputIt, To>
    >;

template <class InputIt, class To>
inline constexpr bool is_input_iterator_of_convertible_to_v 
    = is_input_iterator_of_convertible_to<InputIt, To>::value;

template <class InputIt, class To, class Type = void>
using enable_if_input_iterator_of_convertible_to 
    = typename std::enable_if<is_input_iterator_of_convertible_to_v<InputIt, To>, Type>;

template <class InputIt, class To, class Type = void>
using enable_if_input_iterator_of_convertible_to_t 
    = typename enable_if_input_iterator_of_convertible_to<InputIt, To, Type>::type;

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

template <class InputIt, class From, class Type = void>
using enable_if_iterator_of_convertible_from 
    = typename std::enable_if<is_iterator_of_convertible_from_v<InputIt, From>, Type>;
    
template <class InputIt, class From, class Type = void>
using enable_if_iterator_of_convertible_from_t
    = typename enable_if_iterator_of_convertible_from<InputIt, From, Type>::type;

template <class InputIt, class From>
using is_input_iterator_of_convertible_from
    = std::integral_constant<bool, 
        is_input_iterator_v<InputIt>
        && is_iterator_of_convertible_from_v<InputIt, From>
    >;

template <class InputIt, class From>
inline constexpr bool is_input_iterator_of_convertible_from_v 
    = is_input_iterator_of_convertible_from<InputIt, From>::value;

template <class InputIt, class From, class Type = void>
using enable_if_input_iterator_of_convertible_from 
    = typename std::enable_if<is_input_iterator_of_convertible_from_v<InputIt, From>, Type>;
    
template <class InputIt, class From, class Type = void>
using enable_if_input_iterator_of_convertible_from_t
    = typename enable_if_input_iterator_of_convertible_from<InputIt, From, Type>::type;

#pragma endregion



} // namespace VoiceSDK

#endif //__H_VOICE_SDK_TYPE_TRAITS_HPP
