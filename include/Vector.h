#ifndef VECTOR_H
#define VECTOR_H

#include "VectorBase.h"
#include <memory>

template<typename T, typename A>
class Vector;

template<typename T, typename A>
void swap(Vector<T, A>& a, Vector<T, A>& b);

template<typename T, typename A = std::allocator<T> >
class Vector
{
private:
    VectorBase<T> base;
public:
    Vector() : Vector(0) {}
    Vector(size_t n, const T& val = T(), const A& alloc = A());
    Vector(const Vector& other);
    Vector& operator=(const Vector& other);
    Vector(Vector&& other);
    Vector& operator=(Vector&& other);
    ~Vector()                           { clear(); }

    int capacity() const                { return base.last - base.first; }
    int size() const                    { return base.space - base.first; }
    bool empty() const                  { return size() == 0; }
    T& operator[](int i)                { return *(base.first + i); }
    const T& operator[](int i) const    { return *(base.first + i); }
    T& back()                           { return *(base.space - 1); }
    T& front()                          { return *(base.first); }

    void clear();
    void reserve(size_t new_size);
    void resize(size_t new_size, const T& val = T());
    void push_back(const T& val);
    void pop_back();
    void insert(int index, const T& value);
    void erase(int index);

    friend void swap<T, A>(Vector<T, A>& a, Vector<T, A>& b);

private:
    static void uninitialized_move(T* begin, T* end, T* destination)
    {
        for(;begin != end; ++begin, ++destination)
        {
            new(static_cast<void*>(destination)) T(std::move(*begin));  // move construct
            begin->~T();
        }
    }

    static void destroy_range(T* begin, T* end) // destroy [begin, end)
    {
        for(;begin != end; ++begin)
        {
            begin->~T();
        }
    }

    static void swap_range_backwards(T* begin, T* end)
    {
        T* left = end - 1;
        while(end != begin)
        {
            std::swap(*(left--), *(end--));
        }
    }

};

template<typename T, typename A>
Vector<T, A>::Vector(size_t count, const T& val, const A& alloc)
    : base(alloc, count)
{
    // uses the copy constructor of T
    //std::cout << "Vector constructor called\n";
    std::uninitialized_fill(base.first, base.first + count, val);
}

template<typename T, typename A>
Vector<T, A>::Vector(const Vector& other)
    : base(other.base.alloc, other.size())
{
    //std::cout << "Vector copy constructor called\n";
    std::uninitialized_copy(other.base.first, other.base.space, base.first);
}

template<typename T, typename A>
Vector<T, A>& Vector<T, A>::operator=(const Vector& other)
{
    //std::cout << "Vector copy assignment called\n";
    Vector<T, A> temp(other);
    std::swap(*this, temp);
    //swap(*this, other);
    return *this;
}

template<typename T, typename A>
Vector<T, A>::Vector(Vector&& other)
    : base(std::move(other.base))       // force calling VectorBase move constructor
{
    //std::cout << "Vector move constructor called\n";
}

template<typename T, typename A>
Vector<T, A>& Vector<T, A>::operator=(Vector&& other)
{
    //std::cout << "Vector move assignment called\n";
    clear();        // redundant?
    base = std::move(other.base);
    return *this;
}

template<typename T, typename A>
void Vector<T, A>::clear()
{
    //std::cout << "Vector destructor called\n";
    for (T* cursor = base.first; cursor != base.space; ++cursor)
    {
        cursor->~T();
    }
    base.space = base.first;
    //std::cout << "Vector destructor finished\n";
    //base.printState();
}

template<typename T, typename A>
void Vector<T, A>::reserve(size_t new_size)
{
    if (new_size <= capacity())
        return;

    VectorBase<T, A> temp(base.alloc, size(), new_size - size());
    uninitialized_move(base.first, base.first + size(), temp.first);
    std::swap(base, temp);
}

template<class T, class A>
void Vector<T,A>::resize(size_t new_size, const T& val)
{
    //std::cout << "enter resize()\n";
    reserve(new_size);

    if (size() < new_size)
    {
        //std::cout << size() << " " << base.first + size() << " " << (base.first + new_size) - base.first + size() << "\n";
        std::uninitialized_fill(base.first + size(), base.first + new_size, val);    // construct new elements: [size(), newsize)
    }
    else
    {
        destroy_range(base.first + new_size, base.first + size());      // destroy surplus elements: [newsize, size())
    }

    base.space = base.last = base.first + new_size;
    //std::cout << "exit resize()\n";
}

template<typename T, typename A>
void Vector<T,A>::push_back(const T& val)
{
    //std::cout << "enter push_back\n";
    if (capacity() == size())                 // no more free space; relocate:
    {
        reserve( size() ? (2 * size()) : 8 );         // grow or start with 8
    }
    //base.alloc.construct(&base.first[size()], val);       // add val at end
    base.alloc.construct(base.space, val);
    ++base.space;                         // increment size
    //std::cout << "exit push_back\n";
}

template<typename T, typename A>
void Vector<T,A>::pop_back()
{
    if(!empty())
    {
        base.alloc.destroy(base.space - 1);
        --base.space;
    }
}

template<typename T, typename A>
void Vector<T,A>::insert(int index, const T& value)
{
    if (index == size() + 1)
    {
        return push_back(value);
    }

    if (size() == capacity())
    {
        reserve( size() ? (2 * size()) : 8 );         // grow or start with 8
    }

    base.alloc.construct(base.space, value);
    swap_range_backwards(base.first + index, base.space);
    ++base.space;
}

template<typename T, typename A>
void Vector<T,A>::erase(int index)
{
    if (empty())
    {
        return;
    }

    base.alloc.destroy(base.first + index);
    std::move(base.first + index + 1, base.space, base.first + index);
    --base.space;
}

template<typename T, typename A>
void swap(Vector<T, A>& a, Vector<T, A>& b)
{
    std::swap(a, b);
}

#endif // VECTOR_H
