#include <iostream>
#include <foo/add.h>

int foo_add(int a, int b)
{
#if FOO_DEBUG
    std::cout << "foo: debug build!" << std::endl;
#endif
    return a + b;
}
