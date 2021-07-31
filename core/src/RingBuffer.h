#pragma once

#include <vector>
#include <memory>

template <typename Element>
class RingBuffer
{
public:
	RingBuffer(int capacity);
	~RingBuffer() = default;

	bool offer(std::unique_ptr<Element> element);
	Element &poll();
private:
	int capacity;
	size_t index = 0;
	std::vector<std::unique_ptr<Element>> elements;
};

template <typename Element>
RingBuffer<Element>::RingBuffer(int capacity) : capacity{ capacity }
{
	
}

template <typename Element>
bool RingBuffer<Element>::offer(std::unique_ptr<Element> element)
{
	if (elements.size() == capacity)
	{
		return false;
	}
	elements.push_back(std::move(element));
	return true;
}

template <typename Element>
Element &RingBuffer<Element>::poll()
{
	index = (index + 1) % capacity;
	printf("%d",index);
	return *elements[index].get();
}
