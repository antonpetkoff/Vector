#ifndef VECTORBASE_H
#define VECTORBASE_H

#include <iostream>

/**
 *  Помощен вектор, чрез който имплементираме RAII техниката
 */
template<typename T, typename A = std::allocator<T> >
class VectorBase
{
public:
    A alloc;        // управлението на паметта се делегира на алокатор
	T* first;       // начало на заделеното пространство, маркира първият елемент
	T* space;       // маркира едно място след последния елемент, съответно начало на свободния капацитет
	T* last;        // маркира едно място след края на капацитета

    /**
     *  заделяме паметта, която ни е нужна, общо n + m капацитет, където n е броя на елементите
     *  @param  a - алокатор
     *  @param  n - брой на елементите във вектора
     *  @param  m - брой на свободните позиции в оставащия капацитет на базовия вектор
     */
    VectorBase(const A& a, typename A::size_type n, typename A::size_type m = 0)
        : alloc(a), first(alloc.allocate(n + m)), space(first + n), last(first + n + m) {}

    VectorBase(const VectorBase& other) = delete;               // не искаме копиращ конструктор
    VectorBase& operator=(const VectorBase& other) = delete;    // не искаме копиращо присвояване

    VectorBase(VectorBase&& other);
    VectorBase& operator=(VectorBase&& other);

    ~VectorBase();
};

/**
 *  алокатора се грижи да освободи първоначално заделената памет в деструктора
 */
template<typename T, typename A>
VectorBase<T, A>::~VectorBase()
{
    alloc.deallocate(first, last - first);
}

/**
 *  преместващ конструктор, който "краде" нужните данни от rvalue аргумента
 *  и оставя указателите да сочат към nullptr, за да избегнем двойно освобождаване
 *  на една и съща памет
 */
template<typename T, typename A>
VectorBase<T, A>::VectorBase(VectorBase&& other)
    : alloc(other.alloc), first(other.first), space(other.space), last(other.space)
{
    other.first = other.space = other.last = nullptr;
}

/**
 *  оператор за преместващо присвояване, която разменя данните почленно
 */
template<typename T, typename A>
VectorBase<T, A>& VectorBase<T, A>::operator=(VectorBase&& other)
{
    std::swap(alloc, other.alloc);
    std::swap(first, other.first);
    std::swap(space, other.space);
    std::swap(last, other.last);

    return *this;
}

#endif // VECTORBASE_H
