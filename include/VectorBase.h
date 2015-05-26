#ifndef VECTORBASE_H
#define VECTORBASE_H

template<typename T, typename A = std::allocator<T> >
struct VectorBase
{
    A alloc_;
	T* first_;      // start of allocated space, start of element sequence
	T* space_;      // end of element sequence, start of space allocated for possible expansion
	T* last_;       // end of allocated space

    void printState();

    VectorBase(const A& alloc, typename A::size_type n)
        : alloc_(alloc), first_(alloc_.allocate(n)), space_(first_ + n), last_(first_ + n) {printState();}

    VectorBase(const VectorBase& other) = delete;
    VectorBase& operator=(const VectorBase& other) = delete;

    VectorBase(const VectorBase&& other);
    VectorBase& operator=(const VectorBase&& other);

    ~VectorBase() {alloc_.deallocate(first_, last_ - first_);}
};

template<typename T, typename A>
void VectorBase<T, A>::printState()
{
    std::cout << "vector base size: " << last_ - first_ << "\n";
}


#endif // VECTORBASE_H
