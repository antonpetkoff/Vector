#ifndef VECTORBASE_H
#define VECTORBASE_H

#include <iostream>

template<typename T, typename A = std::allocator<T> >
class VectorBase
{
public:
    A alloc;
	T* first;      // start of allocated space, start of element sequence
	T* space;      // end of element sequence, start of space allocated for possible expansion
	T* last;       // end of allocated space

    VectorBase(const A& a, typename A::size_type n, typename A::size_type m = 0);

    VectorBase(const VectorBase& other) = delete;
    VectorBase& operator=(const VectorBase& other) = delete;

    VectorBase(VectorBase&& other);
    VectorBase& operator=(VectorBase&& other);

    ~VectorBase();

    void printState();
};

template<typename T, typename A>
VectorBase<T, A>::VectorBase(const A& a, typename A::size_type n, typename A::size_type m)
    : alloc(a), first(alloc.allocate(n + m)), space(first + n), last(first + n + m)
{
    //std::cout << "VectorBase constructor\n";
}

template<typename T, typename A>
VectorBase<T, A>::~VectorBase()
{
    //std::cout << "VectorBase destructor called "<< this <<" \n";
    //std::cout << "deleting range of length: " << last - first << "\n";
    alloc.deallocate(first, last - first);
    //std::cout << "VectorBase destructor finished "<< this <<" \n";
}

template<typename T, typename A>
void VectorBase<T, A>::printState()
{
    std::cout << "vector base capacity: " << last - first << "\n";
    std::cout << "vector base elements size: " << space - first << "\n";
}

template<typename T, typename A>
VectorBase<T, A>::VectorBase(VectorBase&& other)
    : alloc(other.alloc), first(other.first), space(other.space), last(other.space)
{
    //std::cout << "enter VectorBase move constructor\n";
    other.first = other.space = other.last = nullptr;
    //std::cout << "exit VectorBase move constructor\n";
}

template<typename T, typename A>
VectorBase<T, A>& VectorBase<T, A>::operator=(VectorBase&& other)
{
    //std::cout << "enter VectorBase move assignment\n";

    std::swap(alloc, other.alloc);
    std::swap(first, other.first);
    std::swap(space, other.space);
    std::swap(last, other.last);

    //std::cout << "exit VectorBase move assignment\n";
    return *this;
}

#endif // VECTORBASE_H
