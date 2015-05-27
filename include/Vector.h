#ifndef VECTOR_H
#define VECTOR_H

#include <memory>

template<typename T, typename A = std::allocator<T> >
class Vector
{
private:
    VectorBase<T> base;
    void destroy_elements();
public:
    explicit Vector(size_t n, const T& val = T(), const A& alloc = A());
    Vector(const Vector& other);
    ~Vector();
    int capacity() const    { return base.last - base.first; }
    int size() const        { return base.space - base.first; }
    T* begin() const        { return base.first; }
    T* end() const          { return base.space; }
};

template<typename T, typename A>
Vector<T, A>::Vector(size_t count, const T& val, const A& alloc)
    : base(alloc, count)
{
    // uses the copy constructor of T
    std::cout << "Vector constructor called\n";
    std::uninitialized_fill(base.first, base.first + count, val);
}

template<typename T, typename A>
Vector<T, A>::Vector(const Vector& other)
    : base(other.base.alloc, other.size())
{
    std::cout << "Vector copy constructor called\n";
    std::uninitialized_copy(other.begin(), other.base.space, base.first);
}

template<typename T, typename A>
void Vector<T, A>::destroy_elements()
{
    std::cout << "Vector destructor called\n";
    for (T* cursor = base.first; cursor != base.space; ++cursor)
    {
        cursor->~T();
    }
    base.space = base.first;
    std::cout << "Vector destructor finished\n";
    base.printState();
}

template<typename T, typename A>
Vector<T, A>::~Vector()
{
    destroy_elements();
}

#endif // VECTORBASE_H
