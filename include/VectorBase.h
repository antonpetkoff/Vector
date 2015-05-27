#ifndef VECTORBASE_H
#define VECTORBASE_H

template<typename T, typename A = std::allocator<T> >
struct VectorBase
{
    A alloc;
	T* first;      // start of allocated space, start of element sequence
	T* space;      // end of element sequence, start of space allocated for possible expansion
	T* last;       // end of allocated space

    void printState();

    VectorBase(const A& a, typename A::size_type n)
        : alloc(a), first(alloc.allocate(n)), space(first + n), last(first + n) {printState();}

    VectorBase(const VectorBase& other) = delete;
    VectorBase& operator=(const VectorBase& other) = delete;

    VectorBase(VectorBase&& other);
    VectorBase& operator=(VectorBase&& other);

    ~VectorBase();
};

template<typename T, typename A>
VectorBase<T, A>::~VectorBase()
{
    std::cout << "VectorBase destructor called\n";
    alloc.deallocate(first, last - first);
    std::cout << "VectorBase destructor finished\n";
}

template<typename T, typename A>
void VectorBase<T, A>::printState()
{
    std::cout << "vector base size: " << last - first << "\n";
}

template<typename T, typename A>
VectorBase<T, A>::VectorBase(VectorBase&& other)
    : alloc(other.alloc), first(other.first), space(other.space), last(other.last)
{
    std::cout << "VectorBase move constructor\n";
    other.first = other.space = other.last = nullptr;
}

template<typename T, typename A>
VectorBase<T, A>& VectorBase<T, A>::operator=(VectorBase&& other)
{
    std::cout << "VectorBase move assignment\n";
    std::swap(*this, other);
    return *this;
}

#endif // VECTORBASE_H
