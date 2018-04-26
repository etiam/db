#include <iostream>
#include <string>

class A
{
  public:
    static std::string type;
};

class B : public A
{
  public:
    static std::string type;
};

std::string A::type = "a";
std::string B::type = "b";

int
main(int, char **)
{
    std::cout << A::type << std::endl;
    std::cout << B::type << std::endl;
    return 0;
}
