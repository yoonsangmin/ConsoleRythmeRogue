#pragma once

#pragma warning (disable : 4251)

#include <iostream>

template <typename T>
class List
{
public:
    List()
        : capacity(2)
    {
        data = new T[capacity];
        memset(data, 0, sizeof(T) * capacity);
    }

    ~List()
    {
        if (data != nullptr)
        {
            delete[] data;
        }
    }

    void PushBack(const T& value)
    {
        if (size == capacity)
        {
            ReAllocate(capacity * 2);
        }

        data[size] = value;
        size++;
    }

    void PushBack(T&& value)
    {
        if (size == capacity)
        {
            ReAllocate(capacity * 2);
        }

        data[size] = std::move(value);
        size++;
    }

    bool Contains(const T& value) const
    {
        for (int ix = 0; ix < size; ++ix)
        {
            if (data[ix] == value)
            {
                return true;
            }
        }

        return false;
    }

    void Erase(int index)
    {
        // 예외 처리.
        if (index < 0 || index >= size)
        {
            __debugbreak();
        }

        // 삭제한 데이터의 인덱스 다음 위치를 한 칸 앞으로 모두 이동.
        for (int ix = index; ix < size - 1; ++ix)
        {
            data[ix] = std::move(data[ix + 1]);
        }

        // 삭제 후 크기 줄이기.
        --size;
    }

    void Clear()
    {
        if (data != nullptr)
        {
            delete[] data;
        }
        
        size = 0;
        capacity = 2;
        data = new T[capacity];
        memset(data, 0, sizeof(T) * capacity);
    }

    int Size() const
    {
        return size;
    }

    int Capacity() const
    {
        return capacity;
    }

    // 복사 대입 연산자.
    List& operator=(const List& other)
    {
        // 자기 대입 방지.
        if (this != &other)
        {
            // 기존 데이터 삭제.
            delete[] data;

            // 새로 할당하고 데이터 복사.
            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];

            memcpy(data, other.data, sizeof(T) * capacity);
        }

        return *this;
    }

    // 이동 대입 연산자.
    List& operator=(List&& other)
    {
        // 자기 대입 방지.
        if (this != &other)
        {
            // 기존 데이터 삭제.
            delete[] data;

            // 다른 객체의 데이터로 이동.
            data = other.data;
            size = other.size;
            capacity = other.capacity;

            // 이동된 객체 초기화.
            other.data = nullptr;
            other.Clear();
        }

        return *this;
    }

    const T& operator[](int index) const
    {
        if (index >= size)
        {
            __debugbreak();
        }

        return data[index];
    }

    T& operator[](int index)
    {
        if (index >= size)
        {
            __debugbreak();
        }

        return data[index];
    }

    T* begin() const
    {
        return data;
    }

    T* end() const
    {
        return data + size;
    }

private:
    void ReAllocate(int newCapacity)
    {
        // 1. allocate a new block of memory.
        // 2. copy/move old elements into new block.
        // 3. delete.

        T* newBlock = new T[newCapacity];
        memset(newBlock, 0, sizeof(T) * newCapacity);

        if (newCapacity < size)
        {
            size = newCapacity;
        }

        //memcpy(newBlock, data, sizeof(T) * capacity);
        memcpy(newBlock, data, sizeof(T) * size);
        //for (int ix = 0; ix < size; ++ix)
        //{
        //    // newBlock[ix] = data[ix];
        //    newBlock[ix] = std::move(data[ix]);
        //}

        delete[] data;
        data = newBlock;
        capacity = newCapacity;
    }

private:
    T* data = nullptr;
    int size = 0;
    int capacity = 0;
};