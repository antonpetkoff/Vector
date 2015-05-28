#ifndef VECTOR_H
#define VECTOR_H

#include "VectorBase.h"
#include <memory>
#include <cmath>

template<typename T, typename A>
class Vector;

template<typename T, typename A>
void swap(Vector<T, A>& a, Vector<T, A>& b);

/**
 *  Вектор, композиран от помощния вектор VectorBase, за да използва RAII техниката
 */
template<typename T, typename A = std::allocator<T> >
class Vector
{
private:
    VectorBase<T> base;

    static const int INITIAL_SIZE = 4;              // начален размер на вектор, в случай че е бил празен
    static constexpr double GROWTH_RATE = 1.5;      // множител на нарастване на вектора

public:
    Vector() : Vector(0) {}                                                     // конструктор по подразбиране, създава вектор с размер 0
    Vector(size_t n, const T& val = T(), const A& alloc = A());
    Vector(const Vector& other);
    Vector& operator=(const Vector& other);
    Vector(Vector&& other);
    Vector& operator=(Vector&& other);
    ~Vector()                           { clear(); }                            // дестуктор, който се грижи да унищожи елементите на вектора

    int capacity() const                { return base.last - base.first; }      // връща капацитета на вектора
    int size() const                    { return base.space - base.first; }     // връща броя елементи във вектора
    bool empty() const                  { return size() == 0; }                 // проверява дали вектора е празен
    T& operator[](int i)                { return *(base.first + i); }           // дава read-write достъп до i-я елемент на вектора
    const T& operator[](int i) const    { return *(base.first + i); }           // дава read-only дотъп до i-я елемент на вектора
    T& back()                           { return *(base.space - 1); }           // дава референция към последния елемент
    T& front()                          { return *(base.first); }               // дава референция към първия елемент

    void clear();
    void reserve(size_t new_size);
    void resize(size_t new_size, const T& val = T());
    void push_back(const T& val);
    void pop_back();
    void insert(int index, const T& value);
    void erase(int index);
    void shrink_to_fit();

    friend void swap<T, A>(Vector<T, A>& a, Vector<T, A>& b);

private:

    /**
     *  Помощна функция за преместващо копиране на интервал [begin, end) в интервал,
     *  започващ с dest включително. След изпълнението на функцията, елементите
     *  от интервала [begin, end) ще са унищожени.
     *
     *  @param  begin   - указател към първия елемент, който да се копира (включително)
     *  @param  end     - указател към последния елемент, който да се копира (изключващо)
     *  @param  dest    - маркира началото на предназначеното място, където да се копират елементите
     */
    static void uninitialized_move(T* begin, T* end, T* dest)
    {
        for(;begin != end; ++begin, ++dest)
        {
            new(static_cast<void*>(dest)) T(std::move(*begin));  // форсираме конструктора за преместващо копиране
            begin->~T();
        }
    }

    /**
     *  Помощна функция за унищожаване на елементите в интервал [begin, end)
     *
     *  @param  begin   - указател към първия елемент, който да се унищожи (включително)
     *  @param  end     - указател към последния елемент, който да се унищожи (изключващо)
     */
    static void destroy_range(T* begin, T* end) // destroy [begin, end)
    {
        for(;begin != end; ++begin)
        {
            begin->~T();
        }
    }

    /**
     *  Помощна функция за преместване на елементите в интервала [begin, end)
     *  с една позиция надясно. Функцията се използва за реализиране на insert метода.
     *
     *  @param  begin   - указател към първия елемент от интервала (включително)
     *  @param  end     - указател към последния елемент от интервала (изключващо)
     */
    static void swap_range_backwards(T* begin, T* end)
    {
        T* left = end - 1;
        while(end != begin)
        {
            std::swap(*(left--), *(end--));
        }
    }

};

/**
 *  Конструктор, който не изисква от типа T да има има конструктор по подразбиране,
 *  а използва копиращия конструктор на T, за конструира count на брой елемента
 *  със стойността val.
 *
 *  @param  count   - брой на елементите във вектора
 *  @param  val     - стойност, с която да бъдат инициализирани елементите
 *  @param  alloc   - алокатор, който да се грижи за управлението на паметта
 */
template<typename T, typename A>
Vector<T, A>::Vector(size_t count, const T& val, const A& alloc)
    : base(alloc, count)
{
    std::uninitialized_fill(base.first, base.first + count, val);
}

/**
 *  Тривиален копиращ конструктор, който използва std::uninitialized_copy,
 *  за да копира всички елементи от подадения вектор в *this.
 */
template<typename T, typename A>
Vector<T, A>::Vector(const Vector& other)
    : base(other.base.alloc, other.size())
{
    std::uninitialized_copy(other.base.first, other.base.space, base.first);
}

/**
 *  Оператор за копиращи присвояване, който използва copy-and-swap идиома.
 *  Преизползва се копиращият конструктор, за да се построи временен обект
 *  (копие на подадения аргумент), чието съдържание се разменя със *this.
 *  *this вече е копие на аргумента, а на старото съдържание, което вече е
 *  в temp, се извиква неявно деструкторът.
 */
template<typename T, typename A>
Vector<T, A>& Vector<T, A>::operator=(const Vector& other)
{
    Vector<T, A> temp(other);
    std::swap(*this, temp);
    return *this;
}

/**
 *  Тривиален преместващ конструктор.
 */
template<typename T, typename A>
Vector<T, A>::Vector(Vector&& other)
    : base(std::move(other.base))
{}

/**
 *  Тривиален оператор за преместващо присвояване.
 */
template<typename T, typename A>
Vector<T, A>& Vector<T, A>::operator=(Vector&& other)
{
    clear();
    base = std::move(other.base);
    return *this;
}

/**
 *  процедура за унищожаване на всички елементи на вектора
 */
template<typename T, typename A>
void Vector<T, A>::clear()
{
    for (T* cursor = base.first; cursor != base.space; ++cursor)
    {
        cursor->~T();
    }
    base.space = base.first;        // краят на елементите съвпада с началото
}

/**
 *  Ако new_size <= моментния капацитет, функцията приключва.
 *  В противен случай създава нов помощен вектор, в който премества всички елементи
 *  на this->base, разменя новия помощен вектор със this->base, който вече е празен и
 *  съответно му се извиква деструктора.
 *
 *  @param  new_size    -   естествено число, нов капацитет на вектора
 */
template<typename T, typename A>
void Vector<T, A>::reserve(size_t new_capacity)
{
    if (new_capacity <= capacity())
        return;

    VectorBase<T, A> temp(base.alloc, size(), new_capacity - size());
    uninitialized_move(base.first, base.first + size(), temp.first);
    std::swap(base, temp);
}

/**
 *  Процедура, която се грижи за оразмеряването на вектора.
 *  Използва reserve, за да подсигури нужния капацитет. Ако новия размер на вектора
 *  е по-голям от предишния (вектора е уголемен), свободния капацитет се допълва с
 *  елементи, конструирани по образа на val. В противен случай, когато вектора е смален,
 *  излишните елементи се унищожават. Накрая space = last, защото капацитета е запълнен.
 *
 *  @param  new_size    -   естествено число, нов брой на елементите на вектора
 *  @param  val         -   стойност, с която да се инициализират новодобавените елементи
 */
template<class T, class A>
void Vector<T,A>::resize(size_t new_size, const T& val)
{
    reserve(new_size);

    if (size() < new_size)
    {
        // конструирай нови елементи в интервала: [size(), new_size)
        std::uninitialized_fill(base.first + size(), base.first + new_size, val);
    }
    else
    {
        // унищожи елементите в интервала: [new_size, size())
        destroy_range(base.first + new_size, base.first + size());
    }

    base.space = base.last = base.first + new_size;
}

/**
 *  push_back се грижи за добавяне на нов елемент към вектора в задния край.
 *  В случай, че капацитета е изчерпан се заделя допълнителен капацитет, който
 *  се определя от множителя на растене GROWTH_RATE. Ако векторът е бил празен,
 *  се заделя INITIAL_SIZE капацитет. Новият елемент се конструира от алокатора.
 *  Накрая се увеличава указателят, който маркира новия край на елементите.
 *
 *  @param  new_size    -   естествено число, нов брой на елементите на вектора
 *  @param  val         -   стойност, с която да се инициализират новодобавените елементи
 */
template<typename T, typename A>
void Vector<T,A>::push_back(const T& val)
{
    if (capacity() == size())                   // ако няма повече капацитет
    {
        reserve( size() ? (round(GROWTH_RATE * size())) : INITIAL_SIZE );
    }
    base.alloc.construct(base.space, val);
    ++base.space;                               // увеличи брояча на елементи
}

/**
 *  pop_back() унищожава последният елемент във вектора, ако има такъв.
 */
template<typename T, typename A>
void Vector<T,A>::pop_back()
{
    if(!empty())
    {
        base.alloc.destroy(base.space - 1);
        --base.space;
    }
}

/**
 *  В случай, че искаме да добавим последен елемент, свеждаме решението до
 *  push_back. В противен случай се осигурява нужния капацитет.
 *  Конструира се елемента, който искаме да вмъкнем на позиция space
 *  (вече нов последен елемент), а swap_range_backwards чрез последователни
 *  съседни смени го измества на позиция index.
 *
 *  @param  index   -   позиция на вмъкване във вектора
 *  @param  value   -   стойност, която да се вмъкне във вектора
 */
template<typename T, typename A>
void Vector<T,A>::insert(int index, const T& value)
{
    if (index == size() + 1)
    {
        return push_back(value);
    }

    if (size() == capacity())
    {
        reserve( size() ? (round(GROWTH_RATE * size())) : INITIAL_SIZE );
    }

    base.alloc.construct(base.space, value);
    swap_range_backwards(base.first + index, base.space);
    ++base.space;
}

/**
 *  erase трие елементът на позиция index, ако има елементи въобще.
 *  елементът се унищожава от алокатора, а std::move се грижи да
 *  запази инварианта на последователност на елементите на вектора,
 *  като измести всички елементи вдясно от изтрития една позиция наляво.
 *
 *  @param  index   -   позицията на елемента, който ще бъде изтрит
 */
template<typename T, typename A>
void Vector<T,A>::erase(int index)
{
    if (empty())
    {
        return;
    }

    base.alloc.destroy(base.first + index);
    std::move(base.first + index + 1, base.space, base.first + index);  // премести наляво
    --base.space;
}

/**
 *  Процедура за смаляване на капацитета до броя на елементите във вектора.
 *  Проста реализация чрез copy-and-swap идиома.
 */
template<typename T, typename A>
void Vector<T, A>::shrink_to_fit()
{
    Vector<T, A> temp(*this);
    swap(*this, temp);
}

/**
 *  Тривиална имплементация на swap за два вектора.
 */
template<typename T, typename A>
void swap(Vector<T, A>& a, Vector<T, A>& b)
{
    std::swap(a, b);
}

#endif // VECTOR_H
