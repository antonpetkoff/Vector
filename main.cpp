#include <iostream>
#include "VectorBase.h"

int main()
{
    std::allocator<int> alloc;
    VectorBase<int> vectorBase(alloc, 10);
    return 0;
}
