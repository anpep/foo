#include <iostream>
#include <foo/add.h>

int main(int argc, char *argv[])
{
    std::cout << "foo: hello from main!" << std::endl;
    std::cout << foo_add(3, 2) << std::endl;
    return 0;
}
