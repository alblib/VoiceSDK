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
#include <Eigen/Dense>

#ifndef VoiceSDK_DISABLE_THREADS
#include <queue>
#include <mutex>
#include <condition_variable>
#endif

namespace VoiceSDK{

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


template <typename T, size_t BufferSize, class Allocator = std::allocator<T>>
class RingBuffer {
private:
	std::array<T, BufferSize> buffer;
	size_t head = 0;
	size_t tail = 0;
	bool contains_content = false;

#ifndef VoiceSDK_DISABLE_THREADS
	mutable std::mutex mutex_;
	mutable std::condition_variable condition_;
#endif

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
	RingBuffer(const RingBuffer& other)
	{
		operator = (other);
	}

	RingBuffer& operator = (const RingBuffer& other)
	{
#ifndef VoiceSDK_DISABLE_THREADS
		std::lock_guard<std::mutex> lock(other.mutex_);
#endif

		buffer = other.buffer;
		head = other.head;
		tail = other.tail;
		contains_content = other.contains_content;

#ifndef VoiceSDK_DISABLE_THREADS
		other.condition_.notify_one();
#endif
		return *this;
	}

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
	typename std::enable_if<is_input_iterator_v<InputIt>>::type
		enqueue(InputIt begin, InputIt end)
	{
		enqueue(begin, std::distance(begin, end));
	}

	template <class InputIt>
	typename std::enable_if<is_input_iterator_v<InputIt>>::type
		enqueue(InputIt begin, size_t size)
	{
#ifndef VoiceSDK_DISABLE_THREADS
		std::lock_guard<std::mutex> lock(mutex_);
#endif

		if (size > buffer_size)
		{
			std::advance(begin, size - buffer_size);
			size = buffer_size;
		}

		const size_type part1 = std::min(size, buffer_size - tail);
		const size_type part2 = size - part1;

		IFCONSTEXPR(std::is_trivially_copyable<value_type>::value &&
			is_contiguous_iterator_v<InputIt>)
		{
			std::memcpy(buffer.data() + tail, &*(begin), part1 * sizeof(value_type));
			std::memcpy(buffer.data(), &*(begin)+part1, part2 * sizeof(value_type));
		}
		else
		{
			InputIt middle = begin, end = begin;
			std::advance(middle, part1);
			std::advance(end, size);
			std::copy(begin, middle, buffer.data() + tail);
			std::copy(middle, end, buffer.data());
		}

		tail = (tail + size) % buffer_size;
		contains_content = contains_content || size;

#ifndef VoiceSDK_DISABLE_THREADS
		condition_.notify_one();
#endif
	}

	// casts and enqueue

	#pragma region dequeue

	std::vector<value_type> dequeue() { return dequeue(content_size()); }

	std::vector<value_type> dequeue(size_type elem_count)
	{
#ifndef VoiceSDK_DISABLE_THREADS
		std::lock_guard<std::mutex> lock(mutex_);
#endif
		const size_type size = std::min(content_size(), elem_count);

		std::vector<value_type> result;
		result.reserve(size);

		const size_type part1 = std::min(size, buffer_size - head);
		const size_type part2 = size - part1;

		const_pointer begin = buffer.data() + head;
		const_pointer middle = begin + part1;
		const_pointer end = buffer.data() + part2;

		IFCONSTEXPR(std::is_trivially_copyable<value_type>::value)
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

#ifndef VoiceSDK_DISABLE_THREADS
		condition_.notify_one();
#endif

		return result;
	}

	#pragma endregion

	typename std::enable_if<std::is_floating_point<T>::value, RingBuffer&>::type
		multiply(value_type multiplier)
	{
#ifndef VoiceSDK_DISABLE_THREADS
		std::lock_guard<std::mutex> lock(mutex_);
#endif
		const size_type part1 = std::min(size, buffer_size - head);
		const size_type part2 = size - part1;

		const_pointer begin = buffer.data() + head;
		const_pointer middle = begin + part1;
		const_pointer end = buffer.data() + part2;

		auto map1 = Eigen::Map<Eigen::ArrayX<T>>(begin, part1);
		auto map2 = Eigen::Map<Eigen::ArrayX<T>>(middle, part2);

		map1 *= multiplier;
		map2 *= multiplier;

#ifndef VoiceSDK_DISABLE_THREADS
		condition_.notify_one();
#endif
		return *this;
	}
};

}

#endif