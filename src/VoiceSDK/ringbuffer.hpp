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

namespace VoiceSDK{

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
			(std::is_same_v<InputIt, pointer>
				|| std::is_same_v<InputIt, const_pointer>
				|| std::is_same_v<InputIt, typename std::vector<value_type>::iterator>
				|| std::is_same_v<InputIt, typename std::array<value_type, buffer_size>::iterator>
				|| std::is_same_v<InputIt, decltype(std::begin(std::declval<std::valarray<value_type>>()))>
				))
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