#ifndef VECTOR_H
#define VECTOR_H

template<typename T, typename A = std::allocator<T> >
class Vector
{
private:
    VectorBase<T> base;
public:

};


template<typename T, typename A>
void Vector<T, A>::printState()
{
    std::cout << "state\n";
}


#endif // VECTORBASE_H
