#ifndef __H_VOICE_SDK_RINGBUFFER_HPP
#define __H_VOICE_SDK_RINGBUFFER_HPP __TIMESTAMP__

#include "definitions.hpp"
#include <type_traits>
#include <algorithm>
#include <memory>
#include <vector>
#include <array>
#include <valarray>
#include <utility>
#include <cstring>
#include <iterator>
#include <string>

namespace VoiceSDK{

#pragma region is_contiguous_iterator

template <class Iter>
struct is_contiguous_iterator : std::integral_constant<bool, false> {};

template <class Ptr, typename = std::enable_if<std::is_pointer<Ptr>::value>::type>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};

#if _CPP_VER >= 202002L
template <class Iter, typename = std::enable_if_t<std::contiguous_iterator<Iter>>>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};
#else

template <class Iter, typename = std::enable_if<std::is_same<Iter, typename std::vector<typename std::iterator_traits<Iter>::value_type>::iterator>::value>::type>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};
template <class Iter, typename = std::enable_if<std::is_same<Iter, typename std::array<typename std::iterator_traits<Iter>::value_type, 10>::iterator>::value>::type>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};
template <class Iter, typename = std::enable_if<std::is_same<Iter, decltype(std::begin(std::declval<std::valarray<typename std::iterator_traits<Iter>::value_type>>()))>::value>::type>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};
template <class Iter, typename = std::enable_if<std::is_same<Iter, typename std::basic_string<typename std::iterator_traits<Iter>::value_type>::iterator>::value>::type>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};

#if _CPP_VER >= 201703L
template <class Iter, typename = std::enable_if<std::is_same<Iter, typename std::basic_string_view<typename std::iterator_traits<Iter>::value_type>::iterator>::value>::type>
struct is_contiguous_iterator : std::integral_constant<bool, true> {};
#endif

#endif

template <class Iter>
inline constexpr bool is_contiguous_iterator_v = is_contiguous_iterator<Iter>::value;

#pragma endregion

template <typename T, size_t BufferSize, class Allocator = std::allocator<T>>
class RingBuffer {
private:
	std::array<T, BufferSize> buffer;
	size_t head = 0;
	size_t tail = 0;
	bool contains_content = false;
public:
	static constexpr size_t buffer_size = BufferSize;
	using value_type = T;
	using allocator_type = Allocator;
	using size_type = typename std::allocator_traits<allocator_type>::size_type;
	using difference_type = typename std::allocator_traits<allocator_type>::difference_type;
	using pointer = typename std::allocator_traits<allocator_type>::pointer;
	using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

	void clear()
	{
		contains_content = false;
		head = 0;
		tail = 0;
		buffer.fill(value_type());
	}

	RingBuffer() 
	{
		clear();
	}
	constexpr RingBuffer(const RingBuffer&) = default;
	template <size_t OtherBufferSize>
	explicit RingBuffer(RingBuffer<T, OtherBufferSize>&& other) 
	{
		auto temp = other.dequeue();
		enqueue(temp.cbegin(), temp.cend());
	}

	template <size_t OtherBufferSize>
	explicit RingBuffer(const RingBuffer<T, OtherBufferSize>& other)
	{
		auto temp = other;
		auto temp2 = temp.dequeue();
		enqueue(temp2.cbegin(), temp2.cend());
	}

	constexpr bool empty() { return !contains_content; }
	constexpr size_type content_size() { return contains_content ? (tail + buffer_size - head) % buffer_size : 0; }

	/*!
	 * @brief Enqueues the given byte array into the ring buffer by converting the byte array into the buffer's data type.
	 */
	void enqueue(const void* begin, size_t size)
	{
		enqueue((const_pointer)begin, size);
	}

	template <class InputIt>
	typename std::enable_if_t<std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value, void>
		enqueue(InputIt begin, InputIt end)
	{
		enqueue(begin, std::distance(begin, end));
	}

	template <class InputIt>
	typename std::enable_if_t<std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>::value, void>
		enqueue(InputIt begin, size_t size) 
	{
		if (size > buffer_size)
		{
			std::advance(begin, size - buffer_size);
			size = buffer_size;
		}

		const size_type part1 = std::min(size, buffer_size - tail);
		const size_type part2 = size - part1;

		IFCONSTEXPR(std::is_trivially_copyable_v<value_type> &&
			is_contiguous_iterator_v<InputIt>)
		{
			std::memcpy(buffer.data() + tail, &*(begin), part1 * sizeof(value_type));
			std::memcpy(buffer.data(), &*(begin)+part1, part2 * sizeof(value_type));
		}
		else // IFCONSTEXPR(std::is_base_of_v<std::random_access_iterator_tag, std::iterator_traits<InputIt>::iterator_category>)
		{
			InputIt middle = begin, end = begin;
			std::advance(middle, part1);
			std::advance(end, size);
			std::copy(begin, middle, buffer.data() + tail);
			std::copy(middle, end, buffer.data());
		}

		tail = (tail + size) % buffer_size;
		contains_content = contains_content || size;
	}

	// casts and enqueue

	#pragma region dequeue

	std::vector<value_type> dequeue() { return dequeue(content_size()); }

	std::vector<value_type> dequeue(size_type elem_count)
	{
		const size_type size = std::min(content_size(), elem_count);

		std::vector<value_type> result;
		result.reserve(size);

		const size_type part1 = std::min(size, buffer_size - head);
		const size_type part2 = size - part1;

		const_pointer begin = buffer.data() + head;
		const_pointer middle = begin + part1;
		const_pointer end = buffer.data() + part2;

		IFCONSTEXPR(std::is_trivially_copyable_v<value_type>)
		{
			result.resize(size);
			std::memcpy(result.data(), begin, part1 * sizeof(value_type));
			std::memcpy(result.data() + part1, buffer.data(), part2 * sizeof(value_type));
		}
		else
		{
			result.insert(result.end(), begin, middle);
			result.insert(result.end(), buffer.data(), end);
		}

		head = (head + size) % buffer_size;
		contains_content = (size == elem_count);
		return result;
	}

	#pragma endregion

};

}

#endif