#include <chrono>
#include <thread>

int
main(int, char **)
{
    while(true)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    return 0;
}
